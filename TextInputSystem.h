#ifndef __INCLUDED_TEXTINPUTSYSTEM__
#define __INCLUDED_TEXTINPUTSYSTEM__

#include "StringUtilities.h"
#include "InputManager.h"
#include "SystemTimer.h"

#define TEXTINPUTSYSTEM_CURSOR_LEFT		 INPUTMAN_TEXT_CODE_LEFT
#define TEXTINPUTSYSTEM_CURSOR_RIGHT	 INPUTMAN_TEXT_CODE_RIGHT
#define TEXTINPUTSYSTEM_CURSOR_DELETE	 INPUTMAN_TEXT_CODE_DELETE
#define TEXTINPUTSYSTEM_CURSOR_BACKSPACE INPUTMAN_TEXT_CODE_BACKSPACE
#define TEXTINPUTSYSTEM_CURSOR_INSERT	 INPUTMAN_TEXT_CODE_INSERT
#define TEXTINPUTSYSTEM_CURSOR_END		 INPUTMAN_TEXT_CODE_END
#define TEXTINPUTSYSTEM_CURSOR_HOME		 INPUTMAN_TEXT_CODE_HOME

#define TEXTINPUTSYSTEM_CURSOR_BLINK_RATE 0.5

#define TEXTINPUTSYSTEM_CURSOR_SPACER 111

class CTextInputSystem
{
public:
	CTextInputSystem(int maxCharacterInput = 200, int displayWidth = 60, bool overWriteOn = false);
	~CTextInputSystem();
	void ReadText(const char nextCharacter);
	void ScrollTo(int index);
	bool SetDisplayWidth(int displayWidth);
	bool SetMaxCharacterInput(int maxCharacterInput);
	void SetOverWriteStatus(bool overWriteOn);
	void ResetSystem();
	void ResetSystem(const char *startString);
	bool CheckSystemValidity();
	bool GetOverWriteStatus();
	int GetDisplayWidth();
	int GetMaxCharacterInput();
	int GetDisplayedIndex();
	int GetCursorPosition(bool isRelative);
	const char *GetOutput();
	const char *GetDisplayString();
	const char *GetCursorString();
protected:
	bool	m_isOverWriteOn;
	int		m_maxCharacterInput;
	int		m_displayWidth;
	int		m_displayedIndex;
	int		m_cursorPosition;
	int		m_displayCursor;
	char*	m_pOutput;
	char*	m_pDisplay;
	char*	m_pCursor;
	bool m_cursorOn;

	//CHiResTimer m_timer;
	CSystemTimer m_timer;
	float		m_elapsedTime;
	float		m_timeCount;
};

#endif