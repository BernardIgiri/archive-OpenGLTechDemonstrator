#include "TextFeildRenderer.h"
/********************************************************************************

  Creates a single instance of a text feild Renderer.

********************************************************************************/
CTextFeildRenderer::CTextFeildRenderer(int fontListBase, int displayWidth, int maxChars,
									   int textHeight, int textWidth, int top, int left,
									   int padding, CLog **cLog)
{
	try
	{
		m_cLog=(*cLog);
		m_backgroundColor[0]=1.0f;
		m_backgroundColor[1]=1.0f;
		m_backgroundColor[2]=1.0f;
		m_textColor[0]=0.0f;
		m_textColor[1]=0.0f;
		m_textColor[2]=0.0f;
		m_fontListBase = fontListBase;
		m_displayWidth = displayWidth;
		m_maxChars	   = maxChars;
		m_textHeight   = textHeight;
		m_textWidth	   = textWidth;
		m_padding	   = padding;
		m_adjustment   = (m_textHeight/4+((m_textHeight%4) ? 1 : 0));
		m_top		   = top;
		m_left		   = left;
		m_minX		   = m_left;
		m_maxX		   = m_minX + m_textWidth*m_displayWidth+m_padding*2+3;//3 =width of pipe char '|'
		m_minY		   = m_top;
		m_maxY		   = m_minY + m_textHeight+m_padding*2;
		m_cTextInput = new CTextInputSystem(m_maxChars, m_displayWidth, false);
		m_pReadText = m_cTextInput->GetOutput();
		m_pDisplayText = m_cTextInput->GetCursorString();
		m_pNoCursor = m_cTextInput->GetDisplayString();
		m_pReturnedOutput = NULL;
		m_pReturnedOutput = (char*)malloc(maxChars+1);
		memset(m_pReturnedOutput,NULL,m_maxChars+1);
	}
	catch (...)
	{
		m_cLog->lprintf("CTextFeildRenderer error: failed to initialize.");
		throw;
	}
}
/********************************************************************************

  Deconstructs text feild renderer.

********************************************************************************/
CTextFeildRenderer::~CTextFeildRenderer()
{
	try
	{
		if (m_pReturnedOutput!=NULL)
			free(m_pReturnedOutput);
		if (m_cTextInput)
			delete m_cTextInput;
	}
	catch (...)
	{
		m_cLog->lprintf("CTextFeildRenderer error: failed to deconstruct.");
		throw;
	}
}
/********************************************************************************

  Updates and renders the text feild for a 2D Orthographic Projection Mode.
  Returns NULL until it has received the enter command.
  NOTE: This will not make a call to glFlush() or SwapBuffers(HDC).

********************************************************************************/
const char *CTextFeildRenderer::UpdateAndRender(const char nextCharacter, bool drawBackground, bool show)
{
	m_cTextInput->ReadText(nextCharacter);
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
	if (show)
	{
		glPushMatrix();
			glColor3f(m_textColor[0],m_textColor[1],m_textColor[2]);
			glRasterPos2i(m_minX+m_padding+1,m_minY+m_adjustment+m_padding+((m_minY>189) ? 1:0));
			PrintString(m_fontListBase,(char *)m_pDisplayText);
		glPopMatrix();
	}
	if (texturesOn)
		glEnable(GL_TEXTURE_2D);	
	if (TEXTFEILDRETURNCOMMAND==nextCharacter)
	{
		strcpy(m_pReturnedOutput,m_pReadText);
		m_cTextInput->ResetSystem();
		return m_pReturnedOutput;
	}
	else
		return NULL;
}
/********************************************************************************

  Moves the text feild to the desired location.

********************************************************************************/
void CTextFeildRenderer::MoveTo(int x, int y)
{
	m_top		   = y;
	m_left		   = x;
	m_minX		   = m_left;
	m_maxX		   = m_minX + m_textWidth*m_displayWidth+m_padding*2+3;//3 =width of pipe char '|'
	m_minY		   = m_top;
	m_maxY		   = m_minY + m_textHeight+m_padding*2;
}
/********************************************************************************

  Sets the color for the background and text in the textfeild

********************************************************************************/
void CTextFeildRenderer::SetColor(float backR, float backG, float backB,
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

  Sets text in text feild to string.

********************************************************************************/
void CTextFeildRenderer::SetTextString(const char *string)
{
	if (string!=NULL)
		m_cTextInput->ResetSystem(string);
	else
		m_cTextInput->ResetSystem();
}
/********************************************************************************

  Sets variables to the coordinates used to draw the background plane.

********************************************************************************/
void CTextFeildRenderer::GetCoordinates(int &left, int &top, int &right, int &bottom)
{
	top=m_maxY;
	left=m_minX;
	bottom=m_minY;
	right=m_maxX;
}
/********************************************************************************

  Returns a const char pointer to the full string beinging entered by the
  user.

********************************************************************************/
const char *CTextFeildRenderer::GetTextString()
{
	return m_pReadText;
}

//bool IsClicked(int cursorX, int cursorY)
/********************************************************************************

  Returns true if cursor coordinates are within the limits of the text feild.

********************************************************************************/
bool CTextFeildRenderer::IsClicked(int cursorX, int cursorY)
{
	return ((cursorX>=m_minX)&&(cursorX<=m_maxX)&&(cursorY>=m_minY)&&(cursorY<=m_maxY));
}
/********************************************************************************

  Returns true if cursor coordinates are within the limits of the text feild.

********************************************************************************/
bool CTextFeildRenderer::IsOver(int cursorX,int cursorY)
{
	return ((cursorX>=m_minX)&&(cursorX<=m_maxX)&&(cursorY>=m_minY)&&(cursorY<=m_maxY));
}
/************************************************************************************

  Dimension Calculator

************************************************************************************/
void TextFeildRendererDimensionCalculator(int &dLeft, int &dTop, int &dRight, int &dBottom,
									   int left, int bottom, int textWidth, int textHeight,
									   int displayWidth, int padding)
{
	dLeft = left;
	dRight = left + textWidth*displayWidth+padding*2+3;//3 =width of pipe char '|'
	dTop = bottom + textHeight+padding*2;
	dBottom = bottom;
}