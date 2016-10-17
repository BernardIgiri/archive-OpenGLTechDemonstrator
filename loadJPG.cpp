#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <ijl.h>

BOOL DecodeJPGFileToGeneralBuffer(
	LPCSTR	lpszPathName,
	DWORD*	width,
	DWORD*	height,
	DWORD*	nchannels,
	BYTE**	buffer)
{
	BOOL	bres;
	IJLERR	jerr;
	DWORD	x = 0;
	DWORD	y = 0;
	DWORD	c = 0;
	DWORD	wholeimagesize;
	BYTE*	pixel_buf = NULL;

	JPEG_CORE_PROPERTIES jcprops;
	
	bres = TRUE;

	__try
	{
		jerr = ijlInit(&jcprops);
		if(IJL_OK != jerr)
		{
			bres = FALSE;
			__leave;
		}

		jcprops.JPGFile = const_cast<LPSTR>(lpszPathName);

		jerr = ijlRead(&jcprops, IJL_JFILE_READPARAMS);
		if(IJL_OK != jerr)
		{
			bres = FALSE;
			__leave;
		}

		x = jcprops.JPGWidth;
		y = jcprops.JPGHeight;
		c = 3;
		
		wholeimagesize = (x * y * c);

		pixel_buf = new BYTE [wholeimagesize];
		if(NULL == pixel_buf)
		{
			bres = FALSE;
			__leave;
		}

		jcprops.DIBWidth	= x;
		jcprops.DIBHeight	= y;
		jcprops.DIBChannels	= c;
		jcprops.DIBColor	= IJL_BGR;
		jcprops.DIBPadBytes	= 0;
		jcprops.DIBBytes	= pixel_buf;

		switch(jcprops.JPGChannels)
		{
		case 1:
			{
				jcprops.JPGColor = IJL_G;
				break;
			}
			
		case 3:
			{
				jcprops.JPGColor = IJL_YCBCR;
				break;
			}

		default:
			{
				jcprops.DIBColor = (IJL_COLOR)IJL_OTHER;
				jcprops.JPGColor = (IJL_COLOR)IJL_OTHER;
				break;
			}
		}

		jerr = ijlRead(&jcprops, IJL_JFILE_READWHOLEIMAGE);
		if (IJL_OK != jerr)
		{
			bres = FALSE;
			__leave;
		}
	} // __try
	__finally
	{
		if(FALSE == bres)
		{
			if(NULL != pixel_buf)
			{
				delete [] pixel_buf;
				pixel_buf = NULL;
			}
		}
		
		ijlFree(&jcprops);

		*width		= x;
		*height		= y;
		*nchannels	= c;
		*buffer		= pixel_buf;
	} // __finally

	return bres;
} // DecodeJPGFileToGeneralBuffer()