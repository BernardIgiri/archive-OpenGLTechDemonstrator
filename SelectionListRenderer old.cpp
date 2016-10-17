#include "SelectionListRenderer.h"
/********************************************************************************

  Creates a single instance of a text feild Renderer.

********************************************************************************/
CSelectionListRenderer::CSelectionListRenderer(int fontListBase, int displayWidth,
											   int displayHeight,
											   int textHeight, int textWidth,
											   int top, int left, int padding,
											   const char *listText, CLog **cLog)
{
	try
	{
		m_cLog=(*cLog);
		m_backgroundColor[0]=1.0f;
		m_backgroundColor[1]=1.0f;
		m_backgroundColor[2]=1.0f;
		m_highLightColor[0]=0.0f;
		m_highLightColor[1]=0.0f;
		m_highLightColor[2]=0.0f;
		m_backLightColor[0]=1.0f;
		m_backLightColor[1]=1.0f;
		m_backLightColor[2]=0.0f;
		m_textColor[0]=0.0f;
		m_textColor[1]=0.0f;
		m_textColor[2]=0.0f;
		m_fontListBase = fontListBase;
		m_displayWidth = displayWidth;
		m_displayHeight= displayHeight;
		m_textHeight   = textHeight;
		m_textWidth	   = textWidth;
		m_padding	   = padding+2; // 2 is a necessary adjustment factor
		m_top		   = top;
		m_left		   = left;
		m_minX		   = m_left;
		m_maxX		   = m_minX + m_textWidth*m_displayWidth+m_padding*2;
		m_minY		   = m_top;
		//m_maxY		   = (m_minY + m_textHeight+4)*displayHeight+(m_padding-2)*2; // 4 adjustment factor
		m_maxY		   = m_minY + (m_textHeight+4)*(displayHeight-1)+m_padding*2;
		m_selectedIndex= 0;
		m_dispIndex	   = 0;
		m_listLength   = 0;
		m_pLineIndexes = NULL;
		m_pSelectedStr = NULL;
		m_pLineString  = NULL;
		m_pListText	   = NULL;
		size_t buffSize = strlen(listText)+1;
		m_pListText	   = (char *)malloc(buffSize);
		memset(m_pListText,NULL,buffSize);
		strcpy(m_pListText,listText);
		// count && set indexes
		m_pLineString=(char *)malloc(m_displayWidth+3);
		m_pSelectedStr=(char *)malloc(m_displayWidth+3);
		/*if (NULL==m_pLineString || NULL==newString)
			return false;*/
		memset(m_pLineString,NULL,m_displayWidth+3);
		memset(m_pSelectedStr,NULL,m_displayWidth+3);
		//count
		int i=0, oldI=0, length=strlen(m_pListText);
		m_listTxtLen = length;
		while (i<length)
		{
			if (StrUtil_ReadNextLine(m_pLineString,m_pListText+i,&i,m_displayWidth+2))
			{
				m_listLength++;
			}
			if (i==oldI)
			break;
			oldI=i;
		}
		// get indexes
		size_t liSize = (m_listLength+1)*sizeof(int);
		m_pLineIndexes = (int *)malloc(liSize);
		memset(m_pLineIndexes,0,liSize);
		i=0;
		oldI=0;
		m_pLineIndexes[0]=0;
		int index=0;
		while (i<length)
		{
			if (StrUtil_ReadNextLine(m_pLineString,m_pListText+i,&i,m_displayWidth+2))
			{
				index++;
				m_pLineIndexes[index]=i;
			}
			if (i==oldI)
			break;
			oldI=i;
		}
	}
	catch (...)
	{
		m_cLog->lprintf("CSelectionListRenderer error: failed to initialize.");
		throw;
	}
}
/********************************************************************************

  Deconstructs text feild renderer.

********************************************************************************/
CSelectionListRenderer::~CSelectionListRenderer()
{
	try
	{
		if (m_pListText!=NULL)
			free(m_pListText);
		if (m_pLineString!=NULL)
			free(m_pLineString);
		if (m_pSelectedStr!=NULL)
			free(m_pSelectedStr);
		if (m_pLineIndexes!=NULL)
			free(m_pLineIndexes);
	}
	catch (...)
	{
		m_cLog->lprintf("CSelectionListRenderer error: failed to deconstruct.");
		throw;
	}
}
/********************************************************************************

  Updates and renders the text feild for a 2D Orthographic Projection Mode.
  Returns NULL until it has received the enter command.
  NOTE: This will not make a call to glFlush() or SwapBuffers(HDC).

********************************************************************************/
const char *CSelectionListRenderer::UpdateAndRender(const char nextCharacter, bool drawBackground,
													bool drawHighlight)
{
	// render
	glClear(GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	bool texturesOn = (glIsEnabled(GL_TEXTURE_2D)!=0);
	if (texturesOn)
		glDisable(GL_TEXTURE_2D);
	if (drawBackground)
	{
		glPushMatrix();
			glColor3f(m_backgroundColor[0],m_backgroundColor[1],m_backgroundColor[2]);
			glBegin(GL_QUADS);
				glNormal3f(0.0f,0.0f,1.0f);
				glVertex2i(m_minX, m_maxY-3);
				glVertex2i(m_minX, m_minY+m_padding);
				glVertex2i(m_maxX, m_minY+m_padding);
				glVertex2i(m_maxX, m_maxY-3);
			glEnd();
		glPopMatrix();
		glClear(GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
	}
	if (drawHighlight)
	{
		glPushMatrix();
			glColor3f(m_backLightColor[0],m_backLightColor[1],m_backLightColor[2]);
			glBegin(GL_QUADS);
				glNormal3f(0.0f,0.0f,1.0f);
				/*glVertex2i(m_minX, (((m_displayHeight-1)-m_selectedIndex)+1-m_dispIndex)*m_textHeight+(m_minY+m_padding));
				glVertex2i(m_minX, (((m_displayHeight-1)-m_selectedIndex)-m_dispIndex)*m_textHeight+(m_minY+m_padding));
				glVertex2i(m_maxX, (((m_displayHeight-1)-m_selectedIndex)-m_dispIndex)*m_textHeight+(m_minY+m_padding)); // m_maxX needs to be adjusted by -2+widthOfPipeChar // '|'
				glVertex2i(m_maxX, (((m_displayHeight-1)-m_selectedIndex)+1-m_dispIndex)*m_textHeight+(m_minY+m_padding));*/
				/*glVertex2i(m_minX, (((m_displayHeight-1-m_dispIndex)-m_selectedIndex-m_dispIndex)+1)*m_textHeight+(m_minY+m_padding));
				glVertex2i(m_minX, (((m_displayHeight-1-m_dispIndex)-m_selectedIndex-m_dispIndex))*m_textHeight+(m_minY+m_padding));
				glVertex2i(m_maxX, (((m_displayHeight-1-m_dispIndex)-m_selectedIndex-m_dispIndex))*m_textHeight+(m_minY+m_padding));
				glVertex2i(m_maxX, (((m_displayHeight-1-m_dispIndex)-m_selectedIndex-m_dispIndex)+1)*m_textHeight+(m_minY+m_padding));*/
				glVertex2i(m_minX, (((m_displayHeight-1)-(m_selectedIndex-m_dispIndex)+1)*m_textHeight+(m_minY+m_padding*3/2))-2);
				glVertex2i(m_minX, (((m_displayHeight-1)-(m_selectedIndex-m_dispIndex))*m_textHeight+(m_minY+m_padding*3/2))-2);
				glVertex2i(m_maxX, (((m_displayHeight-1)-(m_selectedIndex-m_dispIndex))*m_textHeight+(m_minY+m_padding*3/2))-2);
				glVertex2i(m_maxX, (((m_displayHeight-1)-(m_selectedIndex-m_dispIndex)+1)*m_textHeight+(m_minY+m_padding*3/2))-2);
			glEnd();
		glPopMatrix();
		glClear(GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
	}
	glPushMatrix();
		if (m_selectedIndex==m_dispIndex)
			glColor3f(m_highLightColor[0],m_highLightColor[1],m_highLightColor[2]);
		else
			glColor3f(m_textColor[0],m_textColor[1],m_textColor[2]);
		int i=m_pLineIndexes[m_dispIndex], oldI=0, count=0;
		while (i<m_listTxtLen)
		{
			if (StrUtil_ReadNextLine(m_pLineString,m_pListText+i,&i,m_displayWidth+2))
			{
				glRasterPos2i(m_minX+m_padding-1,m_maxY+
					-(m_textHeight+0)*count-m_textHeight);
				if ((count+m_dispIndex)==m_selectedIndex)
					strcpy(m_pSelectedStr,m_pLineString);
				count++;
				if ((count+m_dispIndex)==m_selectedIndex)
				{					
					glPushMatrix();
						glColor3f(m_highLightColor[0],m_highLightColor[1],m_highLightColor[2]);
						PrintString(m_fontListBase,m_pLineString);
					glPopMatrix();
				}
				else
				{
					glPushMatrix();
						glColor3f(m_textColor[0],m_textColor[1],m_textColor[2]);
						PrintString(m_fontListBase,m_pLineString);
					glPopMatrix();
				}
				/*if (count==m_selectedIndex)
					glColor3f(m_textColor[0],m_textColor[1],m_textColor[2]);*/
			}
			if (count==m_displayHeight)
				break;
			if (i==oldI)
				break;
			oldI=i;
		}
	glPopMatrix();
	if (texturesOn)
		glEnable(GL_TEXTURE_2D);
	// get input	
	if (SELECTIONLISTPAGEUPCOMMAND==nextCharacter)
		m_selectedIndex-=(m_displayHeight-1);
	else if (SELECTIONLISTPAGEDOWNCOMMAND==nextCharacter)
		m_selectedIndex+=(m_displayHeight-1);
	else if (SELECTIONLISTUPCOMMAND==nextCharacter)
		m_selectedIndex--;
	else if (SELECTIONLISTDOWNCOMMAND==nextCharacter)
		m_selectedIndex++;
	else if (SELECTIONLISTHOMECOMMAND==nextCharacter)
		m_selectedIndex=0;
	else if (SELECTIONLISTENDCOMMAND==nextCharacter)
		m_selectedIndex=(m_listLength-1);
	// set limits	
	if (m_selectedIndex<0)
		m_selectedIndex=0;
	else if (m_selectedIndex>=m_listLength)
		m_selectedIndex=(m_listLength-1);
	// scroll if necessary
	if (m_selectedIndex<m_dispIndex)
		m_dispIndex=m_selectedIndex;
	else if (m_selectedIndex>=(m_dispIndex-1+m_displayHeight))
		m_dispIndex = m_selectedIndex-(m_displayHeight-1);
	// return output
	if (SELECTIONLISTRETURNCOMMAND==nextCharacter)
	{
		return m_pSelectedStr;
	}
	else
		return NULL;
}
/********************************************************************************

  Moves the selection list to the desired location.

********************************************************************************/
void CSelectionListRenderer::MoveTo(int x, int y)
{
	m_top		   = y;
	m_left		   = x;
	m_minX		   = m_left;
	m_maxX		   = m_minX + m_textWidth*m_displayWidth+m_padding*2;
	m_minY		   = m_top;
	m_maxY		   = m_minY + (m_textHeight+4)*(m_displayHeight-1)+m_padding*2;
}
/********************************************************************************

  Sets the color for the background and text in the selection list

********************************************************************************/
void CSelectionListRenderer::SetColor(float backR, float backG, float backB,
								  float textR, float textG, float textB)
{
	m_backgroundColor[0]=backR;
	m_backgroundColor[1]=backG;
	m_backgroundColor[2]=backB;
	m_textColor[0]=textR;
	m_textColor[1]=textG;
	m_textColor[2]=textB;
}
/********************************************************************************

  Sets the highlight color for the text and the bar behind the text in the
  selection list.

********************************************************************************/
void CSelectionListRenderer::SetHighlightColor(float backR, float backG, float backB,
								  float textR, float textG, float textB)
{
	m_backLightColor[0]=backR;
	m_backLightColor[1]=backG;
	m_backLightColor[2]=backB;
	m_highLightColor[0]=textR;
	m_highLightColor[1]=textG;
	m_highLightColor[2]=textB;
}
/********************************************************************************

  Sets variables to the coordinates used to draw the background plane.

********************************************************************************/
void CSelectionListRenderer::GetCoordinates(int &top, int &left, int &bottom, int &right)
{
	top=m_minY+m_padding;
	left=m_minX;
	bottom=m_maxY-3;
	right=m_maxX;
}

int CSelectionListRenderer::GetDisplayHeight()
{
	return m_displayHeight;
}

int CSelectionListRenderer::GetListLength()
{
	return m_listLength;
}

void CSelectionListRenderer::ScrollDown(int lines)
{
	int distance = m_selectedIndex-m_dispIndex;

	m_dispIndex+=lines;
	if (m_dispIndex>=m_listLength)
		m_dispIndex=m_listLength-1;
	else if (m_dispIndex<0)
		m_dispIndex=0;

	if (m_dispIndex>m_selectedIndex)
		m_selectedIndex=m_dispIndex;
	if (m_dispIndex<(m_selectedIndex-(m_displayHeight-1)))
		m_selectedIndex=m_dispIndex+(m_displayHeight-1);
}
void CSelectionListRenderer::ScrollUp(int lines)
{
	m_dispIndex-=lines;
	if (m_dispIndex>=m_listLength)
		m_dispIndex=m_listLength-1;
	else if (m_dispIndex<0)
		m_dispIndex=0;

	if (m_dispIndex>m_selectedIndex)
		m_selectedIndex=m_dispIndex;
	if (m_dispIndex<(m_selectedIndex-(m_displayHeight-1)))
		m_selectedIndex=m_dispIndex+(m_displayHeight-1);
}