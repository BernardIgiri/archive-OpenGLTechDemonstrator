#include "SliderRenderer.h"
/***********************************************************************************

  Initializes the slider renderer.

***********************************************************************************/
CSliderRenderer::CSliderRenderer(int xPos, int yPos, int width, int height, bool isHorizontal,
								 const char *backTexName, const char *buttonTexName,
								 CLog *cLog, CTextureManager *cTexMan)
{
	try
	{
		m_cLog=cLog;
		m_isHorizontal=isHorizontal;
		m_width=width;
		m_height=height;
		m_minX=xPos;
		m_minY=yPos;
		m_maxX=m_minX+(m_width-1);
		m_maxY=m_minY+(m_height-1);
		m_position=0;
		m_maxPosition=m_width-SLIDERRENDERER_BUTTONWIDTH;
		if (!m_isHorizontal)
			m_maxPosition=m_height-SLIDERRENDERER_BUTTONWIDTH;
		m_cTexMan=cTexMan;
		m_backTexture=m_cTexMan->FindTextureIndex((char *)backTexName);
		m_buttonTexture=m_cTexMan->FindTextureIndex((char *)buttonTexName);
		if (m_backTexture<0)
		{
			m_cLog->lprintf("CSliderRenderer error: backTexName %s not found.",backTexName);
			return;
		}
		
		if (m_buttonTexture<0)
		{
			m_cLog->lprintf("CSliderRenderer error: buttonTexName %s not found.",buttonTexName);
			return;
		}
		m_backIsRGBA=(m_cTexMan->GetTexBpp(m_backTexture)>24);
		m_buttonIsRGBA=(m_cTexMan->GetTexBpp(m_buttonTexture)>24);
	}
	catch (...)
	{
		m_cLog->lprintf("CSliderRenderer error: Initialization failed.");
		throw;
	}
}
/***********************************************************************************

  Deconstructs slider renderer.

***********************************************************************************/
CSliderRenderer::~CSliderRenderer()
{
	return;
}
/***********************************************************************************

  Updates slider renderer with cursor data and returns current position of
  Slider.

***********************************************************************************/
int CSliderRenderer::Update(int cursorX, int cursorY, bool mouseButton)
{
	if ((cursorX>=m_minX)&&(cursorX<=m_maxX)&&
		(cursorY>=m_minY)&&(cursorY<=m_maxY)&&mouseButton)
	{
		if (m_isHorizontal)
			m_position=cursorX-m_minX;
		else
			m_position=cursorY-m_minY;
	}
	if (m_backTexture>=0)
	{
		glEnable(GL_TEXTURE_2D);
		m_cTexMan->SelectTexture(m_backTexture);
		if (m_backIsRGBA)
		{
			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		glBegin(GL_QUADS);
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2i(m_minX, m_maxY);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2i(m_minX, m_minY);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2i(m_maxX, m_minY);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2i(m_maxX, m_maxY);
		glEnd();	
		glDisable(GL_TEXTURE_2D);
		if (m_backIsRGBA)
		{
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}
	}
	if (m_buttonTexture>=0)
	{
		glEnable(GL_TEXTURE_2D);
		m_cTexMan->SelectTexture(m_buttonTexture);
		if (m_buttonIsRGBA)
		{
			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		int bMinX,
			bMaxX,
			bMinY,
			bMaxY;
		if (m_position>m_maxPosition)
			m_position=m_maxPosition;
		else if (m_position<0)
			m_position=0;
		if (m_isHorizontal)
		{
			bMinX = m_minX+m_position;
			bMaxX = m_minX+m_position+(SLIDERRENDERER_BUTTONWIDTH-1);
			bMinY = m_minY;
			bMaxY = m_maxY;
		}
		else
		{
			bMinX = m_minX;
			bMaxX = m_maxX;
			bMinY = m_minY+m_position;
			bMaxY = m_minY+m_position+(SLIDERRENDERER_BUTTONWIDTH-1);
		}
		glBegin(GL_QUADS);
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2i(bMinX, bMaxY);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2i(bMinX, bMinY);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2i(bMaxX, bMinY);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2i(bMaxX, bMaxY);
		glEnd();	
		glDisable(GL_TEXTURE_2D);
		if (m_buttonIsRGBA)
		{
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}
	}
	return (m_position*SLIDERRENDERER_MAXVALUE)/m_maxPosition;
}
/***********************************************************************************

  Moves the slider to the set position.

***********************************************************************************/
void CSliderRenderer::SetPosition(int newPosition)
{
	m_position=newPosition;
	if (m_position<0)
		m_position=0;
	else if (m_position>SLIDERRENDERER_MAXVALUE)
		m_position=SLIDERRENDERER_MAXVALUE;
	m_position=(m_position*m_maxPosition)/SLIDERRENDERER_MAXVALUE;
}
/***********************************************************************************

  Gets the Left, Top, Right, and Bottom Coordinates of Slider.

***********************************************************************************/
void CSliderRenderer::GetCoordinates(int &left, int &top, int &right, int &bottom)
{
	left  =m_minX;
	top	  =m_maxY;
	right =m_maxX;
	bottom=m_minY;
}
/***********************************************************************************

  Returns true if cursor position occurs over the slider.

***********************************************************************************/
bool CSliderRenderer::IsOver(int cursorX, int cursorY)
{
	return ((cursorX>=m_minX)&&(cursorX<=m_maxX)&&(cursorY>=m_minY)&&(cursorY<=m_maxY));
}
/***********************************************************************************

  Moves the slider to the new x y coordinates.

***********************************************************************************/
void CSliderRenderer::MoveTo(int xPos, int yPos)
{
	m_minX=xPos;
	m_minY=yPos;
	m_maxX=m_minX+(m_width-1);
	m_maxY=m_minY+(m_height-1);
}
/*********************************************************************************

  Calculates the dimensions of a Slider with specified attributes.

*********************************************************************************/
void SliderRendererCoordinateCalculator(int &left, int &top, int &right, int &bottom,
									  int xPos, int yPos, int width, int height)
{
	left=xPos;
	bottom=yPos;
	right=left+(width-1);
	top=bottom+(height-1);
}