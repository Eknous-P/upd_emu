#include "upd.h"
#include <stdio.h>

void triggerInterrupt(uPD177x* chip, uPD_Vectors vector) {
  if (!chip->inInterrupt) {
    chip->dataMem.memRegs.stack[chip->SP++]=chip->PC;
    chip->PC=chip->prgMem[vector];
    chip->inInterrupt=true;
  }
}

void UPD_FUNC_A(Initialize, uPD_Variants variant, uint16_t* program) {
  chip->variant=variant;
  chip->prgMem=program;

  uPD177x_Reset(chip);
}

void UPD_FUNC(Reset) {
  chip->PC=chip->prgMem[UPD_VECTOR_RESET];
  chip->portA=chip->portB=0;
  chip->DAC=0;

  memset(chip->dataMem.rawMem, 0, sizeof(chip->dataMem));

  chip->A_=chip->A=0;
  chip->MD=0;
  chip->MD0=chip->MD1=0;

  chip->SP=0;

  chip->X=chip->Y=chip->H=0;
  chip->RG2=chip->RG1=0;

  chip->skip_=chip->skip=0;
}

void UPD_FUNC(Tick) {
  // check for interrupts

  // advance PC
  uint16_t inst=chip->prgMem[chip->PC++];
  // MSB 0
  switch (inst) {
    case 0x0000: // NOP
      break;
    case 0x0002: // OUT PA
      chip->portA=chip->A;
      break;
    case 0x0004: // OUT PB
      chip->portB=chip->A;
      break;
    case 0x0005: // STF
      // TODO: wheres carry?
      break;
    case 0x0008: // MOV X, RG
      chip->X=chip->RG1;
      break;
    case 0x0101: // MON
      // ???
      break;
    case 0x0201: // MOV N, A
      chip->N=chip->A;
      break;
    case 0x0208: // MOV X, A
      chip->X=chip->A;
      break;
    case 0x0401: // IN PA
      chip->A=chip->portA;
      break;
    case 0x0402: // IN PB
      chip->A=chip->portB;
      break;
    case 0x0404: // RAR
      chip->A>>=1;
      break;
    case 0x0408: // RAL
      chip->A<<=1;
      break;
    case 0x0501: // JMPA
      chip->PC+=chip->A;
      break;
    case 0x0502: // OUT DA
     chip->DAC=chip->A;
     break;
    case 0x0504: // MUL1
      chip->A = (chip->A + chip->Y >> 1); // TODO: confirm operator order!
      break;
    case 0x050A: // MUL2
      if (chip->Y&1) {
        chip->A = (chip->X + chip->Y >> 1); // DITTO
      } else {
        chip->A = (chip->A + chip->Y >> 1); // DITTO
      }
      break;
    case 0x0602: // OFF
      // ???
      break;
    case 0x0800: // RET
      chip->PC=chip->dataMem.memRegs.stack[chip->SP--];
      break;
    case 0x0801: // RETS
      chip->PC=chip->dataMem.memRegs.stack[chip->SP--]+1; // really?
      break;
    case 0x090f: // RETI
      chip->PC=chip->dataMem.memRegs.stack[chip->SP--]; // to confirm
      chip->inInterrupt=false;
      break;
    default: {
      // MSB 1

      break;
    }
  }
  printf("PC: %.4x instr: %.4x\n", chip->PC, inst);

  // advance RG1
  chip->lsfrBit= ((chip->RG1>>6) ^ (chip->RG1>>5))&1;
  chip->RG1    =  (chip->RG1<<1) | ~chip->lsfrBit;
  // advance RG2
  if (1) { // TODO: this condition
    chip->lsfrBit=((chip->RG2>>2) ^ (chip->RG2>>1))&1;
    chip->RG2    = (chip->RG2<<1) | ~chip->lsfrBit;
  } else {
    chip->lsfrBit=(chip->RG2>>0)&1;
    chip->RG2    =(chip->RG2<<1) | ~chip->lsfrBit;
  }
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

