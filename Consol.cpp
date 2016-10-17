#include "Consol.h"
#include <stdio.h>
CConsol::CConsol(int fontListBase, int textWidth, int textHeight, int screenWidth, int screenHeight,CLog *cLog,
				   int textBufferSize,int maxCharacterDisplay,int maxCharacterInput)
{
	try
	{
		m_cLog=cLog;
		m_fontListBase=fontListBase;
		m_minX=0;
		m_maxX=screenWidth-1;
		m_maxY=screenHeight-1;
		m_maxCharDisplay=maxCharacterDisplay;
		m_maxLines=screenHeight/textHeight-1;
		m_buffSize=textBufferSize;
		m_displayedLines=3;
		m_minY=m_maxY-(textHeight*m_displayedLines);
		m_sendThru=false;
		m_isOk=true;
		m_redrawAll=false;
		m_isVisible=true;
		{
			char *str=NULL;
			str=(char*)malloc(m_buffSize);
			if (str==NULL)
			{
				m_cLog->lprintf("Consol error: failed to initialize.");
				m_isOk=false;
				return;
			}
			memset(str,'.',m_buffSize-1);
			str[m_buffSize-1]=NULL;
			int scale=m_buffSize/m_maxLines;
			scale/=2;
			int count=0;
			for (int i=scale-1; i<m_buffSize; i+=scale)
			{
				str[i]='\n';
				count++;
			}
			m_cDispList=NULL;
			m_cDispList=new CSelectionListRenderer(m_fontListBase,m_maxCharDisplay,m_displayedLines-1,
									   textHeight,textWidth,m_minY+textHeight,m_minX,
									   0,str,&m_cLog);

			m_cTextFeild=NULL;
			m_cTextFeild=new CTextFeildRenderer(m_fontListBase,m_maxCharDisplay,maxCharacterInput,
									   textHeight,textWidth,m_minY,m_minX,
									   0,&m_cLog);
			if (m_cDispList==NULL||m_cTextFeild==NULL)
			{
				m_cLog->lprintf("Consol error: failed to initialize.");
				m_isOk=false;
				return;
			}
			m_cDispList->SetColor(0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f);
			m_cDispList->SetHighlightColor(0.25f,0.25f,0.25f, 1.0f,1.0f,1.0f);
			m_cDispList->ScrollDown(m_maxLines+1);
			m_cTextFeild->SetColor(0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f);
		}
	}
	catch (...)
	{
		m_cLog->lprintf("Consol error: failed to initialize.");
		m_isOk=false;
		throw;
	}
}
CConsol::~CConsol()
{
	if (m_cDispList!=NULL)
		delete m_cDispList;
	if (m_cTextFeild!=NULL)
		delete m_cTextFeild;
}
/*************************************************************************

  Returns true if no errors have occured.

*************************************************************************/
bool CConsol::GetIsOk()
{
	return m_isOk;
}
/*************************************************************************

  Returns the current number of displayed lines.

*************************************************************************/
int CConsol::GetDisplayLines()
{
	return m_displayedLines;
}
/*************************************************************************

  Updates and Renders the Consol.

*************************************************************************/
const char *CConsol::UpdateAndRender(const unsigned char nextCharacter, bool renderAll)
{
	if (m_redrawAll)
	{
		renderAll=true;
		m_redrawAll=false;
	}
	char send=nextCharacter;
	static bool dispListSelected=false;
	if (m_sendThru)
	{
		m_sendThru=false;
		dispListSelected=false;
		send=INPUTMAN_TEXT_CODE_ENTER;
	}
	if (renderAll&&m_isVisible)
	{
		glDisable(GL_TEXTURE_2D);
		glPushMatrix();
			glColor3f(0.0f,0.0f,0.0f);
			glBegin(GL_QUADS);
				glNormal3f(0.0f,0.0f,1.0f);
				glVertex2i(m_minX, m_maxY);
				glVertex2i(m_minX, m_minY);
				glVertex2i(m_maxX, m_minY);
				glVertex2i(m_maxX, m_maxY);
			glEnd();
		glPopMatrix();
	}
	if (dispListSelected)
	{
		if (send==' ')
			send=INPUTMAN_TEXT_CODE_ENTER;
		const char *chosen = m_cDispList->UpdateAndRender(send,true,true,m_isVisible);
		if (chosen!=NULL)
			m_cTextFeild->SetTextString(chosen);
	}
	else if (renderAll)
		m_cDispList->UpdateAndRender(NULL,renderAll,false,m_isVisible);
	const char *result=NULL;
	if (!dispListSelected)
		result=m_cTextFeild->UpdateAndRender(send,true,m_isVisible);
	else if (renderAll)
		result=m_cTextFeild->UpdateAndRender(NULL,renderAll,m_isVisible);
	if (result!=NULL)
		EchoPrintF(result);
	if (nextCharacter==CONSOLSWITCHCOMMANDCHAR)
		dispListSelected=(!dispListSelected);
	if (renderAll&&m_isVisible)
	{
		glPushMatrix();
			glColor3f(1.0f,1.0f,1.0f);
			glBegin(GL_LINES);
				glNormal3f(0.0f,0.0f,1.0f);
				glVertex2i(m_minX, m_minY);
				glVertex2i(m_maxX, m_minY);
			glEnd();
		glPopMatrix();
	}
	switch (nextCharacter)
	{
	case CONSOLCOMMANDDROPDOWNCHAR:
		if (m_displayedLines<m_maxLines)
			SetDisplayLines(m_maxLines);
		else
			SetDisplayLines(3);
		break;
	case CONSOLCOMMANDCLEARALLCHAR:
		m_cTextFeild->SetTextString(NULL);
		break;
	default:
		break;
	}
	return result;
}
/*************************************************************************

  Echoes Message on the Consol.  Returns False on failure.

*************************************************************************/
bool CConsol::EchoPrintF(const char *format,...)
{
	va_list arglist;
	char buff[1024];
	memset(buff,NULL,sizeof(buff));
	va_start(arglist,format);
	vsprintf(buff,format,arglist);
	va_end(arglist);

	bool result = true;
	char *newString=NULL;
	int *lineIndexes=NULL;
	int lines = StrUtil_WordWrapS(" ,;'\"-+*/",buff,m_maxCharDisplay-3,
								  &newString,lineIndexes);
	char *lineString = NULL;
	lineString=(char *)malloc(m_maxCharDisplay+3);
	if (NULL==lineString || NULL==newString)
		return false;
	memset(lineString,NULL,m_maxCharDisplay+3);
	int i=0, oldI=0, length=strlen(newString);
	while (i<length)
	{
		if (StrUtil_ReadNextLine(lineString,newString+i,&i,m_maxCharDisplay+2))
			result=(result&&EchoMessage(lineString));
		if (i==oldI)
		break;
		oldI=i;
	}
	free(lineString);
	free(newString);
	free(lineIndexes);
	return result;
}
/*************************************************************************

  Echoes Message on the Consol.  Returns False on failure.
  NOTE: This method performs no formating, not even word wrapping.

*************************************************************************/
bool CConsol::EchoMessage(const char *msg)
{
	int count=0, msgLen=strlen(msg);
	for (int i=0;i<msgLen;i++)
		if (msg[i]=='\n')
			count++;
	char *newString=NULL;
	newString=(char *)malloc(m_buffSize);
	if (newString==NULL)
		return false;
	strcpy(newString,m_cDispList->GetListText());
	int nwstrLen=strlen(newString);
	if ((msgLen+nwstrLen)>m_buffSize)
	{
		strcpy(newString,newString+(m_buffSize/2));
		if (newString[0]=='\n')
			newString[0]='.';
	}
	strcat(newString,"\n");
	strcat(newString,msg);
	m_cDispList->ChangeString(newString);
	m_cDispList->ScrollToBottom();
	m_redrawAll=true;
	return true;
}
/*************************************************************************

  Forces the next call to Update to return the data currently in the
  text feild.

*************************************************************************/
void CConsol::SendThru()
{
	m_sendThru=true;
}
/*************************************************************************

  Sets the data in the text feild to the new string.

*************************************************************************/
void CConsol::SetTextStringPF(const char *format,...)
{
	va_list arglist;
	char buff[1024];
	memset(buff,NULL,sizeof(buff));
	va_start(arglist,format);
	vsprintf(buff,format,arglist);
	va_end(arglist);
	m_cTextFeild->SetTextString(buff);
}
/*************************************************************************

  Changes the number of lines shown on the screen.
  3 is the minimum.

*************************************************************************/
void CConsol::SetDisplayLines(int lines)
{
	if (lines<3||lines>m_maxLines||lines==m_displayedLines)
		return;
	m_redrawAll=true;
	int oldLines=m_displayedLines;
	m_displayedLines=lines;
	int left,top,right,bottom;
	m_cDispList->GetCoordinates(left,top,right,bottom);
	int oldHeight=top-bottom+1;
	m_cDispList->ChangeDisplayHeight(m_displayedLines-1);
	m_cDispList->GetCoordinates(left,top,right,bottom);
	int yShiftFactor=oldHeight-(top-bottom+1);
	m_cDispList->MoveTo(left,bottom+yShiftFactor);
	m_cTextFeild->GetCoordinates(left,top,right,bottom);
	m_minY=bottom+yShiftFactor;
	m_cTextFeild->MoveTo(left,m_minY);
	m_cDispList->ScrollToBottom();
}
/*************************************************************************

  Sets Visibility to true.

*************************************************************************/
void CConsol::Show()
{
	m_isVisible=true;
}
/*************************************************************************

  Sets Visibility to false.

*************************************************************************/
void CConsol::Hide()
{
	m_isVisible=false;
}
/*************************************************************************

  Returns Visibility state.

*************************************************************************/
bool CConsol::GetIsVisible()
{
	return m_isVisible;
}