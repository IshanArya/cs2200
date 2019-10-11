

@ADD: 0 add0
@NAND: 1 nand0
@ADDI: 2 addi0
@LW: 3 lw0
@SW: 4 sw0
@GOTO: 5 goto0
@JALR: 6 jalr0
@HALT: 7 halt0
@SKP: 8 skp0
@LEA: 9 lea0
@EI: 10 ei0
@DI: 11 di0
@RETI: 12 reti0
@IN: 13 in0

!TRUE: 1 skp3
!FALSE: 0 fetch0

$YEET: 1 interrupt0
$NAHB: 0 fetch1


fetch0: [OPTest, ChkCmp]
fetch1: [LdA, LdMAR, DrPC] fetch2
fetch2: [LdIR, DrMEM] fetch3
fetch3: [OPTest, ALU=3, LdPC, DrALU]

interrupt0: [DrPC, RegSel=3, WrREG, LdEnInt] interrupt1
interrupt1: [IntAck] interrupt2
interrupt2: [DrDATA, LdMAR] interrupt3
interrupt3: [LdPC, DrMEM] fetch1

in0: [DrOFF, LdDAR] in1
in1: [DrDATA, RegSel=0, WrREG] in2
in2: [RegSel=1, DrREG, LdDAR] fetch0

di0: [LdEnInt] fetch1
ei0: [EnInt, LdEnInt] fetch0

reti0: [LdPC, RegSel=3, DrREG, EnInt, LdEnInt] fetch0

add0: [RegSel=2, LdB, DrREG] add1
add1: [RegSel=1, LdA, DrREG] add2
add2: [WrREG, DrALU] fetch0

nand0: [RegSel=1, LdA, DrREG] nand1
nand1: [RegSel=2, LdB, DrREG] nand2
nand2: [ALU=2, WrREG, DrALU] fetch0

addi0: [LdB, DrOFF] add1

lw0: [RegSel=1, LdA, DrREG] lw1
lw1: [LdB, DrOFF] lw2
lw2: [LdMAR, DrALU] lw3
lw3: [WrREG, DrMEM] fetch0

sw0: [RegSel=1, LdA, DrREG] sw1
sw1: [LdB, DrOFF] sw2
sw2: [LdMAR, DrALU] sw3
sw3: [WrMEM, DrREG] fetch0

goto0: [LdA, DrPC] goto1
goto1: [LdB, DrOFF] goto2
goto2: [LdPC, DrALU] fetch0

jalr0: [WrREG, DrPC] jalr1
jalr1: [RegSel=1, LdPC, DrREG] fetch0

halt0: [] halt0

skp0: [RegSel=1, LdA, DrREG] skp1
skp1: [RegSel=2, LdB, DrREG] skp2
skp2: [ALU=1, LdCmp, DrALU] nop
skp3: [LdA, DrPC] skp4
skp4: [ALU=3, LdPC, DrALU] fetch0

lea0: [LdA, DrPC] lea1
lea1: [LdB, DrOFF] lea2
lea2: [WrREG, DrALU] fetch0

nop: [ChkCmp]