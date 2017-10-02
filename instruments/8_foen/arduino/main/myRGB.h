typedef uint8_t color;

typedef struct myRGB {
  color r;
  color g;
  color b;
} myRGB;

myRGB pallette[] = {{0,0,255},{255,100,0},{255,0,0}};

#define palletteSize sizeof(pallette) / sizeof(pallette[0])
