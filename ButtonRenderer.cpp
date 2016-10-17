#include "ButtonRenderer.h"

CButtonRenderer::CButtonRenderer(int top, int left, int width, int height, const char *buttonText,
								 int buttonTextWidthPx, int buttonTextHeightPx,
								 char *defaultTexName, char *mouseOverTexName,
								 char *mouseDownTexName, unsigned int fontListBase, CLog *cLog,
								 CTextureManager *cTexMan,bool toggle)
{
	try
	{
		m_toggle=toggle;
		m_textColor[0]=0.0f;
		m_textColor[1]=0.0f;
		m_textColor[2]=0.0f;
		m_state=m_oldState=BUTTONRENDERER_STATE_DEFAULT;
		m_minY = top;
		m_minX = left;
		m_maxY = m_minY+(height-1);
		m_maxX = m_minX+(width-1);
		m_font = fontListBase;
		m_cLog = cLog;
		m_cTexMan = cTexMan;
		m_defaultTex = cTexMan->FindTextureIndex(defaultTexName);
		m_mouseOverTex = -1;
		m_mouseDownTex = -1;
		if (mouseOverTexName!=NULL)
			m_mouseOverTex = cTexMan->FindTextureIndex(mouseOverTexName);
		if (mouseDownTexName!=NULL)
			m_mouseDownTex = cTexMan->FindTextureIndex(mouseDownTexName);
		m_buttonText = NULL;
		if (buttonText!=NULL)
		{
			m_buttonText = (char*)malloc(strlen(buttonText)+1);
			strcpy(m_buttonText,buttonText);
		}
		m_textWidth = buttonTextWidthPx;
		m_textHeight = buttonTextHeightPx+(buttonTextHeightPx/4+((buttonTextHeightPx%4) ? 1 : 0));
		m_pX = (width/2) - (m_textWidth/2) + m_minX;
		m_pY = (height/2) - (m_textHeight/2) + m_minY+(buttonTextHeightPx/4+((buttonTextHeightPx%4) ? 1 : 0)/2);

		m_defaultTexIsAlpha=m_mouseOverTexIsAlpha=m_mouseDownTexIsAlpha=false;
		if (m_defaultTex>=0)
			m_defaultTexIsAlpha=(m_cTexMan->GetTexBpp(m_defaultTex)>24);
		if (m_mouseOverTex>=0)
			m_mouseOverTexIsAlpha=(m_cTexMan->GetTexBpp(m_mouseOverTex)>24);
		if (m_mouseDownTex>=0)
			m_mouseDownTexIsAlpha=(m_cTexMan->GetTexBpp(m_mouseDownTex)>24);
	}
	catch (...)
	{
		m_cLog->lprintf("CButtonRenderer error: failed to initialize.");
		throw;
	}
}
CButtonRenderer::~CButtonRenderer()
{
	try
	{
		if (m_buttonText!=NULL)
		{
			free(m_buttonText);
			m_buttonText=NULL;
		}
	}
	catch(...)
	{
		m_cLog->lprintf("CButtonRenderer error: failed to deconstruct.");
		throw;
	}
}
/*************************************************************************************

  Updates and renders buttons with info on cursor position state of
  mouse button and wether or not it has been selected in any other way.
  If the button has been clicke this function returns true.

*************************************************************************************/
bool CButtonRenderer::UpdateAndRender(int cursorX, int cursorY, bool selected, bool mouseButton)
{
	bool clicked = false;
	bool alphaOn=false;
	glEnable(GL_TEXTURE_2D);
	{
		switch (m_state)
		{
		case BUTTONRENDERER_STATE_DEFAULT:
			if (!selected)
			{
				alphaOn=m_defaultTexIsAlpha;
				m_cTexMan->SelectTexture(m_defaultTex);
			}
			break;
		case BUTTONRENDERER_STATE_MOUSEOVER:
			if (m_mouseOverTex!=-1 && !selected)
			{
				alphaOn=m_mouseOverTexIsAlpha;
				m_cTexMan->SelectTexture(m_mouseOverTex);
			}
			break;
		case BUTTONRENDERER_STATE_MOUSEDOWN:
			if (m_mouseDownTex!=-1 && !selected)
			{
				alphaOn=m_mouseDownTexIsAlpha;
				m_cTexMan->SelectTexture(m_mouseDownTex);
			}
			if (m_state!=m_oldState)
				clicked=true;
			break;
		default:
			break;
		}
		if (selected && m_mouseOverTex!=-1 &&(!m_toggle))
		{
			m_cTexMan->SelectTexture(m_mouseOverTex);
			alphaOn=m_mouseOverTexIsAlpha;
		}
		else if (selected && m_mouseDownTex!=-1 && m_toggle)
		{
			m_cTexMan->SelectTexture(m_mouseDownTex);
			alphaOn=m_mouseDownTexIsAlpha;
		}
	}
	m_oldState=m_state;
	if (alphaOn)
	{
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	glPushMatrix();
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
	if (alphaOn)
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}
	
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
		glColor3f(m_textColor[0],m_textColor[1],m_textColor[2]);
		glRasterPos2i(m_pX,m_pY+((m_minY>189) ? 1:0));
		PrintString(m_font,m_buttonText);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);

	if (cursorX>=m_minX && cursorX<=m_maxX &&
		cursorY>=m_minY && cursorY<=m_maxY)
		m_state=BUTTONRENDERER_STATE_MOUSEOVER;
	else
		m_state=BUTTONRENDERER_STATE_DEFAULT;
	if (mouseButton && BUTTONRENDERER_STATE_MOUSEOVER==m_state)
	{
		m_state=BUTTONRENDERER_STATE_MOUSEDOWN;
		if (clicked)
			return true;
		else
			return false;
	}
	else if (clicked)
		return true;
	else
		return false;
}
/*************************************************************************************

  Returns current state of button.

  BUTTONRENDERER_STATE_DEFAULT
  BUTTONRENDERER_STATE_MOUSEOVER
  BUTTONRENDERER_STATE_MOUSEDOWN

*************************************************************************************/
int CButtonRenderer::GetCurrentState()
{
	return m_state;
}

/*************************************************************************************

  Moves the button to the set coordinates.

*************************************************************************************/
void CButtonRenderer::MoveTo(int x, int y)
{
	int width = (m_maxX - m_minX + 1),
		height= (m_maxY - m_minY + 1);
	m_minY = y;
	m_minX = x;
	m_maxY = m_minY+(height-1);
	m_maxX = m_minX+(width-1);
	m_pX = (width/2) - (m_textWidth/2) + m_minX;
	m_pY = (height/2) - (m_textHeight/2) + m_minY+(m_textHeight/4+((m_textHeight%4) ? 1 : 0));
}
/*************************************************************************************

  Changes the text on the button to set r g b color.

*************************************************************************************/
void CButtonRenderer::ChangeTextColor(float r, float g, float b)
{
	m_textColor[0] = r;
	m_textColor[1] = g;
	m_textColor[2] = b;
}
/************************************************************************************

  Dimension Calculator

************************************************************************************/
void ButtonRendererDimensionCalculator(int &dLeft, int &dTop, int &dRight, int &dBottom,
									   int left, int bottom, int width, int height)
{
	dBottom = bottom;
	dLeft = left;
	dTop = dBottom+(height-1);
	dRight = dLeft+(width-1);
}