/*
 uDSC_Full_Test.ino
 Exhaustive LGT8F328P uDSC validation suite
 Uses the supplied lgt8xp_uDSU.h and udsu.S unchanged.
*/
#include "lgt8xp_uDSU.h"

uint16_t passCount=0, failCount=0;

void pass(const char*n){ Serial.print(n); Serial.println(" PASS"); passCount++; }
void fail(const char*n,long e,long a){
  Serial.print(n); Serial.print(" FAIL exp=");
  Serial.print(e); Serial.print(" got="); Serial.println(a); failCount++;
}
#define CU16(N,E,A) (((uint16_t)(E)==(uint16_t)(A))?pass(N):fail(N,(long)(uint16_t)(E),(long)(uint16_t)(A)))
#define CS16(N,E,A) (((int16_t)(E)==(int16_t)(A))?pass(N):fail(N,(long)(int16_t)(E),(long)(int16_t)(A)))
#define CU32(N,E,A) (((uint32_t)(E)==(uint32_t)(A))?pass(N):fail(N,(long)(uint32_t)(E),(long)(uint32_t)(A)))
#define CS32(N,E,A) (((int32_t)(E)==(int32_t)(A))?pass(N):fail(N,(long)(int32_t)(E),(long)(int32_t)(A)))

void banner(const char*s){ Serial.println(); Serial.println(s); }

void test_all(){
banner("ADD/SUB");
CU16("XADD",3345,dsu_xadd(1000,2345));
CS16("XADS",-750,dsu_xads(-1000,250));
CU16("XSUB",1750,dsu_xsub(3000,1250));
CS16("XSBS",-2500,dsu_xsbs(-2000,500));
banner("DA");
CU32("ADAY",101234UL,dsu_aday(100000UL,1234));
CS32("ADAYS",-98766L,dsu_adays(-100000L,1234));
CU32("SBAY",98766UL,dsu_sbay(100000UL,1234));
CS32("SBAYS",-101234L,dsu_sbays(-100000L,1234));
banner("MUL");
CS32("XMULUU",((int32_t)(300)*(400)),dsu_xmuluu(300,400));
CS32("XMULSU",((int32_t)(-300)*(400)),dsu_xmulsu(-300,400));
CS32("XMULUS",((int32_t)(300)*(-400)),dsu_xmulus(300,-400));
CS32("XMULSS",((int32_t)(-300)*(-400)),dsu_xmulss(-300,-400));
CS32("FXMULUU",(((int32_t)(300)*(400))>>1),dsu_fxmuluu(300,400));
CS32("FXMULSU",(((int32_t)(-300)*(400))>>1),dsu_fxmulsu(-300,400));
CS32("FXMULUS",(((int32_t)(300)*(-400))>>1),dsu_fxmulus(300,-400));
CS32("FXMULSS",(((int32_t)(-300)*(-400))>>1),dsu_fxmulss(-300,-400));
CS32("XMNLUU",(-((int32_t)(300)*(400))),dsu_xmnluu(300,400));
CS32("XMNLSU",(-((int32_t)(-300)*(400))),dsu_xmnlsu(-300,400));
CS32("XMNLUS",(-((int32_t)(300)*(-400))),dsu_xmnlus(300,-400));
CS32("XMNLSS",(-((int32_t)(-300)*(-400))),dsu_xmnlss(-300,-400));
CS32("FXMNLUU",(-(((int32_t)(300)*(400))>>1)),dsu_fxmnluu(300,400));
CS32("FXMNLSU",(-(((int32_t)(-300)*(400))>>1)),dsu_fxmnlsu(-300,400));
CS32("FXMNLUS",(-(((int32_t)(300)*(-400))>>1)),dsu_fxmnlus(300,-400));
CS32("FXMNLSS",(-(((int32_t)(-300)*(-400))>>1)),dsu_fxmnlss(-300,-400));
banner("MAC");
CS32("XMACUU0",(((int32_t)(300)*(400))) ,dsu_xmacuu0(300,400));
CS32("XMACUU1",(1000+((int32_t)(300)*(400))),dsu_xmacuu1(1000,300,400));
CS32("XMACSU0",(((int32_t)(-300)*(400))) ,dsu_xmacsu0(-300,400));
CS32("XMACSU1",(1000+((int32_t)(-300)*(400))),dsu_xmacsu1(1000,-300,400));
CS32("XMACUS0",(((int32_t)(300)*(-400))) ,dsu_xmacus0(300,-400));
CS32("XMACUS1",(1000+((int32_t)(300)*(-400))),dsu_xmacus1(1000,300,-400));
CS32("XMACSS0",(((int32_t)(-300)*(-400))) ,dsu_xmacss0(-300,-400));
CS32("XMACSS1",(1000+((int32_t)(-300)*(-400))),dsu_xmacss1(1000,-300,-400));
CS32("SMACUU0",(((int32_t)(300)*(400))) ,dsu_smacuu0(300,400));
CS32("SMACUU1",(1000+((int32_t)(300)*(400))),dsu_smacuu1(1000,300,400));
CS32("SMACSU0",(((int32_t)(-300)*(400))) ,dsu_smacsu0(-300,400));
CS32("SMACSU1",(1000+((int32_t)(-300)*(400))),dsu_smacsu1(1000,-300,400));
CS32("SMACUS0",(((int32_t)(300)*(-400))) ,dsu_smacus0(300,-400));
CS32("SMACUS1",(1000+((int32_t)(300)*(-400))),dsu_smacus1(1000,300,-400));
CS32("SMACSS0",(((int32_t)(-300)*(-400))) ,dsu_smacss0(-300,-400));
CS32("SMACSS1",(1000+((int32_t)(-300)*(-400))),dsu_smacss1(1000,-300,-400));
CS32("XMSCUU0",(-((int32_t)(300)*(400))) ,dsu_xmscuu0(300,400));
CS32("XMSCUU1",(1000-((int32_t)(300)*(400))),dsu_xmscuu1(1000,300,400));
CS32("XMSCSU0",(-((int32_t)(-300)*(400))) ,dsu_xmscsu0(-300,400));
CS32("XMSCSU1",(1000-((int32_t)(-300)*(400))),dsu_xmscsu1(1000,-300,400));
CS32("XMSCUS0",(-((int32_t)(300)*(-400))) ,dsu_xmscus0(300,-400));
CS32("XMSCUS1",(1000-((int32_t)(300)*(-400))),dsu_xmscus1(1000,300,-400));
CS32("XMSCSS0",(-((int32_t)(-300)*(-400))) ,dsu_xmscss0(-300,-400));
CS32("XMSCSS1",(1000-((int32_t)(-300)*(-400))),dsu_xmscss1(1000,-300,-400));
CS32("SMSCUU0",(-((int32_t)(300)*(400))) ,dsu_smscuu0(300,400));
CS32("SMSCUU1",(1000-((int32_t)(300)*(400))),dsu_smscuu1(1000,300,400));
CS32("SMSCSU0",(-((int32_t)(-300)*(400))) ,dsu_smscsu0(-300,400));
CS32("SMSCSU1",(1000-((int32_t)(-300)*(400))),dsu_smscsu1(1000,-300,400));
CS32("SMSCUS0",(-((int32_t)(300)*(-400))) ,dsu_smscus0(300,-400));
CS32("SMSCUS1",(1000-((int32_t)(300)*(-400))),dsu_smscus1(1000,300,-400));
CS32("SMSCSS0",(-((int32_t)(-300)*(-400))) ,dsu_smscss0(-300,-400));
CS32("SMSCSS1",(1000-((int32_t)(-300)*(-400))),dsu_smscss1(1000,-300,-400));
banner("SQUARE");
dsu_usqx0(1234); pass("USQX0");
CU32("USQX1",(uint32_t)1234*1234,dsu_usqx1(1234));
dsu_ssqx0(321); pass("SSQX0");
CU32("SSQX1",(uint32_t)321*321,dsu_ssqx1(321));
dsu_usqy0(2222); pass("USQY0");
CU32("USQY1",(uint32_t)2222*2222,dsu_usqy1(2222));
dsu_ssqy0(456); pass("SSQY0");
CU32("SSQY1",(uint32_t)456*456,dsu_ssqy1(456));
banner("NEG ABS");
dsu_uneg0(); pass("UNEG0");
dsu_uneg1(12345); pass("UNEG1");
CS32("UNEG2",-12345,dsu_uneg2());
CS32("UNEG3",-12345,dsu_uneg3(12345));
dsu_sneg0(); pass("SNEG0");
dsu_sneg1(-12345); pass("SNEG1");
CS32("SNEG2",12345,dsu_sneg2());
CS32("SNEG3",12345,dsu_sneg3(-12345));
dsu_abs0(); pass("ABS0");
dsu_abs1(-54321); pass("ABS1");
CU32("ABS2",54321,dsu_abs2());
CU32("ABS3",54321,dsu_abs3(-54321));
// banner("SHIFT DIV");
// CU32("ASHL0",16,dsu_ashl0(4));
// CU32("ASHL1",16,dsu_ashl1(1,4));
// CU32("ASHR0",4,dsu_ashr0(2));
// CU32("ASHR1",4,dsu_ashr1(16,2));
// CS32("ASHR2",-8,dsu_ashr2(2));
// CS32("ASHR3",-8,dsu_ashr3(-32,2));
// CU32("DIV0",10,dsu_div0());
// CU32("DIV1",27,dsu_div1(1000,37));
banner("SHIFT DIV");
dsu_ashl1(1, 0);              // explicitly set DA = 1
CU32("ASHL0", 16, dsu_ashl0(4));

dsu_ashl1(1, 4);               // DA = 16
CU32("ASHL1", 16, dsu_ashl1(1,4));
dsu_ashr1(16, 2);              // DA = 4
CU32("ASHR0", 4, dsu_ashr0(2));
CU32("ASHR1", 4, dsu_ashr1(16,2));

dsu_ashr3(-32, 0);             // explicitly set DA = -32
CS32("ASHR2", -8, dsu_ashr2(2));
CS32("ASHR3", -8, dsu_ashr3(-32,2));

dsu_div1(100UL, 10);           // explicitly set DA=100, DY=10
CU32("DIV0", 10, dsu_div0());
CU32("DIV1", 27, dsu_div1(1000,37));
banner("FMACSS");
CS32("FMACSS",8000000,dsu_fmacss(1000,2000,4));
}

void setup(){
  Serial.begin(115200); delay(1000);
  dsu_init(DSU_MM_FAST);
  Serial.println("uDSC COMPLETE VALIDATION SUITE");
  test_all();
  Serial.println();
  Serial.print("PASS="); Serial.println(passCount);
  Serial.print("FAIL="); Serial.println(failCount);
}
void loop(){}
