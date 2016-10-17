#ifndef __FONTSHEADER_INCLUDED__
#define __FONTSHEADER_INCLUDED__
extern void ClearFont(unsigned int base);
extern unsigned int CreateBitmapFont(char *fontName, int fontSize);
extern void PrintString(unsigned int base, char *str);
#endif