#include "StringUtilities.h"
#include <io.h>
/************************************************************************

  Reads a command string and returns the data in desired format.

************************************************************************/
void StrUtil_ReadCommandString(const char *string,void *data,
								void (*insertdata)(void *data, const char *command,const char *dataString),
								const char openChar, const char closeChar)
{
	if (string==NULL||data==NULL||insertdata==NULL)
		return;
	int length=strlen(string);
	int strPos=0,cmdPos=0;
	char strBuf[1024]={0};
	char cmdBuf[1024]={0};
	enum SearchModes { FindCommandStart, ReadCommand, FindOpeningChar,
		ReadString } sMode=FindCommandStart;
	for (int i=0; i<length; i++)
	{
		char c=string[i];
		switch (sMode)
		{
		case FindCommandStart:
			if ((c>=65&&c<=90)||(c>=97&&c<=122))
			{
				if (c>=97)
					c-=32;
				cmdPos=0;
				cmdBuf[cmdPos]=c;
				cmdPos++;
				sMode=ReadCommand;
			}
			break;
		case ReadCommand:
			if ((c>=65&&c<=90)||(c>=97&&c<=122))
			{
				if (c>=97)
					c-=32;
				cmdBuf[cmdPos]=c;
				cmdPos++;
			}
			else
			{
				i--;
				sMode=FindOpeningChar;
			}
			break;
		case FindOpeningChar:
			if (c==openChar)
			{
				strPos=0;
				sMode=ReadString;
			}
			break;
		case ReadString:
			if (c!=closeChar)
			{
				strBuf[strPos]=c;
				strPos++;
			}
			else
			{
				insertdata(data,cmdBuf,strBuf);
				memset(cmdBuf,NULL,sizeof(cmdBuf));
				memset(strBuf,NULL,sizeof(strBuf));
				cmdPos=0;
				strPos=0;
				sMode=FindCommandStart;
			}
			break;
		default:
			return;
			break;
		}
	}
}
/************************************************************************
************************************************************************/
void StrUtil_DeleteCharacter(char *string, const int index)
{
	int length = strlen(string);
	if (index<0 || index>=length)
		return;
	if (string[index]==NULL)
		return;
	memcpy(string+index,string+index+1,length-index+1);
}
void StrUtil_InsertCharacter(char *string, const char c, const int index)
{
	int length = strlen(string);
	if (index<0 || index>length)
		return;
	if (string[index]==NULL)
	{
		string[index]=c;
		string[index+1]=NULL;
		return;
	}
	memcpy(string+index+1,string+index,length-index);
	string[index]=c;
}
/*************************************************************************************

  This Version of Word Wrap is still imperfect, use StrUtil_WordWrapS instead.

*************************************************************************************/
bool StrUtil_WordWrap(const char *breakCharacters, const char *string,
					  int width, CStringList *pStringList, int listStartIndex, int *lines)
{ // flawed
	if (width<1 || string==NULL || pStringList==NULL || breakCharacters==NULL
		|| listStartIndex<0) // data validity check
		return false;
	int length = strlen(string),
		bLength= strlen(breakCharacters),
		liCount= pStringList->GetListCount();
	char *buffer=NULL;
	buffer = (char *)malloc(width+1);
	if (buffer==NULL)
		return false;
	memset(buffer,NULL,width+1);
	for (int i=0, line=listStartIndex; i<length; i+=width, line++)
	{
		strncpy(buffer,string+i,width);
		bool finished = false;
		for (int si=width; si>=0; si--) // searches buffer backwards for last breaking character
		{
			for (int bi=0; bi<bLength; bi++)
				if (buffer[si]==breakCharacters[bi])
				{
					finished=true;
					break;
				}
			if (finished)
				break;
		}
		int next = si;
		bool isBreak = false;
		if ((i+width)<length)
			for (int bi=0; bi<bLength; bi++) // checks next character for break character
				if (string[i+width]==breakCharacters[bi])
				{
					isBreak=true;
					break;
				}
		if (!isBreak // rewinds to 1 after the last break char if next char is not a break char
			&& si>0 // but not if that is too far back
			&& (length-i)>width) // and only if it still needs to be wrapped
		{
			buffer[si+1]=NULL; // sets char after  last break char to NULL
			i=si+1+i-width;
		}
		// add string to list
		if (line>=liCount) // if exceeded list length
			pStringList->AddStringToList(buffer);
		else
			pStringList->ChangeString(line,buffer);
	}
	free(buffer);
	*lines=line;
	return true;
}
/**************************************************

  Encodes string by the data in the key.

**************************************************/
void StrUtil_Encript(char *string, const char *key)
{
	int sLength=strlen(string),
		kLength=strlen(key);
	for (int i=0; i<sLength; i++)
		string[i]=(char)( ((int)string[i]+(int)key[i%kLength])%128 );
	// scramble
	char temp;
	for (i=0; i<sLength; i+=4)
	{
		if ((i+4)<sLength)
		{
			temp = string[i];
			string[i]=string[i+2];
			string[i+2]=temp;

			temp=string[i+1];
			string[i+1]=string[i+3];
			string[i+3]=temp;
		}
	}
}
/**************************************************

  Decodes string by the data in the key.

**************************************************/
void StrUtil_Decript(char *string, const char *key)
{
	int sLength=strlen(string),
		kLength=strlen(key);
	// descramble
	char temp;
	for (int i=0; i<sLength; i+=4)
	{
		if ((i+4)<sLength)
		{
			temp = string[i+2];
			string[i+2]=string[i];
			string[i]=temp;

			temp=string[i+3];
			string[i+3]=string[i+1];
			string[i+1]=temp;
		}
	}
	for ( i=0; i<sLength; i++)
	{
		int temp = (char)( ((int)string[i]-(int)key[i%kLength]) );
		if (temp<0)
			temp+=128;
		string[i]=temp;
	}
}
/**************************************************

  Be sure to allocate fileList to a large enough
  size to hold all of the file names or you
  will only get a partial list. Returns number
  of files found. Allocating it to less than 260
  would be a risk.

**************************************************/
int StrUtil_GetFileList(char *fileList, const char *filespec, size_t maxLength)
{
	int count=0;
	_finddatai64_t fileData;
	long handle = _findfirsti64(filespec,&fileData);
	strcpy(fileList,fileData.name);
	//strcat(fileList,"\n");
	if (handle!=-1)
	{
		count=1;
		while (_findnexti64(handle,&fileData)!=-1)
		{
			if ((strlen(fileList)+strlen(fileData.name)+2)>maxLength)
				break; // if not enough space left leave
			if (fileData.name!=NULL && fileData.name[0]!=NULL)
			{
				strcat(fileList,"\n");
				strcat(fileList,fileData.name);
				count++;
			}
		}
	}
	return count;
}
/**************************************************

  Returns index in which the char occurs in the
  string, or returns -1 if not found.

**************************************************/
int StrUtil_FindChar(const char c, const char *string)
{
	register int length = strlen(string);
	for (register int i=0; i<length; i++)
		if (string[i]==c)
			return i;
	return -1;
}
/**************************************************

  Finds if single char exists in string

**************************************************/
bool StrUtil_CompareAgainstString(const char c, const char *string)
{
	register int length = strlen(string);
	for (register int i=0; i<length; i++)
		if (string[i]==c)
			return true;
	return false;
}
/**************************************************

  Remember to free the newString and lineIndexes
  arrays after use.

**************************************************/
int StrUtil_WordWrapS(const char *breakCharacters, const char *string,
					  int width, char **copyOfNewString, int *lineIndexes)
{
	char *newString=NULL;
	if (width<1 || string==NULL || newString!=NULL
		|| breakCharacters==NULL || lineIndexes!=NULL) // data validity check
		return -1;
	int length = strlen(string),
		bLength= strlen(breakCharacters),
		lines = 0;
	newString=(char *)malloc(length+2*(length/width)+2); // allocate to max possible chars
	if (newString==NULL)
		return -1;
	memset(newString,NULL,length+2*(length/width)+2); // clear it
	strcpy(newString,string); // copy to it
	lineIndexes=(int *)malloc(sizeof(int)*2*(length/width)+2); // allocate to max possible lines
	int travelCounter=0;
	for (int i=(width); i<(length+lines); i+=width)
	{
		for (int si=i+1; si>=(i-width); si--) // searches backward for last breaking char
		{
			travelCounter++;
			if ('\n'==newString[si]) // if newline character found count as newline
			{
				lineIndexes[lines]=si+1;
				lines++;
				break;
			}
			else if(StrUtil_CompareAgainstString(newString[si],breakCharacters))
			{
				StrUtil_InsertCharacter(newString,'\n',si+1);
				lineIndexes[lines]=si+1;
				lines++;
				break;
			}
			else if (travelCounter>=width)
			{
				StrUtil_InsertCharacter(newString,'\n',i+1);
				lineIndexes[lines]=i+1;
				lines++;
				break;
			}
		}
		travelCounter=0;
	}
	if (newString)
		(*copyOfNewString)=newString;
	return lines;
}
/**************************************************

  lineString must be preallocated

**************************************************/
bool StrUtil_ReadNextLine(char *lineString, const char *string, int *index, int maxLength)
{
	if (string==NULL || lineString==NULL)
		return false;
	memset(lineString,NULL,maxLength+1);
	register int length = strlen(string);
	for (register int i=0; i<length; i++)
		if (string[i]=='\n')
			break;
	if (i==0)
	{
		*index+=1;
		return false;
	}
	if ((i+1)>=maxLength)
		i=maxLength-1;
	strncpy(lineString,string,i);
	*index+=i;
	return true;
}
/**************************************************

  If lineString is already defined no memory is
  allocated for it.  Otherwise is free(void*)
  to free the string after use.

**************************************************/
bool StrUtil_IsolateLine(const char *string, char *lineString, int lineIndex, int *lineIndexes)
{ // not working yet
	if (lineIndexes==NULL || string==NULL || lineIndex<0)
		return false;
	int lineLength = lineIndexes[lineIndex+1]-lineIndexes[lineIndex];
	if (lineLength<1)
		return false;
	if (lineString==NULL) // null string than allocate memory for it
		lineString=(char *)malloc(lineLength);
	strncpy(lineString,string+lineIndexes[lineIndex],lineLength-1);
	return true;
}
/**************************************************

  If lineString is already defined no memory is
  allocated for it.  Otherwise is free(void*)
  to free the string after use.

**************************************************/
bool StrUtil_IsolateLine(const char *string, char *lineString, int lineIndex, int *lineIndexes,
						 int maxLength)
{ // not working yet
	if (lineIndexes==NULL || string==NULL || lineIndex<0)
		return false;
	int lineLength = lineIndexes[lineIndex+1]-lineIndexes[lineIndex];
	if (lineLength<1)
		return false;
	if (lineLength>maxLength)
		lineLength=maxLength;
	if (lineString==NULL) // null string than allocate memory for it
		lineString=(char *)malloc(lineLength);
	strncpy(lineString,string+lineIndexes[lineIndex],lineLength-1);
	return true;
}


/***************/

CStringList::CStringList()
{
	m_listCount=m_initCount=0;
	m_stringList=NULL;
}

CStringList::~CStringList()
{
	if (m_initCount>0)
	{
		if (m_initCount==1)
		{
			free(*m_stringList);
			free(m_stringList);
		}
		else
		{
			for (int i=0; i<m_initCount; i++)
				free(m_stringList[i]);
			free(m_stringList);
		}
	}
}

bool CStringList::Initialize()
{
	m_stringList=new char *[2];
	if (m_stringList==NULL)
		return false;
	m_stringList[0]=new char[2];
	if (m_stringList[0]==NULL)
		return false;
	m_stringList[1]=new char[2];
	if (m_stringList[1]==NULL)
		return false;
	strcpy(m_stringList[0]," ");
	strcpy(m_stringList[1]," ");
	m_listCount=2;
	return true;
}
bool CStringList::ChangeString(int index, const char *string)
{
	if (index<0 || index>=m_listCount || string==NULL)
		return false;
	memset(m_stringList[index],NULL,strlen(m_stringList[index])+1);
	m_stringList[index]=(char *)realloc(m_stringList[index],strlen(string)+1);
	if (m_stringList[index]==NULL)
		return false;
	strcpy(m_stringList[index],string);
	return true;
}
bool CStringList::CycleUp(const char *newString)
{
	if (m_listCount<2)
		return false;
	for (int i=1; i<m_listCount; i++)
		ChangeString(i-1,m_stringList[i]);
		//m_stringList[i-1]=m_stringList[i];
	if (!ChangeString(m_listCount-1,newString))
		return false;
	return true;
}
bool CStringList::CycleDown(const char *newString)
{
	if (m_listCount<2)
		return false;
	for (int i=1; i<m_listCount; i++)
		ChangeString(i,m_stringList[i-1]);
		//m_stringList[i]=m_stringList[i-1];
	if (!ChangeString(0,newString))
		return false;
	return true;
}
bool CStringList::AddStringToList(const char *string)
{
	m_initCount++;
	m_listCount++;
	if (m_initCount<=2)
	{
		m_listCount=m_initCount;
		m_stringList[m_initCount-1]=(char *)realloc(m_stringList[m_initCount-1],strlen(string)+1);
		if (m_stringList[m_initCount-1]==NULL)
			return false;
		strcpy(m_stringList[m_initCount-1],string);
		return true;
	}
	else if (m_listCount==1)
	{
		m_stringList=(char **)malloc(sizeof(char *));
		if (m_stringList==NULL)
			return false;
		*m_stringList = (char *)malloc(strlen(string)+1);
		if (*m_stringList==NULL)
			return false;
		strcpy((*m_stringList),string);
	}
	else
	{
		char **newList=(char **)realloc(m_stringList,(sizeof(char*)*m_listCount));
		if (newList==NULL)
			return false;
		newList[m_listCount-1]=(char *)malloc(strlen(string)+1);
		if (newList[m_listCount-1]==NULL)
			return false;
		m_stringList=newList;
		strcpy(m_stringList[m_listCount-1],string);
	}
	return true;
}

int CStringList::GetListCount()
{
	return m_listCount;
}

char **CStringList::GetList()
{
	return m_stringList;
}