#include "upd.h"
#include <stdio.h>

void UPD_FUNC_A(Initialize, uPD_Variants variant, uint16_t* program) {
  chip->variant=variant;
  chip->prgMem=program;

  uPD177x_Reset(chip);
}

void UPD_FUNC(Reset) {
  chip->programCounter=chip->prgMem[UPD_VECTOR_RESET];
  chip->portA=chip->portB=0;
  chip->DAC=0;

  memset(chip->dataMem.rawMem, 0, sizeof(chip->dataMem));

  chip->accumulator=0;
  chip->MD=0;
  chip->MD0=chip->MD1=0;

  chip->X=chip->Y=chip->H=0;
  chip->RG1=chip->RG2=0;
}

void UPD_FUNC(Tick) {
  uint16_t inst=chip->prgMem[chip->programCounter++];
  switch (inst) {
    case 0: // NOP
      break;
    case 0x0002: // OUT PA
      chip->portA=chip->accumulator;
      break;
    case 0x0004: // OUT PB
      chip->portB=chip->accumulator;
    default: break;
  }
  printf("PC: %.4x instr: %.4x\n", chip->programCounter, inst);
}

void UPD_FUNC_A(WritePort, uPD_Ports port, uint8_t value) {
  switch (port) {
    case UPD_PORT_A:
      chip->portA=value;
      break;
    case UPD_PORT_B:
      chip->portB=value;
      break;
    default: break;
  }
}

uint8_t UPD_FUNC_A(GetPort, uPD_Ports port) {
  switch (port) {
    case UPD_PORT_A:
      return chip->portA;
    case UPD_PORT_B:
      return chip->portB;
      break;
    default: return 0;
  }
}

uint16_t UPD_FUNC(GetDAC) {
  return chip->DAC;
}

