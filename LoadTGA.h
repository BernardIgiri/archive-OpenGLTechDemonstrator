#ifndef __INCLUDED_TGA_LOADER__
#define __INCLUDED_TGA_LOADER__

#include <stdlib.h>
#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>

typedef struct												// Create A Structure
{
	GLubyte	*imageData;										// Image Data (Up To 32 Bits)
	GLuint	bpp;											// Image Color Depth In Bits Per Pixel.
	GLuint	width;											// Image Width
	GLuint	height;											// Image Height
	GLuint	texID;											// Texture ID Used To Select A Texture
	GLuint  texSize;
} TextureImage_t;

#ifndef TGA_LOADER_NO_FUNCTIONS
extern bool LoadTGA(TextureImage_t *texture, char *filename);
#endif

#endif
