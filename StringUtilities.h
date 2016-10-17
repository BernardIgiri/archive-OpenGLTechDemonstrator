#ifndef __INCLUDED_STRING_UTILITIES__
#define __INCLUDED_STRING_UTILITIES__
#include <string.h>
#include <stdlib.h>
#define STRUTIL_DEFBREAKCHARS " ,;'\"-+*/"

class CStringList
{
public:
	CStringList();
	~CStringList();
	bool Initialize();
	bool AddStringToList(const char *string);
	bool ChangeString(int index, const char *string);
	bool CycleUp(const char *newString);
	bool CycleDown(const char *newString);
	int	 GetListCount();
	char **GetList();
private:
	char **m_stringList;
	int	   m_listCount;
	int	   m_initCount;
};

int StrUtil_FindChar(const char c, const char *string);
void StrUtil_Encript(char *string, const char *key);
void StrUtil_Decript(char *string, const char *key);
int StrUtil_GetFileList(char *fileList, const char *filespec, size_t maxLength);
bool StrUtil_ReadNextLine(char *lineString, const char *string, int *index,
								 int maxLength);
int StrUtil_WordWrapS(const char *breakCharacters, const char *string,
							 int width, char **copyOfNewString, int *lineIndexes);
bool StrUtil_IsolateLine(const char *string, char *lineString, int lineIndex,
								int *lineIndexes);
bool StrUtil_IsolateLine(const char *string, char *lineString, int lineIndex,
								int *lineIndexes,int maxLength);
bool StrUtil_WordWrap(const char *breakCharacters, const char *string,
							 int width, CStringList *pStringList, int listStartIndex,
							 int *lines);
void StrUtil_DeleteCharacter(char *string, const int index);
void StrUtil_InsertCharacter(char *string, const char c, const int index);
bool StrUtil_CompareAgainstString(const char c, const char *string);
void StrUtil_ReadCommandString(const char *string,void *data,
								void (*insertdata)(void *data, const char *command,const char *dataString),
								const char openChar='\'', const char closeChar='\'');

#endif