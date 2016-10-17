#include "Consol.h"
#include <stdio.h>
/*******************************************************************

  Initializes the consol with the coordinates of the screen limit,
  font list base, textHeight measurement, max amount of characters
  permitted to be displayed horizontally on the screen (Note: this
  only limits display, not input), and max number of lines of text
  the consol will be permitted to display.

*******************************************************************/
CConsol::CConsol(HDC hdc, int fontListBase, int numberOfLines,
				 int displayedLines, float textHeight,
				 float leftX,float topY,float rightX,
				 float bottomY, float zPos,
				 float startRasX, float startRasY,
				 int maxCharacterDisplay,
				 int maxCharacterInput)
{
	m_sendThru = false;
	m_rendering = true;
	m_hdc = hdc;
	m_rasX = startRasX;
	m_rasY = startRasY;
	m_minX = leftX;
	m_minY = topY;
	m_maxX = rightX;
	m_zPos = zPos;
	m_textHeight = textHeight;
	m_fontListBase = fontListBase;
	m_numberOfLines = numberOfLines;
	if (m_numberOfLines<2)
		m_numberOfLines=2;
	m_maxCharacterDisplay = maxCharacterDisplay;
	m_displayedLines = displayedLines;
	if (m_displayedLines<2)
		m_displayedLines=2;
	if (m_cConsolText.Initialize())
		for (int i=0; i<m_numberOfLines; i++)
			m_cConsolText.AddStringToList(" ");
	m_cTextInput = new CTextInputSystem(maxCharacterInput, maxCharacterDisplay, false);
	m_pNoCursor = m_cTextInput->GetDisplayString();
	m_pReadText = m_cTextInput->GetOutput();
	m_pDisplayText = m_cTextInput->GetCursorString();
	m_ppDisplayTextList = m_cConsolText.GetList();
	m_pReturnedOutput = NULL;
	m_pReturnedOutput = (char*)malloc(maxCharacterInput+1);

	if (m_displayedLines>numberOfLines)
		m_displayedLines=numberOfLines;
	m_maxY=0.5f-((m_textHeight*1.1f)*(m_displayedLines));
	m_dispScroll = m_numberOfLines-m_displayedLines;
	m_echoState=true;
}
/*******************************************************************

  Destructs the consol.

*******************************************************************/
CConsol::~CConsol()
{
	if (m_pReturnedOutput!=NULL)
		free(m_pReturnedOutput);
	if (m_cTextInput)
		delete m_cTextInput;
}
/*********************************************************************

  Updates and Renders the Consol after reading the nex character
  received by the input system. When the consol has received a
  command (when the user hits enter) this will return a const char
  pointer to the enter string.  Otherwise it will just return NULL.
  Send NULL as the nextCharacter just to render the consol.
  Sends back "RedrawScreen" if an update requires the background
  to be redrawn. (ie if the display lines were reduced)
  Consol background is not always redrawn (for performance
  optimization) however passing true to renderAll ensures that it
  is.

*********************************************************************/
const char *CConsol::UpdateAndRender(const unsigned char nextCharacter, bool renderAll)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	if (m_rendering)
	{/////////////////////////////////////////render
	if (renderAll)
		m_echoState=true;
	m_cTextInput->ReadText(nextCharacter);
	glTranslatef(0.0f,0.0f,m_zPos);
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
		glColor3f(0.0f,0.0f,0.0f);
		static int oldDispLines=m_displayedLines;
		if (m_echoState)
		{
			oldDispLines=m_displayedLines;
			glBegin(GL_QUADS);
				glNormal3f(0.0f,0.0f,1.0f);
				glVertex3f(m_maxX, m_maxY, -0.1f);
				glVertex3f(m_maxX, m_minY, -0.1f);
				glVertex3f(m_minX, m_minY, -0.1f);
				glVertex3f(m_minX, m_maxY, -0.1f);
			glEnd();
			glColor3f(1.0f,1.0f,1.0f);
			float oldWidth;
			glGetFloatv(GL_LINE_WIDTH,&oldWidth);
			glLineWidth(2.0f);
			glBegin(GL_LINES);
				glNormal3f(0.0f,0.0f,1.0f);
				glVertex3f(m_maxX, m_maxY, -0.09f);
				glVertex3f(m_minX, m_maxY, -0.09f);
			glEnd();
			glLineWidth(oldWidth);
		}
		else
		{ // if full background need not be shown show only the part behind last 2 lines of text
			glBegin(GL_QUADS);
				glNormal3f(0.0f,0.0f,1.0f);
				glVertex3f(m_maxX, m_maxY+0.01f, -0.1f);
				glVertex3f(m_maxX, m_maxY+(m_textHeight*3.2f), -0.1f);
				glVertex3f(m_minX, m_maxY+(m_textHeight*3.2f), -0.1f);
				glVertex3f(m_minX, m_maxY+0.01f, -0.1f);
			glEnd();
		}
	glPopMatrix();

	glPushMatrix();
		glPushMatrix();			
			glColor3f(1.0f,1.0f,1.0f);
			if (m_echoState)
			{
				for (int i=0; i<(m_displayedLines-1); i++)
				{
					glRasterPos2f(m_rasX,m_rasY-(float)i*m_textHeight);
					PrintString(m_fontListBase,m_ppDisplayTextList[i+m_dispScroll+1]);
				}
				m_echoState=false;
			}
			else
			{
				glRasterPos2f(m_rasX,m_rasY-(float)(m_displayedLines-2)*m_textHeight);
				PrintString(m_fontListBase,m_ppDisplayTextList[(m_displayedLines-2)+m_dispScroll+1]);
			}
			glRasterPos2f(m_rasX,m_rasY-(float)(m_displayedLines-1)*m_textHeight);
			PrintString(m_fontListBase,(char *)m_pDisplayText);			
		glPopMatrix();
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);

	glFlush();
	SwapBuffers(m_hdc);
	}////////////////////////////////////////render

	if (nextCharacter==CONSOLENTERCOMMANDCHAR||m_sendThru)
	{
		strcpy(m_pReturnedOutput,m_pReadText);
		//m_cConsolText.ChangeString(m_numberOfLines-1,m_pNoCursor);
		EchoFMessage(m_pReturnedOutput,!m_sendThru);
		m_cTextInput->ResetSystem(m_pNoCursor);
		m_sendThru=false;
		return m_pReturnedOutput;
	}
	else
	{
		if (CONSOLCOMMANDGETLASTSTRINGCHAR==nextCharacter && m_pReturnedOutput!=NULL)
			m_cTextInput->ResetSystem(m_pReturnedOutput);
		else if (CONSOLCOMMANDCLEARALLCHAR==nextCharacter)
			m_cTextInput->ResetSystem();
		else if (CONSOLCOMMANDDROPDOWNCHAR==nextCharacter)
		{
			if (m_displayedLines<m_numberOfLines)
				SetDisplayLines(m_numberOfLines);
			else
			{
				SetDisplayLines(2);
				return "RedrawScreen";
			}
		}
		return NULL;
	}
}
/*******************************************************************

  Echo's message on the consol. Returns true if successful.
  Set render to true to display after update.

*******************************************************************/
bool CConsol::EchoMessage(const char *msg, bool render)
{
	m_echoState=true;
	bool success = m_cConsolText.CycleUp(msg);
	if (success && render)
		UpdateAndRender(NULL);
	return success;
}
/*******************************************************************

  Echo's Formatted Text message on the consol.
  Returns true if successful.
  Set render to true to display during update.

*******************************************************************/
bool CConsol::EchoFMessage(const char*msg,bool render)
{
	bool result = true;
	char *newString=NULL;
	int *lineIndexes=NULL;
	int lines = StrUtil_WordWrapS(" ,;'\"-+*/",msg,m_maxCharacterDisplay,
								  &newString,lineIndexes);
	char *lineString = NULL;
	lineString=(char *)malloc(m_maxCharacterDisplay+3);
	if (NULL==lineString || NULL==newString)
		return false;
	memset(lineString,NULL,m_maxCharacterDisplay+3);
	int i=0, oldI=0, length=strlen(newString);
	while (i<length)
	{
		if (StrUtil_ReadNextLine(lineString,newString+i,&i,m_maxCharacterDisplay+2))
			result=(result&&EchoMessage(lineString,render));
		if (i==oldI)
		break;
		oldI=i;
	}
	free(lineString);
	free(newString);
	free(lineIndexes);
	return result;
}
/*******************************************************************

  Echo's Unformatted Text message on the consol
  via printf methods. Returns true if successful.
  Set render to true to display during update.
  Buffer limited to 1024 chars.

*******************************************************************/
bool CConsol::EchoPrintF(bool render, const char *format,...)
{
	va_list arglist;
	char buff[1024];
	memset(buff,NULL,sizeof(buff));
	va_start(arglist,format);
	vsprintf(buff,format,arglist);
	va_end(arglist);
	return EchoFMessage(buff,render);
}
/*******************************************************************

  Returns the current number of lines being displayed.

*******************************************************************/
int CConsol::GetDisplayLines()
{
	return m_displayedLines;
}
/*******************************************************************

  Returns the max number of lines the system is set to contain.

*******************************************************************/
int CConsol::GetMaxLines()
{
	return m_numberOfLines;
}
/*******************************************************************

  Changes the amount of displayed lines. NOTE: Using this function
  will not cause the "RedrawScreen" command to be posted.

*******************************************************************/
void CConsol::SetDisplayLines(int lines)
{
	m_echoState=true;
	if (lines<=m_numberOfLines && lines>=2)
	{
		m_displayedLines=lines;
		m_dispScroll = m_numberOfLines-m_displayedLines;
		//m_maxY=0.5f-(m_textHeight*m_displayedLines);
		m_maxY=0.5f-((m_textHeight*1.1f)*(m_displayedLines));
	}
}
/********************************************************************************

  Sets text entry feild in consol to string.

********************************************************************************/
void CConsol::SetTextString(const char *string)
{
	m_cTextInput->ResetSystem(string);
}
/*******************************************************************

  Sets text entry feild in consol to string
  via printf methods.
  Buffer limited to 1024 chars.

*******************************************************************/
void CConsol::SetTextStringPF(const char *format,...)
{
	va_list arglist;
	char buff[1024];
	memset(buff,NULL,sizeof(buff));
	va_start(arglist,format);
	vsprintf(buff,format,arglist);
	va_end(arglist);
	m_cTextInput->ResetSystem(buff);
}

void CConsol::SendThru()
{
	m_sendThru=true;
}

void CConsol::TurnRenderingOff()
{
	m_rendering=false;
}
void CConsol::TurnRenderingOn()
{
	m_rendering=true;
}
bool CConsol::GetRenderingState()
{
	return m_rendering;
}