#ifndef __SELECTIONLISTRENDERER_INCLUDED__
#define __SELECTIONLISTRENDERER_INCLUDED__
#include "log.h"
#include "TextInputSystem.h"
#include "StringUtilities.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include "fonts.h"
#define SELECTIONLISTRETURNCOMMAND	 INPUTMAN_TEXT_CODE_ENTER
#define SELECTIONLISTPAGEUPCOMMAND	 INPUTMAN_TEXT_CODE_PAGEUP
#define SELECTIONLISTPAGEDOWNCOMMAND INPUTMAN_TEXT_CODE_PAGEDOWN
#define SELECTIONLISTUPCOMMAND		 INPUTMAN_TEXT_CODE_UP
#define SELECTIONLISTDOWNCOMMAND	 INPUTMAN_TEXT_CODE_DOWN
#define SELECTIONLISTHOMECOMMAND	 INPUTMAN_TEXT_CODE_HOME
#define SELECTIONLISTENDCOMMAND		 INPUTMAN_TEXT_CODE_END

class CSelectionListRenderer
{
public:
	CSelectionListRenderer(int fontListBase, int displayWidth, int displayHeight,
									   int textHeight, int textWidth, int bottom, int left,
									   int padding,  const char *listText, CLog **cLog);
	~CSelectionListRenderer();
	const char *UpdateAndRender(const char nextCharacter, bool drawBackground=true,
								bool drawHighlight=true, bool show=false);
	void MoveTo(int x, int y);
	void SetColor(float backR, float backG, float backB,
				  float textR, float textG, float textB);
	void SetHighlightColor(float backR, float backG, float backB,
				  float textR, float textG, float textB);
	const char *GetListText();
	void GetCoordinates(int &left,int &top, int &right, int &bottom);
	int  GetListLength();
	int  GetDisplayHeight();
	void ChangeDisplayHeight(int lines);
	bool ChangeString(const char *newString);
	void ScrollToBottom();
	void ScrollDown(int lines=1);
	void ScrollUp(int lines=1);
	void ScrollTo(float percent);
	float ScrollPosition();
	bool IsClicked(int cursorX, int cursorY);
	bool IsOver(int cursorX, int cursorY);
protected:
	int m_fontListBase,
		m_displayWidth,
		m_displayHeight,
		m_textHeight,
		m_textWidth,
		m_bottom,
		m_left,
		m_padding,
		m_adjustment,
		m_listLength,
		m_listTxtLen,
		m_dispIndex,
		m_selectedIndex,
		*m_pLineIndexes;
	size_t m_maxSize;
	int m_maxHeight;
	float m_backgroundColor[3],
		  m_textColor[3],
		  m_highLightColor[3],
		  m_backLightColor[3];
	int	  m_minX,m_maxX,
		  m_minY,m_maxY;
	char  *m_pListText,
		  *m_pLineString,
		  *m_pSelectedStr;
	CLog  *m_cLog;
};
void SelectionListRendererDimensionCalculator(int &dLeft, int &dTop, int &dRight, int &dBottom,
									   int left, int bottom, int textWidth, int textHeight,
									   int displayWidth, int displayHeight, int padding);
#endif