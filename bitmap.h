#ifndef __INCLUDED_BITMAP_LOADER__
#define __INCLUDED_BITMAP_LOADER__

extern unsigned char *LoadBitmapFileWithAlpha(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
extern unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);

#endif