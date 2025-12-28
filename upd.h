
#ifndef UPD_EMU_H
#define UPD_EMU_H

#include <stdint.h>
#include <stdlib.h> // malloc, free
#include <string.h> // memset

// types

typedef int unknown_size;

typedef uint8_t data_word;
typedef uint16_t prg_word;

typedef enum _uPD_Variants {
  uPD1771C,
} uPD_Variants;

typedef enum _uPD_Ports {
  UPD_PORT_A=0,
  UPD_PORT_B=1
} uPD_Ports;

typedef enum _uPD_Vectors {
  UPD_VECTOR_RESET=0,
  UPD_VECTOR_TONE_4=0x20,
} uPD_Vectors;

// main struct

typedef struct _uPD177x {
  // emulator state
  uPD_Variants variant;

  // ram and rom
  union {
    data_word rawMem[64];

    data_word Rr[32];
    data_word therest[32];
  } dataMem;
  prg_word  *prgMem;

  // registers
  uint16_t programCounter;
  uint8_t accumulator;
  uint8_t X:7,Y:5,H:6; // limiting the number of bits doesnt reduce the size of the struct, but its there so overflow occurs
  uint8_t portA, portB;
  uint16_t DAC:9;

  uint16_t MD:10;
  uint8_t MD0, MD1;
  uint8_t SP:3;

  uint8_t RG1:7, RG2:3;

} uPD177x;

// functions

#define UPD_FUNC(n) uPD177x_##n (uPD177x* chip)
#define UPD_FUNC_A(n, ...) uPD177x_##n (uPD177x* chip, __VA_ARGS__)

void UPD_FUNC_A(Initialize, uPD_Variants variant, uint16_t* program);
void UPD_FUNC  (Reset);

void UPD_FUNC(Tick);

void     UPD_FUNC_A(WritePort, uPD_Ports port, uint8_t value);
uint8_t  UPD_FUNC_A(GetPort  , uPD_Ports port);
uint16_t UPD_FUNC  (GetDAC);

#endif
