#include "SelectionListRenderer.h"
/********************************************************************************

  Creates a single instance of a text feild Renderer.

********************************************************************************/
CSelectionListRenderer::CSelectionListRenderer(int fontListBase, int displayWidth,
											   int displayHeight,
											   int textHeight, int textWidth,
											   int bottom, int left, int padding,
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
		m_padding	   = padding;
		m_bottom	   = bottom;
		m_left		   = left;
		m_minX		   = m_left;
		m_maxX		   = m_minX + m_textWidth*m_displayWidth+m_padding*2;
		m_maxY		   = m_bottom + (m_textHeight)*(displayHeight)+m_padding*2;
		m_minY		   = m_bottom;
		m_adjustment   = (m_textHeight/4+((m_textHeight%4) ? 1 : 0));
		m_selectedIndex= 0;
		m_dispIndex	   = 0;
		m_listLength   = 0;
		m_maxHeight	   = 0;
		m_maxSize	   = 0;
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
		m_maxSize = strlen(m_pListText);
		m_maxHeight = m_listLength;
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

********************************************************************************/
const char *CSelectionListRenderer::UpdateAndRender(const char nextCharacter, bool drawBackground,
													bool drawHighlight, bool show)
{
	// render
	bool texturesOn = (glIsEnabled(GL_TEXTURE_2D)!=0);
	if (texturesOn)
		glDisable(GL_TEXTURE_2D);
	if (drawBackground&&show)
	{
		glPushMatrix();
			glColor3f(m_backgroundColor[0],m_backgroundColor[1],m_backgroundColor[2]);
			glBegin(GL_QUADS);
				glNormal3f(0.0f,0.0f,1.0f);
				glVertex2i(m_minX, m_maxY);
				glVertex2i(m_minX, m_minY);
				glVertex2i(m_maxX, m_minY);
				glVertex2i(m_maxX, m_maxY);
			glEnd();
		glPopMatrix();
	}
	if (drawHighlight&&show)
	{
		int offset = m_padding-((m_selectedIndex-m_dispIndex-m_displayHeight)*m_textHeight);
		glPushMatrix();
			glColor3f(m_backLightColor[0],m_backLightColor[1],m_backLightColor[2]);
			glBegin(GL_QUADS);
				glNormal3f(0.0f,0.0f,1.0f);
				glVertex2i(m_minX+m_padding, m_minY+offset);
				glVertex2i(m_minX+m_padding, m_minY+offset-m_textHeight);
				glVertex2i(m_maxX-m_padding, m_minY+offset-m_textHeight);
				glVertex2i(m_maxX-m_padding, m_minY+offset);
			glEnd();
		glPopMatrix();
	}
	if (show)
	{
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
				glRasterPos2i(m_minX+m_padding+1,m_minY+(m_textHeight)*((m_displayHeight-1)-count)+m_adjustment+m_padding+((m_minY>189) ? 1:0));
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
			}
			if (count==m_displayHeight)
				break;
			if (i==oldI)
				break;
			oldI=i;
		}
		glPopMatrix();
	}
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
	m_bottom		   = y;
	m_left		   = x;
	m_minX		   = m_left;
	m_maxX		   = m_minX + m_textWidth*m_displayWidth+m_padding*2;
	m_maxY		   = m_bottom + (m_textHeight)*(m_displayHeight)+m_padding*2;
	m_minY		   = m_bottom;
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
void CSelectionListRenderer::GetCoordinates(int &left,int &top, int &right,int &bottom)
{
	top=m_maxY;
	left=m_minX;
	bottom=m_minY;
	right=m_minX;
}
/********************************************************************************

  Returns number of lines displayed in the list.

********************************************************************************/
int CSelectionListRenderer::GetDisplayHeight()
{
	return m_displayHeight;
}
/********************************************************************************

  Returns the length of the list.

********************************************************************************/
int CSelectionListRenderer::GetListLength()
{
	return m_listLength;
}
/********************************************************************************

  Scrolls to the bottom of the list.

********************************************************************************/
void CSelectionListRenderer::ScrollToBottom()
{
	m_dispIndex=m_listLength-m_displayHeight;
	if (m_dispIndex<0)
		m_dispIndex=0;

	if (m_dispIndex>m_selectedIndex)
		m_selectedIndex=m_dispIndex;
	if (m_dispIndex<(m_selectedIndex-(m_displayHeight-1)))
		m_selectedIndex=m_dispIndex+(m_displayHeight-1);
}
/********************************************************************************

  Scrolls Down set amount of lines. (May receive positive or negative values)

********************************************************************************/
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
/********************************************************************************

  Scrolls Up set amount of lines. (May receive positive or negative values)

********************************************************************************/
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
/********************************************************************************

  Scrolls To relative position, 1.0f = max 0.5f is halfway, 0.0f = min, etc.

********************************************************************************/
void CSelectionListRenderer::ScrollTo(float percent)
{
	if (percent>1.0f)
		percent=1.0f;
	else if (percent<0.0f)
		percent=0.0f;
	m_dispIndex=(int)((float)(m_listLength-1)*percent);
	if (m_dispIndex>=m_listLength)
		m_dispIndex=m_listLength-1;
	else if (m_dispIndex<0)
		m_dispIndex=0;

	if (m_dispIndex>m_selectedIndex)
		m_selectedIndex=m_dispIndex;
	if (m_dispIndex<(m_selectedIndex-(m_displayHeight-1)))
		m_selectedIndex=m_dispIndex+(m_displayHeight-1);
}
/********************************************************************************

  Returns the scoll position, 1.0f = max 0.5f is halfway, 0.0f = min, etc.

********************************************************************************/
float CSelectionListRenderer::ScrollPosition()
{
	return (float)m_dispIndex/(float)(m_listLength-1);
}
/********************************************************************************

  Changes the height of the Selection List to set value.

********************************************************************************/
void CSelectionListRenderer::ChangeDisplayHeight(int lines)
{
	bool isReduction = (m_displayHeight>lines);
	m_displayHeight = lines;
	MoveTo(m_left,m_bottom); // update coordinates
	// update highlight
	if (isReduction && ((m_selectedIndex-m_dispIndex)>m_displayHeight)) // if a reduction and selected index is to far down
	{
		if ((m_listLength-1)==m_selectedIndex)
			m_dispIndex=m_selectedIndex;
		else
			m_dispIndex=(m_selectedIndex-(m_displayHeight-1));
	}
	ScrollDown(0);
}
/********************************************************************************

  Changes the displayed string.
  Returns false if it fails.
  Fails if string is longer than or has more lines than the initial string.
  Make sure that none of the lines of the new string exceed the width setting
  for this selection list.

********************************************************************************/
bool CSelectionListRenderer::ChangeString(const char *newString)
{
	try
	{
		int nLength = strlen(newString);
		if ((unsigned)nLength>m_maxSize)
			return false;
		int lineCount = 1;
		for (int i=0; i<nLength; i++)
			if ('\n'==newString[i])
				lineCount++;
		if (lineCount>m_maxHeight)
			return false;
		m_listLength = lineCount;
		/** /
		m_dispIndex=0;
		m_selectedIndex=0;
		/**/
		if (m_dispIndex>=m_listLength)
			m_dispIndex=m_listLength-1;
		if (m_selectedIndex>=m_listLength)
			m_selectedIndex=m_listLength-1;/**/
		memset(m_pListText,NULL,m_maxSize);
		strcpy(m_pListText,newString);
		i=0;
		int oldI=0,length=strlen(m_pListText);
		m_listTxtLen = length;
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
		for (i=index+1; i<m_maxHeight; i++)
			m_pLineIndexes[i]=m_pLineIndexes[index];
	}
	catch (...)
	{
		m_cLog->lprintf("CSelectionListRenderer error: failed to change string.");
		throw;
	}
	return true;
}
/********************************************************************************

  Returns true if cusor is within the coordinate limits of Selection List.
  This will also cause the member of the list at the cursor coordinates
  to be selected.

********************************************************************************/
bool CSelectionListRenderer::IsClicked(int cursorX, int cursorY)
{
	if ((cursorX>=m_minX)&&(cursorX<=m_maxX)&&(cursorY>=m_minY)&&(cursorY<=m_maxY))
	{
		for (int i=0; i<m_displayHeight; i++)
		{
			int offset = m_padding-((i-m_displayHeight)*m_textHeight);
			if ((cursorX>=(m_minX+m_padding))&&(cursorX<=(m_maxX-m_padding))
				&&(cursorY>=(m_minY+offset-m_textHeight))&&(cursorY<=(m_minY+offset)))
			{
				m_selectedIndex=i+m_dispIndex;
				if (m_selectedIndex>=m_listLength)
					m_selectedIndex=m_listLength-1;
				return true;
			}
		}
	}
	else
		return false;
	return true;
}
/********************************************************************************

  Returns true if cusor is within the coordinate limits of Selection List.

********************************************************************************/
bool CSelectionListRenderer::IsOver(int cursorX,int cursorY)
{
	return ((cursorX>=m_minX)&&(cursorX<=m_maxX)&&(cursorY>=m_minY)&&(cursorY<=m_maxY));
}
/************************************************************************************

  Returns a const pointer to the list text.

************************************************************************************/
const char *CSelectionListRenderer::GetListText()
{
	return m_pListText;
}
/************************************************************************************

  Dimension Calculator

************************************************************************************/
void SelectionListRendererDimensionCalculator(int &dLeft, int &dTop, int &dRight, int &dBottom,
									   int left, int bottom, int textWidth, int textHeight,
									   int displayWidth, int displayHeight, int padding)
{
	dLeft = left;
	dRight = left + textWidth*displayWidth+padding*2;
	dTop = bottom + (textHeight)*(displayHeight)+padding*2;
	dBottom = bottom;
}
