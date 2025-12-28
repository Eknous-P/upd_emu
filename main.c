#include "upd.h"
#include <stdio.h>

void dumpData(const void* data, size_t len) {
  uint8_t b;
  for (size_t i=0, b=1; i<len; i++,b++, b&=0xf) {
    printf("%.2x ", ((uint8_t*)data)[i]);
    if (b==0) printf("\n");
  }
}

int main(int argc, char** argv) {
  if (argc<2) {
    printf("no file given! exiting...\n");
    return 0;
  }
  FILE* rom=fopen(argv[1], "rb");
  if (rom==NULL) {
    fprintf(stderr, "could not open %s!\n", argv[1]);
    return 1;
  }

  fseek(rom, 0, SEEK_END);
  size_t flen=ftell(rom);
  rewind(rom);
  uint16_t romData[32768];
  printf("read file %s (%lu bytes)\n", argv[1], flen);
  if (flen>65536) flen=65536;
  fread(romData, 1, flen, rom);
  fclose(rom);
  // dumpData(romData, 1024);

  uPD177x upd;
  uPD177x_Initialize(&upd, uPD1771C, romData);
  for (int i=0; i<32; i++) {
    uPD177x_Tick(&upd);
  }

  return 0;
}
