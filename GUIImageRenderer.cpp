#include "GUIImageRenderer.h"
/******************************************************

  Creates an instance of CGUIImageRenderer
  Set texName to NULL for no Image.

******************************************************/
CGUIImageRenderer::CGUIImageRenderer(const char *texName,
									 int xPos,int yPos,int width,int height, CLog *cLog, CTextureManager *cTexMan)
{
	if (cLog==NULL||cTexMan==NULL)
		return;
	m_render=(texName!=NULL);
	m_cLog=cLog;
	m_cTexMan=cTexMan;
	m_texture=0;
	m_isAlpha=false;
	if (m_render)
	{
		if ((m_texture=m_cTexMan->FindTextureIndex((char *)texName))<0)
		{
			m_cLog->lprintf("CGUIImageRenderer error: could not find texture.");
			m_render=false;
			return;
		}
		m_isAlpha=(m_cTexMan->GetTexBpp(m_texture)>24);
	}
	m_minY=yPos;
	m_minX=xPos;
	m_maxY=m_minY+(height-1);
	m_maxX=m_minX+(width-1);
}
/******************************************************

  Destroys instance of CGUIImageRenderer

******************************************************/
CGUIImageRenderer::~CGUIImageRenderer()
{
	return;
}
/******************************************************

  Renders Image

******************************************************/
void CGUIImageRenderer::Render()
{
	if (!m_render)
		return;
	glEnable(GL_TEXTURE_2D);
	m_cTexMan->SelectTexture(m_texture);
	if (m_isAlpha)
	{
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	glPushMatrix();
		if (m_isAlpha)
			glColor4f(1.0f,1.0f,1.0f,1.0f);
		else
			glColor3f(1.0f,1.0f,1.0f);
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
	glPopMatrix();
	if (m_isAlpha)
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}
}
/***************************************************

  Changes the image. Set to NULL for no image.

***************************************************/
void CGUIImageRenderer::ChangeImage(const char *texName)
{
	m_render=(texName!=NULL);	
	if (m_render)
	{
		if ((m_texture=m_cTexMan->FindTextureIndex((char *)texName))<0)
		{
			m_cLog->lprintf("CGUIImageRenderer error: could not find texture.");
			m_render=false;
			return;
		}
		m_isAlpha=(m_cTexMan->GetTexBpp(m_texture)>24);
	}
}
/***************************************************

  Returns true if cursor coordinates occur
  over the Image.

***************************************************/
bool CGUIImageRenderer::IsMouseOver(int cursorX,int cursorY)
{
	return ((cursorX>=m_minX)&&(cursorX<=m_maxX)&&(cursorY>=m_minY)&&(cursorY<=m_maxY));
}
/***************************************************

  Gets the coordinates of the corners of the
  Image

***************************************************/
void CGUIImageRenderer::GetCoordinates(int &left,int &top,int &right,int &bottom)
{
	left=m_minX;
	top=m_maxY;
	right=m_maxX;
	bottom=m_minY;
}
/***************************************************

  Moves image to new coordinates.

***************************************************/
void CGUIImageRenderer::MoveTo(int x,int y)
{
	int height=(m_maxY-m_minY+1);
	int width=(m_maxX-m_minX+1);
	m_minY=y;
	m_minX=x;
	m_maxY=m_minY+(height-1);
	m_maxX=m_minX+(width-1);
}
/***************************************************

  Calculates Image coordinates.

***************************************************/
void GUIImageRendererCoordinateCalculater(int &dLeft, int &dTop, int &dRight, int &dBottom,
										  int xPos,int yPos,int width,int height)
{
	dLeft=xPos;
	dBottom=yPos;
	dTop=dBottom+(height-1);
	dRight=dLeft+(width-1);
}
