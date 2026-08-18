#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// ==========================================
// FIR FILTER CONFIGURATION (Fixed-Point)
// ==========================================
#define FILTER_TAPS 11

// Circular history buffers using signed 16-bit integers
int16_t sample_history[FILTER_TAPS] = { 0 };
int history_index = 0;

/* 
 * FIXED-POINT SCALING FACTOR: Q14 Format
 * All coefficients have been multiplied by 16,384 (2^14).
 * This maximizes precision while preventing 16-bit bitwise overflow during multiplication.
 */
const int16_t coeff_ch1[FILTER_TAPS] = {
  0, 0, 0, 0, 0, 16384, 0, 0, 0, 0, 0  // Reference delay (Equivalent to 1.0)
};

const int16_t coeff_ch2[FILTER_TAPS] = {
  -819, 0, -1966, 0, -9994, 0, 9994, 0, 1966, 0, 819  // Hilbert 90° Phase (Scaled placeholders)
};

// ==========================================
// FAST INTEGER SINE-WAVE GENERATOR (Q14)
// ==========================================
// A 16-step lookup table for a 488.28 Hz Sine Wave (7812.5 Hz / 16 steps)
// Scaled to Q14 max amplitudes (-16384 to 16384)
#define SINE_STEPS 16
const int16_t sine_table[SINE_STEPS] PROGMEM = {
  0, 6270, 11585, 15137, 16384, 15137, 11585, 6270,
  0, -6270, -11585, -15137, -16384, -15137, -11585, -6270
};
volatile uint8_t table_index = 0;

void setup() {
  noInterrupts();
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  // Setup Timer 3 for Fast 8-bit PWM Audio DAC on the dedicated OC3A pin on LQFP48 Nano
  TCCR3A = (1 << COM3A1) | (1 << WGM30);
  TCCR3B = (1 << WGM32) | (1 << CS30);  // Prescaler 1
  PMX0 = (1 << WCE);                    // unlock PMX writes
  PMX1 = (1 << C3AC);                   // route OC3A to the physical OC3A pin
  OCR3AL = 127;

  pinMode(9, OUTPUT);
  TCCR1A = (1 << COM1A1) | (1 << WGM10);
  TCCR1B = (1 << WGM12) | (1 << CS10);

  // 2. SETUP SAMPLING ENGINE: Timer 0 Compare Match B at 7812.5 Hz
  TCCR0B = (TCCR0B & 0xF8) | (1 << CS01);  // Prescaler 8
  OCR0B = 255;
  TIMSK0 |= (1 << OCIE0B);

  interrupts();
}

// ==========================================
// REAL-TIME FIXED-POINT PROCESSOR
// ==========================================
ISR(TIMER0_COMPB_vect) {
  PORTB |= (1 << PORTB5);
  // STEP A: Fetch Integer Test Signal from PROGMEM (Flash Memory)
  int16_t current_input = pgm_read_word(&sine_table[table_index]);

  table_index++;
  if (table_index >= SINE_STEPS) {
    table_index = 0;
  }

  // STEP B: Update the Circular History Buffer
  sample_history[history_index] = current_input;

  // STEP C: Process FIR Accumulation using 32-bit Integers (Prevents Math Overflow)
  int32_t accumulator_ch1 = 0;
  int32_t accumulator_ch2 = 0;
  int tap_ptr = history_index;

  for (int i = 0; i < FILTER_TAPS; i++) {
    // Integer Multiplication (Executed instantly via hardware multiplier)
    accumulator_ch1 += (int32_t)sample_history[tap_ptr] * coeff_ch1[i];
    accumulator_ch2 += (int32_t)sample_history[tap_ptr] * coeff_ch2[i];

    // Roll backwards through history buffer
    tap_ptr--;
    if (tap_ptr < 0) {
      tap_ptr = FILTER_TAPS - 1;
    }
  }

  // Advance history index for next clock tick
  history_index++;
  if (history_index >= FILTER_TAPS) {
    history_index = 0;
  }

  // STEP D: Downshift from Q28 back to Unsigned 8-Bit PWM
  /*
   * Math breakdown:
   * 1. The input is Q14, the coefficients are Q14 -> Result is Q28 format.
   * 2. Shifting right by 14 bits (`>> 14`) returns it back to a standard Q14 range (-16384 to 16384).
   * 3. Shifting right by another 7 bits (`>> 7`) normalizes it to an 8-bit signed scale (-128 to 127).
   * 4. Adding 128 offsets it to the unsigned 0 to 255 required by the PWM register.
   * Combined step: Shift right by 21 bits total.
   */
  int16_t pwm_ch1 = (int16_t)(accumulator_ch1 >> 21) + 128;
  int16_t pwm_ch2 = (int16_t)(accumulator_ch2 >> 21) + 128;

  // Hard clipping protection bounds check
  if (pwm_ch1 > 255) pwm_ch1 = 255;
  else if (pwm_ch1 < 0) pwm_ch1 = 0;
  if (pwm_ch2 > 255) pwm_ch2 = 255;
  else if (pwm_ch2 < 0) pwm_ch2 = 0;

  // STEP E: Write to outputs simultaneously
  OCR3AL = (uint8_t)pwm_ch1;  // Channel 1 (PE2)
  OCR1AL = (uint8_t)pwm_ch2;  // Channel 2 (D9)
  PORTB &= ~(1 << PORTB5);
}

void loop() {
  // Free processing space
}
