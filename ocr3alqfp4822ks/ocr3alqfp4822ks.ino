#include <avr/io.h>
#include <avr/interrupt.h>

// High-Fidelity 16-bit Sine Wave Table (64 samples)
const uint16_t audio_sample_table_16bit[] PROGMEM = {
  32768, 35980, 39161, 42280, 45307, 48214, 50972, 53555, 
  55938, 58097, 60011, 61661, 63032, 64111, 64888, 65355, 
  65510, 65355, 64888, 64111, 63032, 61661, 60011, 58097, 
  55938, 53555, 50972, 48214, 45307, 42280, 39161, 35980, 
  32768, 29555, 26374, 23255, 20228, 17321, 14563, 11980, 
   9597,  7438,  5524,  3874,  2503,  1424,   647,   180, 
     25,   180,   647,  1424,  2503,  3874,  5524,  7438, 
   9597, 11980, 14563, 17321, 20228, 23255, 26374, 29555
};

volatile uint16_t sample_index = 0;
volatile int32_t error_accumulator = 0; 

// Try altering this target value now:
// 16000 -> 16 kHz sample rate
// 22050 -> 22.05 kHz sample rate
// 44100 -> 44.1 kHz sample rate
#define SAMPLE_RATE_HZ 22050 

void setup() {
  noInterrupts(); 
  pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

  // =========================================================
  // 🏎️ THE CRITICAL CPU CLOCK FIX: FORCE TRUE 32 MHz LOGIC
  // =========================================================
  // Bypasses the board variant core defaults by modifying the system clock divider
  CLKPR = (1 << CLKPCE); // Step 1: Open the clock change enable window
  CLKPR = 0x00;          // Step 2: Clear all dividers -> Forces true 32 MHz operation!

  // -------------------------------------------------------
  // 1. TIMER 3: 125 kHz HIGH-SPEED PWM DAC (Carrier changes to 125kHz at 32MHz)
  // -------------------------------------------------------
  TCCR3A = (1 << COM3A1) | (1 << WGM31) | (1 << WGM30);
  TCCR3B = (1 << CS30); 
  OCR3A = 128; 

  // Matrix Pin Allocation (Your proven hardware connection)
  PMX0 = (1 << WCE);   
  PMX1 = (1 << C3AC); 
  DDRC |= (1 << DDC0);

  // -------------------------------------------------------
  // 2. TIMER 1: METRONOME SAMPLING ENGINE
  // -------------------------------------------------------
  TCCR1A = 0; 
  TCCR1B = (1 << WGM12) | (1 << CS10); // Mode 4: CTC Mode

  // Explicitly calculate deadline values using true 32 MHz speeds
  OCR1A = (32000000UL / SAMPLE_RATE_HZ) - 1; 

  TIMSK1 |= (1 << OCIE1A); 

  interrupts(); 
}

ISR(TIMER1_COMPA_vect) {
  PORTB |= (1 << PORTB5); // 🟢 PIN HIGH: Mark the exact start of the ISR execution

  uint16_t sample_16bit = pgm_read_word(&(audio_sample_table_16bit[sample_index]));

  // Delta-Sigma Noise Shaping Calculation
  int32_t output_pipeline = (int32_t)sample_16bit + error_accumulator;
  int16_t hardware_8bit_value = output_pipeline >> 8;

  if (hardware_8bit_value > 255) hardware_8bit_value = 255;
  else if (hardware_8bit_value < 0) hardware_8bit_value = 0;

  error_accumulator = output_pipeline - ((int32_t)hardware_8bit_value << 8);
  OCR3A = (uint8_t)hardware_8bit_value; 

  sample_index++;
  if (sample_index >= 64) {
    sample_index = 0;
  }

  PORTB &= ~(1 << PORTB5); // 🔴 PIN LOW: Mark the exact completion of the ISR
}


void loop() {
  // Free loop execution space
}
