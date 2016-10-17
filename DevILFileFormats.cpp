#include "DevILFileFormats.h"
unsigned char *loadDevILSupportedImages(char *filename, int *width, int *height, int *bpp, size_t *size)
{
	static bool firstTime=true;
	if (firstTime)
	{
		ilInit();
		iluInit();
		firstTime=false;
	}
	if (ilLoadImage(filename)==IL_FALSE)
		return NULL;
	ILubyte *data = ilGetData();
	ILinfo imageInfo;
	iluGetImageInfo(&imageInfo);
	(*width)=(int)imageInfo.Width;
	(*height)=(int)imageInfo.Height;
	(*bpp)=(int)imageInfo.Bpp*8;
	(*size)=(size_t)imageInfo.SizeOfData;
	unsigned char *dataCopy = (unsigned char *)malloc(imageInfo.SizeOfData);
	if (dataCopy!=NULL)
		memcpy(dataCopy,data,imageInfo.SizeOfData);
	return dataCopy;
}