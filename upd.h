
#ifndef UPD_EMU_H
#define UPD_EMU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h> // memset

// types

// eh i dont need stdbool
#ifndef bool
typedef uint8_t bool;
enum boolStates {
  false=0,
  true=1
};
#endif

typedef enum _uPD_Variants {
  uPD1771C,
} uPD_Variants;

typedef enum _uPD_Ports {
  UPD_PORT_A=0,
  UPD_PORT_B=1
} uPD_Ports;

typedef enum _uPD_Vectors : uint16_t {
  UPD_VECTOR_RESET =0x00,
  UPD_VECTOR_TONE_4=0x20,
  UPD_VECTOR_TONE_3=0x24,
  UPD_VECTOR_TONE_2=0x28,
  UPD_VECTOR_TONE_1=0x2c,
  UPD_VECTOR_NOISE =0x48,
  UPD_VECTOR_EXT   =0x60,
  UPD_VECTOR_TIME  =0x80
} uPD_Vectors;

// main struct

typedef struct _uPD177x {
  // EMULATOR STATE
  uPD_Variants variant;
  bool inInterrupt;

  // RAM AND ROM
  /* ram
   * union of 2 objects
   * first one being the full 64 bytes
   * the second one split into more arrays
   * the top 32 bytes are registers, then 8 words of stack
   */
  union {
    uint8_t rawMem[64];

    struct {
      uint8_t  Rr[32];
      uint16_t stack[8];
    } memRegs;
  } dataMem;
  // rom
  uint16_t  *prgMem;

  // REGISTERS
  // program counter
  uint16_t PC;
  // accumulator (with "shadow")
  uint8_t  A, A_;
  // X, Y, H
  uint8_t  X:7, Y:5, H:6; // limiting the number of bits doesnt reduce the size of the struct, but its there so overflow occurs
  // ports
  uint8_t  portA, portB;
  // d/a converter
  uint16_t DAC:9;

  // flags
  uint16_t MD:10;
  uint8_t  MD0, MD1;
  //stack pointer
  uint8_t  SP:3;

  // random number generators
  uint8_t RG1:7, RG2:3;
  // "bit" variable for lfsrs
  uint8_t lsfrBit;

  // ??
  uint8_t skip, skip_;

  // ???
  uint8_t N;

} uPD177x;

void triggerInterrupt(uPD177x* chip, uPD_Vectors vector);

// external functions

#define UPD_FUNC(n) uPD177x_##n (uPD177x* chip)
#define UPD_FUNC_A(n, ...) uPD177x_##n (uPD177x* chip, __VA_ARGS__)

void UPD_FUNC_A(Initialize, uPD_Variants variant, uint16_t* program);
void UPD_FUNC  (Reset);

void UPD_FUNC(Tick);

void     UPD_FUNC_A(WritePort, uPD_Ports port, uint8_t value);
uint8_t  UPD_FUNC_A(GetPort  , uPD_Ports port);
uint16_t UPD_FUNC  (GetDAC);

#ifdef __cplusplus
}
#endif

#endif
