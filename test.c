#include "upd.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* print format
PC: 0000 | A: 00[00] | X: 00 Y: 00 H: 00 |
MD:  000 | MD0: 00 | MD1: 00 | SP: 0     |
RG1:  00 | RG2: 0  | SKIP: 00[00]| N: 00 |
PORT         A: 00 | B:   00 | DAC: -00  |
RAM:
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/


// poor man's hexdump
void dumpData(const void* data, size_t len) {
  uint8_t b;
  for (size_t i=0, b=1; i<len; i++,b++, b&=0xf) {
    printf("%.2x ", ((uint8_t*)data)[i]);
    if (b==0) printf("\n");
  }
}

void printuPD(uPD177x* chip) {
  uint16_t dac16=chip->DAC<<7;
  signed short dac=*(signed short*)&dac16;
  printf(
"PC: %.4x | A: %.2x[%2.x] | X: %.2x Y: %.2x H: %.2x |\n"
"MD:  %.3x | MD0: %.2x | MD1: %.2x | SP: %.1x     |\n"
"RG1:  %.2x | RG2: %.1x  | SKIP: %.2x[%.2x]| N: %.2x |\n"
"PORT         A: %.2x | B:   %.2x | DAC:  %c%.2d |\n"
"RAM:\n",
chip->PC, chip->A, chip->A_, chip->X, chip->Y, chip->H,
chip->MD, chip->MD0, chip->MD1, chip->SP,
chip->RG1, chip->RG2, chip->skip, chip->skip_, chip->N,
chip->portA, chip->portB, ((dac<0)?'-':' '),abs(dac)>>7
  );
  printf("\x1b[34m");
  dumpData(chip->dataMem.memRegs.Rr, 32);
  printf("\x1b[32m");
  dumpData(chip->dataMem.memRegs.stack, 16);
  printf("\x1b[0m");
  dumpData(chip->dataMem.rawMem+48, 16);
}

void writeU32(FILE* f, unsigned int n) {
  fwrite(&n, sizeof(unsigned int), 1, f);
}

void writeU16(FILE* f, unsigned short n) {
  fwrite(&n, sizeof(unsigned short), 1, f);
}

void initWav(FILE* f, unsigned int sampleRate, unsigned short channels, unsigned int samples) {
  rewind(f);
  fprintf(f, "RIFF");
  writeU32(f, 36);
  fprintf(f, "WAVE");
  fprintf(f, "fmt ");
  writeU32(f, 16);
  writeU16(f, 1);
  writeU16(f, channels);
  writeU32(f, sampleRate);
  writeU32(f, sampleRate*channels*2);
  writeU16(f, channels*2);
  writeU16(f, 16);
  fprintf(f, "data");
  writeU32(f, samples*2);
}

// loads a ROM file, puts into the emulator and runs
int main(int argc, char** argv) {
  if (argc<2) {
    printf("no file given!\n");
    printf(
"usage:\n"
"%s [romfile]\n"
"%s [romfile] [outfile] [samples]\n", argv[0], argv[0]
);
printf("romfile is a µPD ROM\n\
outfile is a WAV audio file, will output [samples] samples of the DAC\n");
    return 0;
  }
  // open ROM file
  FILE* rom=fopen(argv[1], "rb");
  if (rom==NULL) {
    fprintf(stderr, "could not open %s!\n", argv[1]);
    return 1;
  }
  FILE* out=NULL;
  unsigned int ticks=256;
  if (argc>=3) {
    if (argc==3) {
      printf("no out samples specified! will output 256 samples...\n");
    } else {
      ticks=atoi(argv[3]);
    }
    out=fopen(argv[2], "wb");
    if (out==NULL) {
      fprintf(stderr, "could not open %s!\n", argv[2]);
    }
  }

  // get file size
  fseek(rom, 0, SEEK_END);
  size_t flen=ftell(rom);
  rewind(rom);

  // ideally i should use a pointer and malloc using the size
  // but this works
  uint16_t romData[32768];
  printf("read file %s (%lu bytes)\n", argv[1], flen);
  if (flen>65536) flen=65536;
  // read the file into the array
  fread(romData, 1, flen, rom);
  // and close
  fclose(rom);

  // initialise the emulator
  uPD177x upd;
  uPD177x_Initialize(&upd, uPD1771C, romData);
  // run for 32 ticks
  printf("\x1b[1J\x1b[1;1H"); // erase terminal
  printf("%s, variant %d\n", argv[1], upd.variant);
  if (out) {
    initWav(out, 192000, 1, ticks);
  }
  for (int i=0; i<ticks; i++) {
    uPD177x_Tick(&upd);
    printf("\x1b[2;1H");
    printuPD(&upd);
    if (out) {
      uint16_t dac16=upd.DAC<<7;
      signed short dac=*(signed short*)&dac16;
      fwrite(&dac, 2, 1, out);
    }
  }

  if (out) fclose(out);

  return 0;
}
