#ifndef __JPGLOADER_INCLUDED__
#define __JPGLOADER_INCLUDED__
extern	BOOL DecodeJPGFileToGeneralBuffer(
		LPCSTR	lpszPathName,
		DWORD*	width,
		DWORD*	height,
		DWORD*	nchannels,
		BYTE**	buffer);
#endif