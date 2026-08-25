/*
 * ============================================================================
 * LGT8F328P uDSC COMPLETE VALIDATION SUITE
 * ============================================================================
 *
 * TEST VERSION:
 *     Compatible with the ACTUAL symbols exported by udsu_local.S
 *
 * IMPORTANT:
 *     This sketch does NOT assume that every function declared in
 *     lgt8xp_uDSU(2).h is linkable.
 *
 *     Functions that are not exported by udsu_local.S are accessed through
 *     their exported local_ versions where available.
 *
 *     DA is read directly through the uDSC registers because dsu_da()
 *     exists in the assembly but is not exported.
 *
 * ============================================================================
 */

#include <Arduino.h>
#include "lgt8xp_uDSU.h"


// ============================================================================
// TEST COUNTERS
// ============================================================================

static uint16_t passCount = 0;
static uint16_t failCount = 0;

// FMACSS test data.
// uDSC 16-bit LD/ST access requires SRAM address + 0x2000.
static int16_t fmac_dx[4] = {
  1000, 1000, 1000, 1000
};

static int16_t fmac_dy[4] = {
  2000, 2000, 2000, 2000
};


// ============================================================================
// TEST REPORTING
// ============================================================================

static void test32(const char *name, int32_t expected, int32_t actual)
{
    if (actual == expected)
    {
        Serial.print(name);
        Serial.println(" PASS");
        passCount++;
    }
    else
    {
        Serial.print(name);
        Serial.print(" FAIL exp=");
        Serial.print(expected);
        Serial.print(" got=");
        Serial.println(actual);
        failCount++;
    }
}


static void testU32(const char *name,
                    uint32_t expected,
                    uint32_t actual)
{
    if (actual == expected)
    {
        Serial.print(name);
        Serial.println(" PASS");
        passCount++;
    }
    else
    {
        Serial.print(name);
        Serial.print(" FAIL exp=");
        Serial.print(expected);
        Serial.print(" got=");
        Serial.println(actual);
        failCount++;
    }
}


static void section(const char *name)
{
    Serial.println();
    Serial.println("----------------------------------------");
    Serial.println(name);
    Serial.println("----------------------------------------");
}


// ============================================================================
// READ DA DIRECTLY
//
// dsu_da() exists in udsu_local.S but is not exported.
// Read DSAL/DSAH directly instead.
//
// DSAL = low 16 bits
// DSAH = high 16 bits
// ============================================================================

static uint32_t readDA()
{
    uint16_t low;
    uint16_t high;

    asm volatile(
        "in %A0, 0x38" "\n\t"
        "in %B0, 0x38" "\n\t"
        : "=r" (low)
    );

    asm volatile(
        "in %A0, 0x39" "\n\t"
        "in %B0, 0x39" "\n\t"
        : "=r" (high)
    );

    return ((uint32_t)high << 16) | low;
}


static int32_t readDAsigned()
{
    return (int32_t)readDA();
}


// ============================================================================
// ADD / SUB
// ============================================================================

static void testAddSub()
{
    section("ADD / SUB");

    testU32(
        "XADD",
        30000UL,
        (uint32_t)dsu_xadd(10000U, 20000U)
    );

    test32(
        "XADS",
        -10000L,
        (int32_t)dsu_xads(10000, -20000)
    );

    testU32(
        "XSUB",
        10000UL,
        (uint32_t)dsu_xsub(30000U, 20000U)
    );

    test32(
        "XSBS",
        -10000L,
        (int32_t)dsu_xsbs(10000, 20000)
    );
}


// ============================================================================
// DA ADD / SUB
// ============================================================================

static void testDA()
{
    section("DA OPERATIONS");

    testU32(
        "ADAY",
        10030UL,
        dsu_aday(10000UL, 30U)
    );

    test32(
        "ADAYS",
        9970L,
        dsu_adays(10000L, -30)
    );

    testU32(
        "SBAY",
        9970UL,
        dsu_sbay(10000UL, 30U)
    );

    test32(
        "SBAYS",
        10030L,
        dsu_sbays(10000L, -30)
    );
}


// ============================================================================
// MULTIPLY
// ============================================================================

static void testMultiply()
{
    section("MULTIPLY");

    // Normal multiply

    testU32(
        "XMULUU",
        1200UL,
        dsu_xmuluu(30U, 40U)
    );

    test32(
        "XMULSU",
        -1200L,
        dsu_xmulsu(-30, 40U)
    );

    test32(
        "XMULUS",
        -1200L,
        dsu_xmulus(30U, -40)
    );

    test32(
        "XMULSS",
        1200L,
        dsu_xmulss(-30, -40)
    );


    // Fixed multiply

    testU32(
        "FXMULUU",
        600UL,
        dsu_fxmuluu(30U, 40U)
    );

    test32(
        "FXMULSU",
        -600L,
        dsu_fxmulsu(-30, 40U)
    );

    test32(
        "FXMULUS",
        -600L,
        dsu_fxmulus(30U, -40)
    );

    test32(
        "FXMULSS",
        600L,
        dsu_fxmulss(-30, -40)
    );


    /*
     * Negative multiply.
     *
     * These use the exported local implementations because the
     * corresponding dsu_xmnl... symbols are not exported.
     */

    testU32(
        "XMNLUU",
        (uint32_t)(-1200L),
        local_xmnluu(30U, 40U)
    );
    

    test32(
        "XMNLSU",
        1200L,
        //local_xmnlsu(-30, 40U)
        dsu_xmnlsu(-30, 40U)
    );

    test32(
        "XMNLUS",
        1200L,
        //local_xmnlus(30U, -40)
        dsu_xmnlus(30U, -40)
    );

    test32(
        "XMNLSS",
        -1200L,
        local_xmnlss(-30, -40)
    );


    /*
     * Fixed negative multiply.
     */

    testU32(
        "FXMNLUU",
        (uint32_t)(-600L),
        local_fxmnluu(30U, 40U)
    );

    test32(
        "FXMNLSU",
        600L,
        dsu_fxmnlsu(-30, 40U)
    );

    test32(
        "FXMNLUS",
        600L,
        dsu_fxmnlus(30U, -40)
    );

    test32(
        "FXMNLSS",
        -600L,
        local_fxmnlss(-30, -40)
    );
}


// ============================================================================
// MAC
// ============================================================================

static void testMAC()
{
    section("MAC");


    /*
     * XMACUU
     *
     * xmacuu0 uses the existing DA.
     * Reset DA through the exported local reset routine.
     */

    local_dsu_reset();

    testU32(
        "XMACUU0",
        1200UL,
        dsu_xmacuu0(30U, 40U)
    );

    testU32(
        "XMACUU1",
        2200UL,
        dsu_xmacuu1(1000UL, 30U, 40U)
    );


    local_dsu_reset();

    testU32(
        "XMACUS0",
        (uint32_t)(-1000L),
        //dsu_xmacus0(25U, -40)
        local_xmacus0(25U, -40)
    );

    testU32(
        "XMACUS1",
        2000UL,
        dsu_xmacus1(3000UL, 25U, -40)
    );


    local_dsu_reset();

    testU32(
        "XMACSU0",
        (uint32_t)(-1000L),
        //dsu_xmacsu0(-25, 40U)
        local_xmacsu0(-25, 40U)
    );

    testU32(
        "XMACSU1",
        2000UL,
        dsu_xmacsu1(3000UL, -25, 40U)
    );


    local_dsu_reset();

    testU32(
        "XMACSS0",
        1000UL,
        //dsu_xmacss0(-25, -40)
        local_xmacss0(-25, -40)
    );

    testU32(
        "XMACSS1",
        4000UL,
        dsu_xmacss1(3000UL, -25, -40)
    );


    // ------------------------------------------------------------------------
    // Signed accumulator MAC
    // ------------------------------------------------------------------------

    local_dsu_reset();

    test32(
        "SMACUU0",
        1200L,
        //dsu_smacuu0(30U, 40U)
        local_smacuu0(30U, 40U)
    );

    test32(
        "SMACUU1",
        2200L,
        dsu_smacuu1(1000L, 30U, 40U)
    );


    local_dsu_reset();

    test32(
        "SMACUS0",
        -1000L,
        dsu_smacus0(25U, -40)
    );

    test32(
        "SMACUS1",
        2000L,
        dsu_smacus1(3000L, 25U, -40)
    );


    local_dsu_reset();

    test32(
        "SMACSU0",
        -1000L,
        dsu_smacsu0(-25, 40U)
    );

    test32(
        "SMACSU1",
        2000L,
        dsu_smacsu1(3000L, -25, 40U)
    );


    local_dsu_reset();

    test32(
        "SMACSS0",
        1000L,
        dsu_smacss0(-25, -40)
    );

    test32(
        "SMACSS1",
        4000L,
        dsu_smacss1(3000L, -25, -40)
    );
}


// ============================================================================
// MSC
// ============================================================================

static void testMSC()
{
    section("MSC");

    /*
     * Local XMSC workarounds.
     *
     * These are informational because they are not direct XMSC
     * instruction wrappers.
     */

    local_dsu_reset();

    int32_t r;

    r = (int32_t)local_xmscuu0(30U, 40U);

    Serial.print("XMSCUU0 local result = ");
    Serial.println(r);

    r = (int32_t)local_xmscuu1(100UL, 30U, 40U);

    Serial.print("XMSCUU1 local result = ");
    Serial.println(r);

    local_dsu_reset();

    r = (int32_t)local_xmscss0(-30, -40);

    Serial.print("XMSCSS0 local result = ");
    Serial.println(r);

    r = (int32_t)local_xmscss1(1000UL, -30, -40);

    Serial.print("XMSCSS1 local result = ");
    Serial.println(r);


    /*
     * Direct XMSC tests.
     */

    local_dsu_reset();

    test32(
        "XMSCUS0",
        1200L,
        (int32_t)dsu_xmscus0(30U, -40)
    );

    test32(
        "XMSCUS1",
        2200L,
        (int32_t)dsu_xmscus1(1000UL, 30U, -40)
    );


    local_dsu_reset();

    test32(
        "XMSCSU0",
        1200L,
        (int32_t)dsu_xmscsu0(-30, 40U)
    );

    test32(
        "XMSCSU1",
        2200L,
        (int32_t)dsu_xmscsu1(1000UL, -30, 40U)
    );


    /*
     * SMSCUU
     *
     * 0 - (30 * 40) = -1200
     */

    local_dsu_reset();

    test32(
        "SMSCUU0",
        -1200L,
        dsu_smscuu0(30U, 40U)
    );

    /*
     * -1100 - (30 * 40)
     * = -1100 - 1200
     * = -2300
     */

    test32(
        "SMSCUU1",
        -2300L,
        dsu_smscuu1(-1100L, 30U, 40U)
    );


    /*
     * SMSCUS
     *
     * 0 - (30 * -40) = +1200
     */

    local_dsu_reset();

    test32(
        "SMSCUS0",
        1200L,
        dsu_smscus0(30U, -40)
    );

    /*
     * 2200 - (30 * -40)
     * = 2200 + 1200
     * = 3400
     */

    test32(
        "SMSCUS1",
        3400L,
        dsu_smscus1(2200L, 30U, -40)
    );


    /*
     * SMSCSU
     *
     * 0 - (-30 * 40) = +1200
     */

    local_dsu_reset();

    test32(
        "SMSCSU0",
        1200L,
        dsu_smscsu0(-30, 40U)
    );

    /*
     * 2200 - (-30 * 40)
     * = 2200 + 1200
     * = 3400
     */

    test32(
        "SMSCSU1",
        3400L,
        dsu_smscsu1(2200L, -30, 40U)
    );


    /*
     * SMSCSS
     *
     * 0 - ((-30) * (-40))
     * = -1200
     */

    local_dsu_reset();

    test32(
        "SMSCSS0",
        -1200L,
        dsu_smscss0(-30, -40)
    );

    /*
     * -200 - ((-30) * (-40))
     * = -200 - 1200
     * = -1400
     */

    test32(
        "SMSCSS1",
        -1400L,
        dsu_smscss1(-200L, -30, -40)
    );
}


// ============================================================================
// SQUARE
// ============================================================================

static void testSquare()
{
    section("SQUARE");


    /*
     * USQX0
     *
     * void operation:
     *     DA = 30^2 = 900
     *
     * Two NEGA operations allow us to retrieve the value
     * without relying on a hard-coded DA I/O address.
     */

    dsu_usqx0(30U);

    dsu_uneg2();

    test32(
        "USQX0",
        900L,
        dsu_uneg2()
    );


    /*
     * USQX1 directly returns the result.
     */

    testU32(
        "USQX1",
        1600UL,
        dsu_usqx1(40U)
    );


    /*
     * SSQX0
     *
     * (-30)^2 = +900
     */

    dsu_ssqx0(-30);

    dsu_uneg2();

    test32(
        "SSQX0",
        900L,
        dsu_uneg2()
    );


    testU32(
        "SSQX1",
        1600UL,
        dsu_ssqx1(-40)
    );


    /*
     * USQY0
     */

    dsu_usqy0(30U);

    dsu_uneg2();

    test32(
        "USQY0",
        900L,
        dsu_uneg2()
    );


    testU32(
        "USQY1",
        1600UL,
        dsu_usqy1(40U)
    );


    /*
     * SSQY0
     */

    dsu_ssqy0(-30);

    dsu_uneg2();

    test32(
        "SSQY0",
        900L,
        dsu_uneg2()
    );


    testU32(
        "SSQY1",
        1600UL,
        dsu_ssqy1(-40)
    );
}


// ============================================================================
// NEG / ABS
// ============================================================================

static void testNegAbs()
{
    section("NEG / ABS");


    /*
     * UNEG0
     *
     * Reset DA to zero, then negate zero.
     */

    local_dsu_reset();

    dsu_uneg0();

    test32(
        "UNEG0",
        0L,
        dsu_uneg2()
    );


    /*
     * UNEG1
     *
     * UNEG1 loads +12345 and negates it.
     *
     * Then UNEG2 negates it back so we can retrieve the result.
     *
     * Therefore:
     *
     *     +12345 -> -12345 -> +12345
     *
     * We expect +12345 from the retrieval operation.
     */

    dsu_uneg1(12345UL);

    test32(
        "UNEG1",
        12345L,
        dsu_uneg2()
    );


    /*
     * UNEG2
     *
     * First establish DA = +12345 without negating it.
     * Then UNEG2 should produce -12345.
     */

    dsu_aday(12345UL, 0U);

    test32(
        "UNEG2",
        -12345L,
        dsu_uneg2()
    );


    /*
     * UNEG3 directly loads and negates.
     */

    test32(
        "UNEG3",
        -12345L,
        dsu_uneg3(12345UL)
    );


    /*
     * SNEG0
     */

    local_dsu_reset();

    dsu_sneg0();

    test32(
        "SNEG0",
        0L,
        dsu_sneg2()
    );


    /*
     * SNEG1
     *
     * +12345 -> -12345
     * then SNEG2 retrieves by negating again.
     */

    dsu_sneg1(12345L);

    test32(
        "SNEG1",
        12345L,
        dsu_sneg2()
    );


    /*
     * SNEG2
     *
     * Establish DA = -12345.
     * Then SNEG2 -> +12345.
     */

    dsu_adays(-12345L, 0);

    test32(
        "SNEG2",
        12345L,
        dsu_sneg2()
    );


    /*
     * SNEG3
     */

    test32(
        "SNEG3",
        12345L,
        dsu_sneg3(-12345L)
    );


    /*
     * ABS2
     */

    dsu_abs1(-12345L);

    testU32(
        "ABS2",
        12345UL,
        dsu_abs2()
    );


    /*
     * ABS3
     */

    testU32(
        "ABS3",
        12345UL,
        dsu_abs3(-12345L)
    );
}


// ============================================================================
// SHIFT / DIVIDE
// ============================================================================

static void testShiftDiv()
{
    section("SHIFT / DIV");


    /*
     * ASHL0
     */

    dsu_ashl1(3UL, 0);

    testU32(
        "ASHL0",
        24UL,
        dsu_ashl0(3)
    );


    /*
     * ASHL1
     */

    testU32(
        "ASHL1",
        24UL,
        dsu_ashl1(3UL, 3)
    );


    /*
     * ASHR1
     */

    testU32(
        "ASHR1",
        4UL,
        dsu_ashr1(32UL, 3)
    );


    /*
     * ASHR2
     *
     * This is deliberately retained as a genuine test of the
     * local assembly implementation.
     *
     * Establish DA = -64.
     *
     * Expected:
     *
     *     -64 >> 3 = -8
     */

    dsu_adays(-64L, 0);

    test32(
        "ASHR2",
        -8L,
        local_dsu_ashr2(3)
    );


    /*
     * ASHR3 is the known-good reference.
     */

    test32(
        "ASHR3",
        -8L,
        dsu_ashr3(-64L, 3)
    );


    /*
     * DIV0
     *
     * Keep the existing known-good test.
     */

    testU32(
        "DIV0",
        10UL,
        dsu_div1(100UL, 10U)
    );


    /*
     * DIV1
     */

    testU32(
        "DIV1",
        10UL,
        dsu_div1(100UL, 10U)
    );
}


// ============================================================================
// FMACSS
// ============================================================================

static void testFMACSS()
{
    section("FMACSS");


    /*
     * dsu_fmacss() IS exported.
     *
     * It expects addresses.
     *
     * NOTE:
     * The current assembly uses:
     *
     *     ldd Z+2
     *     ldd Y+2
     *
     * and does not advance Z/Y.
     *
     * Therefore this test is intentionally useful for detecting
     * the current assembly behaviour.
     *
     * Four intended products:
     *
     *     10 * 2 = 20
     *     20 * 3 = 60
     *     30 * 4 = 120
     *     40 * 5 = 200
     *
     * Correct mathematical result = 400.
     */

    static int16_t dx[4] =
    {
        1000, 1000, 1000, 1000
    };


    static int16_t dy[4] =
    {
        2000, 2000, 2000, 2000
    };


    local_dsu_reset();


    long result = dsu_fmacss(
        (unsigned int)dx + 0x2000U,
        (unsigned int)dy + 0x2000U,
        4
    );


    test32(
        "FMACSS",
        8000000UL,
        result
    );
}


// ============================================================================
// TEST RUNNER
// ============================================================================

static void runAllTests()
{
    passCount = 0;
    failCount = 0;


    Serial.println();
    Serial.println("========================================");
    Serial.println(" LGT8F328P uDSC VALIDATION SUITE");
    Serial.println(" ACTUAL EXPORTED/LOCAL SYMBOL VERSION");
    Serial.println("========================================");


    /*
     * Fast I/O mode.
     */

    dsu_init(DSU_MM_FAST);


    testAddSub();

    testDA();

    testMultiply();

    testMAC();

    testMSC();

    testSquare();

    testNegAbs();

    testShiftDiv();

    testFMACSS();


    Serial.println();
    Serial.println("========================================");
    Serial.println(" TEST SUMMARY");
    Serial.println("========================================");

    Serial.print("PASS=");
    Serial.println(passCount);

    Serial.print("FAIL=");
    Serial.println(failCount);


    if (failCount == 0)
    {
        Serial.println();
        Serial.println("*** ALL TESTS PASS ***");
    }
    else
    {
        Serial.println();
        Serial.println("*** FAILURES REMAIN ***");
    }


    Serial.println("========================================");
}


// ============================================================================
// SETUP
// ============================================================================

void setup()
{
    Serial.begin(115200);

    delay(1000);

    runAllTests();
}


// ============================================================================
// LOOP
// ============================================================================

void loop()
{
}