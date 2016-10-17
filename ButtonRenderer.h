#ifndef __BUTTONRENDERER_INCLUDED__
#define __BUTTONRENDERER_INCLUDED__
#include "log.h"
#include "texture manager.h"
#include "TextInputSystem.h"
#include "StringUtilities.h"
#include "fonts.h"
#define BUTTONRENDERER_STATE_DEFAULT   0
#define BUTTONRENDERER_STATE_MOUSEOVER 1
#define BUTTONRENDERER_STATE_MOUSEDOWN 2
class CButtonRenderer
{
public:
	CButtonRenderer(int top, int left, int width, int height, const char *buttonText,
		int buttonTextWidthPx, int buttonTextHeightPx,
		char *defaultTexName, char *mouseOverTexName, char *mouseDownTexName,
		unsigned int fontListBase, CLog *cLog, CTextureManager *cTexMan, bool toggle=false);
	~CButtonRenderer();
	bool UpdateAndRender(int cursorX, int cursorY, bool selected, bool mouseButton);
	int GetCurrentState();
	void MoveTo(int x, int y);
	void ChangeTextColor(float r, float g, float b);
protected:
	int	m_minX,
		m_maxX,
		m_minY,
		m_maxY,
		m_pX,
		m_pY,
		m_defaultTex,
		m_mouseOverTex,
		m_mouseDownTex,
		m_textWidth,
		m_textHeight,
		m_state,
		m_oldState;
	float m_textColor[3];
	unsigned int m_font;
	bool m_defaultTexIsAlpha,
		 m_mouseOverTexIsAlpha,
		 m_mouseDownTexIsAlpha;
	bool  m_toggle;
	bool  m_activated;
	char *m_buttonText;
	CLog *m_cLog;
	CTextureManager *m_cTexMan;
};

void ButtonRendererDimensionCalculator(int &dLeft, int &dTop, int &dRight, int &dBottom,
									   int left, int bottom, int width, int height);
#endif