#ifndef __CONSOL_INCLUDED__
#define __CONSOL_INCLUDED__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include "fonts.h"
#include "StringUtilities.h"
#include "TextInputSystem.h"

#define CONSOLDISPLAYWIDTH 100
#define CONSOLMAXCHARACTERINPUT 199
#define CONSOLENTERCOMMANDCHAR INPUTMAN_TEXT_CODE_ENTER

#define CONSOLCOMMANDCLEARALLCHAR INPUTMAN_TEXT_CODE_F1
#define CONSOLCOMMANDGETLASTSTRINGCHAR INPUTMAN_TEXT_CODE_F2
#define CONSOLCOMMANDDROPDOWNCHAR INPUTMAN_TEXT_CODE_F3

class CConsol
{
public:
	CConsol(HDC hdc, int fontListBase, int numberOfLines = 42,
			int displayedLines = 2, float textHeight = 0.02f,
			float leftX = -1.0f, float topY = 1.0f,
			float rightX = 1.0f, float bottomY = 0.5f,
			float zPos = -1.0f,
			float startRasX = -0.65f, float startRasY = 0.47f,
			int maxCharacterDisplay=CONSOLDISPLAYWIDTH,
			int maxCharacterInput=CONSOLMAXCHARACTERINPUT);
	~CConsol();
	const char *UpdateAndRender(const unsigned char nextCharacter, bool renderAll=false);
	bool EchoMessage(const char*msg, bool render = true);
	bool EchoFMessage(const char*msg,bool render = true);
	bool EchoPrintF(bool render,const char*format,...);
	int GetDisplayLines();
	int GetMaxLines();
	void SetDisplayLines(int lines);
	void SetTextString(const char *string);
	void SetTextStringPF(const char *format,...);
	void SendThru();
	void TurnRenderingOn();
	void TurnRenderingOff();
	bool GetRenderingState();
private:
	HDC	  m_hdc;
	bool  m_echoState,
		  m_sendThru,
		  m_rendering;
	float m_minX,
		  m_minY,
		  m_maxX,
		  m_maxY,
		  m_textHeight,
		  m_zPos,
		  m_rasX,
		  m_rasY;
	int	  m_fontListBase,
		  m_numberOfLines,
		  m_maxCharacterDisplay,
		  m_displayedLines,
		  m_dispScroll;
	const char *m_pReadText,
			   *m_pDisplayText,
			   *m_pNoCursor;
	char	  **m_ppDisplayTextList,
			   *m_pReturnedOutput;
	CStringList m_cConsolText;
	CTextInputSystem *m_cTextInput;
};
#endif