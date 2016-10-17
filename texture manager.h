#ifndef __INCLUDED_TEXMAN__
#define __INCLUDED_TEXMAN__

// system headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <string.h>
// file format headers
#include "bitmap.h"
#include "LoadTGA.h"
#include "LoadJPG.h"
#include "DevILFileFormats.h"
/// File Type Defines
#define TEXMAN_BMP		0
#define TEXMAN_BMP_RGBA	1
#define TEXMAN_TGA		2
#define TEXMAN_JPG		3
#define TEXMAN_DEVIL	4
/// Default Image File Defines
#define TEXMAN_DEFAULT_BMP "defaultImages\\default.bmp"
#define TEXMAN_DEFAULT_JPG "defaultImages\\default.jpg"
#define TEXMAN_DEFAULT_TGA "defaultImages\\default.tga"
/// Utility Function Defines
#define TEXMAN_RESTRICTEDCHARS ",!@#$%^&*` \t\n-+/\"'"
typedef struct
{
	int bpp;
	int width;
	int height;
	unsigned int texID;
	unsigned char *data;
	char *texName;
	size_t texSize;
} texture_t;

/*************************************************************************

  Needs to be able to:
  ----------------------------
  Load textures
  Make Texure Current Texture
  Free Textures

  Receives On Initialization:
  ----------------------------
  FileType
  BMP or TGA filenames
  Number of Textures
  Texture Name Tag

  Receives On Call:
  ----------------------------
  Index of texture
  or Texture Name Tag

**************************************************************************/

class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();
	bool InitializeSystem(int fileType, int numberOfTextures, char* textureFiles[], char* textureNames[]);
	void KillSystem();
	void SelectTexture(int listIndex);
	void SelectTexture(char *texName);
	int	 FindTextureIndex(char *texName);
	const char *FindTextureName(int index);
	bool AddTexture(int fileType, char* textureFile, char* textureName);
	bool AddTextures(int fileType, int numberOfTextures, char* textureFiles[], char* textureNames[]);
	bool AddTextures(char *textureList, int fileType=TEXMAN_DEVIL, bool enableChecking=true);
	void DeleteTexture(int listIndex);
	void DeleteTexture(char *texName);
	int	 GetTexBpp(int listIndex);
	int	 GetTexBpp(char *texName);
	void GetCurTexDimensions(int &width, int &height);
	int	 GetListCount();
	size_t GetTotalAllocatedTextureSize();
	unsigned char *GetCurrentTextureData(size_t *textureSize);
protected:
	bool LoadList(int startIndex, int fileType, char* textureFiles[], char* textureNames[]);
	texture_t*	m_textureList;
	int			m_numberOfTextures;
	int			m_currentTexture;
};

bool TexManAddTextures(CTextureManager *cTexMan, char *textureList, int fileType=TEXMAN_DEVIL, bool enableChecking=true);

#endif