#ifndef __TEXTFEILDRENDER_INCLUDED__
#define __TEXTFEILDRENDER_INCLUDED__
#include "log.h"
#include "TextInputSystem.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include "fonts.h"
#define TEXTFEILDRETURNCOMMAND INPUTMAN_TEXT_CODE_ENTER
class CTextFeildRenderer
{
public:
	CTextFeildRenderer(int fontListBase, int displayWidth, int maxChars,
									   int textHeight, int textWidth, int top, int left,
									   int padding, CLog **cLog);
	~CTextFeildRenderer();
	const char *UpdateAndRender(const char nextCharacter, bool drawBackground=true, bool show=false);
	void MoveTo(int x, int y);
	void SetColor(float backR, float backG, float backB,
				  float textR, float textG, float textB);
	void SetTextString(const char *string);
	void GetCoordinates(int &left,int &top, int &right,int &bottom);
	bool IsClicked(int cursorX, int cursorY);
	bool IsOver(int cursorX, int cursorY);
	const char *GetTextString();
protected:
	int m_fontListBase,
		m_displayWidth,
		m_maxChars,
		m_textHeight,
		m_textWidth,
		m_top,
		m_left,
		m_padding,
		m_adjustment;
	float m_backgroundColor[3],
		  m_textColor[3];
	int	  m_minX,m_maxX,
		  m_minY,m_maxY;
	const char *m_pReadText,
			   *m_pDisplayText,
			   *m_pNoCursor;
	char	   *m_pReturnedOutput;
	CTextInputSystem *m_cTextInput;
	CLog			 *m_cLog;
};
void TextFeildRendererDimensionCalculator(int &dLeft, int &dTop, int &dRight, int &dBottom,
									   int left, int bottom, int textWidth, int textHeight,
									   int displayWidth, int padding);
#endif