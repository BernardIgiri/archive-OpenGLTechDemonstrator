#include "StringUtilities.h"
#include "texture manager.h"

/*****************************************************************************

  Creates texture manager system

*****************************************************************************/
CTextureManager::CTextureManager()
{
	m_numberOfTextures = 0;
	m_textureList = NULL;
	m_currentTexture = 0;
}
/*****************************************************************************

  Destroys texture manager system

*****************************************************************************/
CTextureManager::~CTextureManager()
{
	m_numberOfTextures = 0;
	m_textureList = NULL;
	m_currentTexture = 0;
}
/*****************************************************************************

  Initializes the texture manager system with
  #1 The predefined id of the file type
	TEXMAN_BMP for bitmaps
  #2 A list of textures which includes the list count, filenames, and id to be
	assigned to each texture

*****************************************************************************/
bool CTextureManager::InitializeSystem(int fileType, int numberOfTextures, char* textureFiles[], char* textureNames[])
{
	if (m_textureList&&m_numberOfTextures)
	{
		for (int i =0; i<m_numberOfTextures; i++)
		{
			if (m_textureList[i].data)
				free(m_textureList[i].data);
			if (glIsTexture(m_textureList[i].texID))
				glDeleteTextures(1,&m_textureList[i].texID);
			if (m_textureList[i].texName)
				free(m_textureList[i].texName);
		}
		delete[] m_textureList;
		m_textureList = NULL;
	}
	m_numberOfTextures = numberOfTextures;
	m_textureList = new texture_t[m_numberOfTextures];
	if (LoadList(0, fileType,textureFiles,textureNames))
		return true;
	return false;
}

/*****************************************************************************

  Adds a single texture to the current texture system.

*****************************************************************************/
bool CTextureManager::AddTexture(int fileType, char *textureFile, char *textureName)
{
	if (!m_textureList&&m_numberOfTextures)
		return false;
	m_numberOfTextures++;
	texture_t *newList = (texture_t *)realloc(m_textureList,sizeof(texture_t)*m_numberOfTextures);
	if (newList==NULL)
		return false;
	m_textureList = newList;
	char* files[]={textureFile,""};
	char* names[]={textureName,""};
	return LoadList(m_numberOfTextures-1,fileType,files,names);
}

/*****************************************************************************

  Adds a list of textures to the texture with greater efficiency than adding
  one at a time.

*****************************************************************************/
bool CTextureManager::AddTextures(int fileType, int numberOfTextures, char *textureFiles[], char *textureNames[])
{
	int startIndex = m_numberOfTextures;
	if (!m_textureList&&m_numberOfTextures)
		return false;
	if (numberOfTextures<1)
		return false;
	m_numberOfTextures+=numberOfTextures;
	texture_t *newList = (texture_t *)realloc(m_textureList,sizeof(texture_t)*m_numberOfTextures);
	if (newList==NULL)
		return false;
	m_textureList = newList;
	return LoadList(startIndex,fileType,textureFiles,textureNames);/**/
	return true;
}
/*****************************************************************************

  The safer, easier, more reliable texture adding method.  Uses a string
  format with the following syntax:
  (textureFileName, textureName)

  For each texture.
  NOTE: Spaces tabs and newline characters are ingored and skipped during
  processing of the string input.
  See the definition of TEXMAN_RESTRICTEDCHARS in texture manager.h to
  find the other ignored characters.

*****************************************************************************/
bool CTextureManager::AddTextures(char *textureList, int fileType, bool enableChecking)
{
	const char OPENCHAR = '(',
			   CLOSECHAR= ')',
			   COMMA	= ',';
	int length=strlen(textureList);
	int texCount=0;
	char *copy = (char *)malloc(length+1);
	memset(copy,NULL,length+1);
	char ext[4]={0};
	for (register int i=0; i<length; i++)
		if(textureList[i]==OPENCHAR)
			texCount++;
	if (texCount<1)
		return false;
	int pad=((texCount<2) ? 1 : 0);
	char **fileNames = (char **)malloc(sizeof(char *)*(texCount+pad)),
		 **texNames  = (char **)malloc(sizeof(char *)*(texCount+pad));
	enum SeachModes { FindOpen, ReadFileName, ReadTexName } smode = FindOpen;
	int curTex=0;
	int strPos=0;
	for (i=0; i<length; i++)
	{
		char c=textureList[i];
		switch (smode)
		{
		case FindOpen:
			if (c==OPENCHAR)
			{
				smode=ReadFileName;
				fileNames[curTex]=&copy[strPos];
			}
			break;
		case ReadFileName:
			if (c==COMMA)
			{
				copy[strPos]=NULL;
				texNames[curTex]=&copy[strPos+1];
				smode=ReadTexName;
			}
			else
			{
				if (!StrUtil_CompareAgainstString(c,TEXMAN_RESTRICTEDCHARS))
					copy[strPos]=c;
				else
					strPos--;
			}
			strPos++;
			break;
		case ReadTexName:
			if (c==CLOSECHAR)
			{
				copy[strPos]=NULL;
				if ((texNames[curTex]==NULL)||(fileNames[curTex]==NULL))
					return false;
				curTex++;
				smode=FindOpen;
			}
			else
			{
				if (!StrUtil_CompareAgainstString(c,TEXMAN_RESTRICTEDCHARS))
					copy[strPos]=c;
				else
					strPos--;
			}
			strPos++;
			break;
		default:
			return false;
			break;
		}
	}
	if (enableChecking)
		for (i=0; i<texCount; i++)
		{
			if ((texNames[curTex]==NULL)||(fileNames[curTex]==NULL))
				return false;
			if (FindTextureIndex(texNames[i])!=-1)
				return false;
		}
	return AddTextures(fileType,texCount,fileNames,texNames);
}
/*****************************************************************************

  Loads list of files into the texture manager systems list starting from the
  start index.

*****************************************************************************/
bool CTextureManager::LoadList(int startIndex, int fileType, char *textureFiles[], char *textureNames[])
{
	bool isRGB = true;
	bool testFailed=false;
	for(int i=startIndex; i<m_numberOfTextures; i++)
	{
		{// check to see if file exists, if not give warning
			FILE *testOpenFptr=NULL;
			testOpenFptr=fopen(textureFiles[i-startIndex],"rb");//default
			testFailed=(testOpenFptr==NULL);
			if (!testFailed)
				fclose(testOpenFptr);
		}
		switch (fileType)
		{
			case TEXMAN_BMP:
			{
				if (testFailed) // if file does not exist, set load file to default
					textureFiles[i-startIndex]=TEXMAN_DEFAULT_BMP;
				isRGB = true;
				BITMAPINFOHEADER texInfo;
				m_textureList[i].data = LoadBitmapFile(textureFiles[i-startIndex], &texInfo);
				m_textureList[i].width = texInfo.biWidth;
				m_textureList[i].height = texInfo.biHeight;
				m_textureList[i].texSize = texInfo.biSizeImage;
			}
			break;
			case TEXMAN_BMP_RGBA:
			{
				if (testFailed) // if file does not exist, set load file to default
					textureFiles[i-startIndex]=TEXMAN_DEFAULT_BMP;
				isRGB=false;
				BITMAPINFOHEADER texInfo;
				m_textureList[i].data = LoadBitmapFileWithAlpha(textureFiles[i-startIndex], &texInfo);
				m_textureList[i].width = texInfo.biWidth;
				m_textureList[i].height = texInfo.biHeight;
				m_textureList[i].texSize = texInfo.biSizeImage;
			}
			break;
			case TEXMAN_TGA:
			{
				if (testFailed) // if file does not exist, set load file to default
					textureFiles[i-startIndex]=TEXMAN_DEFAULT_TGA;
				TextureImage_t texInfo;
				if (!LoadTGA(&texInfo,textureFiles[i-startIndex]))
					return false;
				m_textureList[i].data		= texInfo.imageData;
				m_textureList[i].width		= texInfo.width;
				m_textureList[i].height		= texInfo.height;
				m_textureList[i].texSize	= texInfo.texSize;
				isRGB = (texInfo.bpp==24);
			}
			break;
			case TEXMAN_JPG:
			{
				if (testFailed) // if file does not exist, set load file to default
					textureFiles[i-startIndex]=TEXMAN_DEFAULT_JPG;
				DWORD width, height, nchannels;
				DecodeJPGFileToGeneralBuffer(textureFiles[i-startIndex],&width,&height,&nchannels,
					&m_textureList[i].data);
				if (m_textureList[i].data==NULL)
					return false;
				m_textureList[i].width	= (int)width;
				m_textureList[i].height	= (int)height;
				m_textureList[i].texSize= (size_t)(width*height*nchannels);
				register unsigned char temp;
				if (nchannels==3)
					for (register unsigned int n = 0; n<m_textureList[i].texSize; n+=3)
					{
						temp = m_textureList[i].data[n];
						m_textureList[i].data[n]=m_textureList[i].data[n+2];
						m_textureList[i].data[n+2]=temp;
					}
				isRGB = true;
			}
			break;
			case TEXMAN_DEVIL:
			{
				if (testFailed) // if file does not exist, set load file to default
					textureFiles[i-startIndex]=TEXMAN_DEFAULT_BMP;
				int bpp=0;
				m_textureList[i].data = loadDevILSupportedImages(textureFiles[i-startIndex],
					&m_textureList[i].width,&m_textureList[i].height,&bpp,&m_textureList[i].texSize);
				isRGB =(bpp<32);
				if (m_textureList[i].data==NULL)
					return false;
				break;
			}
			default:
				return 0;
				break;
		}
		if ((m_textureList[i].width>0)&&(m_textureList[i].height>0))
			m_textureList[i].bpp = 8*(int)((long)m_textureList[i].texSize/(long)((long)m_textureList[i].width*(long)m_textureList[i].height));
		m_textureList[i].texName = (char*)malloc((size_t)(strlen(textureNames[i-startIndex])+1));
		strcpy(m_textureList[i].texName,textureNames[i-startIndex]);
		if (m_textureList[i].data==NULL)
		{
			if (startIndex)
				m_numberOfTextures = i;
			KillSystem();
			m_numberOfTextures = i;
			return false;
		}
		glGenTextures(1,&m_textureList[i].texID);
		glBindTexture(GL_TEXTURE_2D, m_textureList[i].texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		if (isRGB)
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, m_textureList[i].width, m_textureList[i].height, GL_RGB, GL_UNSIGNED_BYTE, m_textureList[i].data);
		else
			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, m_textureList[i].width, m_textureList[i].height, GL_RGBA, GL_UNSIGNED_BYTE, m_textureList[i].data);
		free(m_textureList[i].data);
		m_textureList[i].data=NULL;
	}
	return true;
}

/*****************************************************************************

  Selects a texture by index value making it active for OpenGl rendering.

*****************************************************************************/
void CTextureManager::SelectTexture(int listIndex)
{
	if (m_textureList&&m_numberOfTextures&&(listIndex>=0)&&(listIndex<m_numberOfTextures)&&
		glIsTexture(m_textureList[listIndex].texID))
		glBindTexture(GL_TEXTURE_2D, m_textureList[listIndex].texID);
	else return;
	m_currentTexture = listIndex;
}

/*****************************************************************************

  Selects a texture by assigned id making it active for OpenGl rendering.

*****************************************************************************/
void CTextureManager::SelectTexture(char *texName)
{
	if (texName==NULL)
		return;
	if (m_textureList&&m_numberOfTextures)
		for (int i=0; i<m_numberOfTextures; i++)
			if(strcmp(texName,m_textureList[i].texName)==NULL)
			{
				if (!(glIsTexture(m_textureList[i].texID)))
					return;
				glBindTexture(GL_TEXTURE_2D, m_textureList[i].texID);
				m_currentTexture=i;
				return;
			}
}
/*****************************************************************************

  Returns -1 if not found, else returns index of texture.

*****************************************************************************/
int CTextureManager::FindTextureIndex(char *texName)
{
	if (texName==NULL)
		return -1;
	if (m_textureList&&m_numberOfTextures)
		for (int i=0; i<m_numberOfTextures; i++)
			if(strcmp(texName,m_textureList[i].texName)==NULL)
				return i;
	return -1;
}
/*****************************************************************************

  Returns texture name at specified index.  Returns NULL on failure.

*****************************************************************************/
const char *CTextureManager::FindTextureName(int index)
{
	if ((index<0)||(index>=m_numberOfTextures))
		return NULL;
	return m_textureList[index].texName;
}
/*****************************************************************************

  Returns the current number of textures in the texture manage system.

*****************************************************************************/
int CTextureManager::GetListCount()
{
	return m_numberOfTextures;
}

/*****************************************************************************

  Kills the texture manager releasing the memory it has allocated.

*****************************************************************************/
void CTextureManager::KillSystem()
{
	if (m_textureList)
	{
		for (int i =0; i<m_numberOfTextures; i++)
		{
			if (m_textureList[i].data!=NULL)
				free(m_textureList[i].data);
			if (glIsTexture(m_textureList[i].texID))
				glDeleteTextures(1,&m_textureList[i].texID);
			if (m_textureList[i].texName!=NULL)
				free(m_textureList[i].texName);
		}
		delete[] m_textureList;
		m_textureList = NULL;
	}
	m_numberOfTextures = 0;
}

/*****************************************************************************

  Returns a pointer to the texture data of the currently selected texture
  so that it may be manipulated.  Also passes the size on to size_t pointer.

*****************************************************************************/
unsigned char *CTextureManager::GetCurrentTextureData(size_t *textureSize)
{
	(*textureSize)=m_textureList[m_currentTexture].texSize;
	return m_textureList[m_currentTexture].data;
}
/*****************************************************************************

  Returns the total amount of memory allocated for the texture data loaded in
  the texture system.  This does not calculate total system memory
  requirements.

*****************************************************************************/
size_t CTextureManager::GetTotalAllocatedTextureSize()
{
	size_t totalSize=0;
	for (int i=0; i<m_numberOfTextures; i++)
		totalSize+=m_textureList[i].texSize;
	return totalSize;
}
/*****************************************************************************

  Deletes Texture Data from RAM
  (note, texture description data, remains in list)

*****************************************************************************/
void CTextureManager::DeleteTexture(int listIndex)
{
	if (m_textureList&&m_numberOfTextures&&(listIndex>=0&&listIndex<m_numberOfTextures))
		if (glIsTexture(m_textureList[listIndex].texID))
			glDeleteTextures(1,&m_textureList[listIndex].texID);
}
/*****************************************************************************

  Deletes Texture Data from RAM
  (note, texture description data, remains in list)

*****************************************************************************/
void CTextureManager::DeleteTexture(char *texName)
{
	int listIndex = FindTextureIndex(texName);
	DeleteTexture(listIndex);
}
/*****************************************************************************

  Calculates the amount of Bits Per Pixel for given texure and returns
  result.
  Returns -1 on failure;

*****************************************************************************/
int CTextureManager::GetTexBpp(int listIndex)
{
	if (m_textureList&&m_numberOfTextures&&(listIndex>=0&&listIndex<m_numberOfTextures))
	{
		if (m_textureList[listIndex].texSize==0)
			return -1;
		return m_textureList[listIndex].bpp;//8*(int)((long)m_textureList[listIndex].texSize/(long)((long)m_textureList[listIndex].width*(long)m_textureList[listIndex].height));
	}
	return -1;
}
/*****************************************************************************

  Calculates the amount of Bits Per Pixel for given texure and returns
  result.
  Returns -1 on failure;

*****************************************************************************/
int CTextureManager::GetTexBpp(char *texName)
{
	int listIndex=FindTextureIndex(texName);
	if (m_textureList&&m_numberOfTextures&&(listIndex>0&&listIndex<m_numberOfTextures))
	{
		if ((m_textureList[listIndex].texSize==0)||(m_textureList[listIndex].height==0)||
			(m_textureList[listIndex].width==0))
			return -1;
		return m_textureList[listIndex].bpp;
	}
	return -1;
}
/*****************************************************************************

  Returns the Dimensions of the currently selected Texture.

*****************************************************************************/
void CTextureManager::GetCurTexDimensions(int &width, int &height)
{
	width=m_textureList[m_currentTexture].width;
	height=m_textureList[m_currentTexture].height;
}
/*****************************************************************************

  A Utility function for increasing the ease and reliability of adding new
  Textures to the system.

*****************************************************************************/
bool TexManAddTextures(CTextureManager *cTexMan, char *textureList, int fileType, bool enableChecking)
{
	const char OPENCHAR = '(',
			   CLOSECHAR= ')',
			   COMMA	= ',';
	int length=strlen(textureList);
	int texCount=0;
	char *copy = (char *)malloc(length+1);
	memset(copy,NULL,length+1);
	char ext[4]={0};
	for (register int i=0; i<length; i++)
		if(textureList[i]==OPENCHAR)
			texCount++;
	if (texCount<1)
		return false;
	int pad=((texCount<2) ? 1 : 0);
	char **fileNames = (char **)malloc(sizeof(char *)*(texCount+pad)),
		 **texNames  = (char **)malloc(sizeof(char *)*(texCount+pad));
	enum SeachModes { FindOpen, ReadFileName, ReadTexName } smode = FindOpen;
	int curTex=0;
	int strPos=0;
	for (i=0; i<length; i++)
	{
		char c=textureList[i];
		switch (smode)
		{
		case FindOpen:
			if (c==OPENCHAR)
			{
				smode=ReadFileName;
				fileNames[curTex]=&copy[strPos];
			}
			break;
		case ReadFileName:
			if (c==COMMA)
			{
				copy[strPos]=NULL;
				texNames[curTex]=&copy[strPos+1];
				smode=ReadTexName;
			}
			else
			{
				if (!StrUtil_CompareAgainstString(c,TEXMAN_RESTRICTEDCHARS))
					copy[strPos]=c;
				else
					strPos--;
			}
			strPos++;
			break;
		case ReadTexName:
			if (c==CLOSECHAR)
			{
				copy[strPos]=NULL;
				if ((texNames[curTex]==NULL)||(fileNames[curTex]==NULL))
					return false;
				curTex++;
				smode=FindOpen;
			}
			else
			{
				if (!StrUtil_CompareAgainstString(c,TEXMAN_RESTRICTEDCHARS))
					copy[strPos]=c;
				else
					strPos--;
			}
			strPos++;
			break;
		default:
			return false;
			break;
		}
	}
	if (enableChecking)
		for (i=0; i<texCount; i++)
		{
			if ((texNames[curTex]==NULL)||(fileNames[curTex]==NULL))
				return false;
			if (cTexMan->FindTextureIndex(texNames[i])!=-1)
				return false;
		}
	return cTexMan->AddTextures(fileType,texCount,fileNames,texNames);
}
