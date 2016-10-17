/**********************************************

  Select ButtonList function

  Button Types	|Returns
  ----------------------
  Toggle		|Off/On (changed with each tap if button)
  360Degree		|360.0f-0.0f distance moved / position
  Analog		|1.0f-0.0f distance moved / position
  Button		|Off/On
  Terminate		|Off/On stays on once detected

  *must set: bool stationary

  ----
  checkButton function returns
  #1 float value as desctibed in chart button types chart
  #2 sends back: float secondsHeld // 100.0f means n=>100
  #3 sends back: float secondsInactive // 100.0f means n=>100

***********************************************/

#ifndef __INCLUDED_INPUTMAN__
#define __INCLUDED_INPUTMAN__

#include "HiResTimer.h"
#include "InputSystem.h"
//Button Type Defines
#define INPUTMAN_TYPE_TOGGLE 0
#define INPUTMAN_TYPE_360DEGREE 1
#define INPUTMAN_TYPE_ANALOG 2
#define INPUTMAN_TYPE_BUTTON 3
#define INPUTMAN_TYPE_TERMINATE 4
//Mouse Key Defines
#define INPUTMAN_MOUSE_X -3
#define INPUTMAN_MOUSE_Y -2
#define INPUTMAN_MOUSE_WHEEL_UP -1
#define INPUTMAN_MOUSE_WHEEL_DOWN 0
#define INPUTMAN_MOUSE_BUTTON 1
//Joystick Key Defines
#define INPUTMAN_JOYSTICK_X -2
#define INPUTMAN_JOYSTICK_Y -1
#define INPUTMAN_JOYSTICK_BUTTON 0
//Device Defines
#define INPUTMAN_DEVICE_KEYBOARD 0
#define INPUTMAN_DEVICE_MOUSE 1
//Text Input System Defines
#define INPUTMAN_TEXT_REPEATE_DELAY 0.5f
#define INPUTMAN_TEXT_REPEATE_RATE 0.1f
#define INPUTMAN_TEXT_REPEATE_DEFAULT true

typedef struct
{
	char	*buttonName; // button name ie "Up"
	int		buttonType; // INPUTMAN_TYPE_TOGGLE / 360DEGREE / ANALOG / BUTTON / TERMINATE
	float	secondsHeld; // 0.0f
	float	secondsInactive; // 0.0f
	bool	stationary; // if true use exact position else use distance traveled
	
	bool	toggleStatus; // only for toggle buttons
	int		oldValue; // -1
	int		count; // 0 position counter
	int		travel; // 0 travel counter

	bool	fixedDefine; // true if button cannot be redefined
	int		inputDevice; // INPUTMAN_DEVICE_KEYBOARD / MOUSE
	int		inputKey; // key from device ie "A"
	/**********************************************************
	
	  Max Accepted Value ie joystick/mouse range on selected
	  axis, Set to 1 for keyboard keys and push buttons.
	  When analog button set as not stationary, this value
	  will limit the max amount of movement units (ie pixels)
	  per second the system will report.

	**********************************************************/
	int		maxValue;
} inputManButton_t;

typedef struct
{
	float	buttonValue;
	float	secondsHeld;
	float	secondsInactive;
} inputManButtonStatus_t;

typedef struct
{
	bool	buttonValue;
	bool	oldValue;
	float	secondsHeld;
	float	secondsInactive;
	int		repeateCounter;
} inputManTextButtons_t;

class CInputManager
{
public:
	CInputManager();
	~CInputManager();
	void Initialize(HWND hwnd, HINSTANCE appInstance, bool isExclusive, DWORD flags =0);
	void Update();
	bool CreateButtons(int numberOfbuttons, inputManButton_t *buttonList);
	float ButtonStatus(int indexValue, float *secondsHeld, float *secondsInactive);
	void SetButtonXYCoordinate(int xIndexValue, int yIndexValue, int newX, int newY);
	void SetButtonToggle(int indexValue, bool newStatus);
	void SetTextRepeate(bool status);
	void SetCapsLock(bool status);
	void ResetKey(int indexValue, int newKey, int device);
	void ResetKey(int indexValue, int newKey, int device, bool stationary);
	size_t GetSizeOfButtonStatusData();
	bool GetTextRepeate();
	bool GetCapsLock();
	bool GetShiftStatus();
	int GetKeyboardKey();
	int GetMouseKey();
	int GetListCount();
	int GetButtonIndex(char *buttonName);
	const long double *GetSystemTimePointer();
	const inputManButton_t *GetButtonList(int *listCount);
	const inputManButtonStatus_t *GetStatusPointer();
	const inputManTextButtons_t *GetKeyboardStatusPointer();
	const unsigned char *GetTextInputPointer();
	CInputSystem			m_input;
protected:
	float GenerateStatus(int received, int listIndex);
	long double				m_systemTime;
	unsigned char		    m_textOutput;
	inputManButton_t	   *m_buttonList;
	inputManButtonStatus_t *m_buttonStatus;
	inputManTextButtons_t	m_textButtons[256];
	float					m_elapsedTime;
	int						m_listCount;
	CHiResTimer				m_timer;
	bool					m_initialized;
	bool m_capsLock; // true if capsLock toggled on
	bool m_shifted; // true if shift key down
	bool m_textRepeate; // true if text input is set to repeat
};


/********************************************************************************

  Utility Functions to make using the input system easier and less code intesive.

*********************************************************************************/
#ifndef INPUTMAN_NO_UTILITIES
extern int INPUTMANGenerateStandardButtons (inputManButton_t *buttonList, const int currentIndex, char buttonName[], const int buttonKey,const int buttonType, const int buttonDevice, const bool fixedDefine, const bool stationary, const int maxValue);
extern int INPUTMANGenerateStandardButtons (inputManButton_t *buttonList, const int startIndex, const int count, char *buttonNames[], const int buttonKeys[],const int buttonType, const int buttonDevice, const bool fixedDefine, const bool stationary, const int maxValue);
extern int INPUTMANGenerateStandardButtons (inputManButton_t *buttonList, const int startIndex, const int count, char *buttonNames[], const int buttonKeys[],const int buttonType, const int buttonDevice, const bool fixedDefine, const bool stationary, int maxValue[]);
extern unsigned char INPUTMANConvertTextInputToCharacters(unsigned const char *textInput, bool capsLock, bool shiftKey);
#endif


// SPECIAL KEY DEFINES
#define INPUTMAN_TEXT_CODE_BACKSPACE 8
#define INPUTMAN_TEXT_CODE_TAB		9
#define INPUTMAN_TEXT_CODE_SHIFTTAB	10
#define INPUTMAN_TEXT_CODE_ENTER	13
#define INPUTMAN_TEXT_CODE_ESCAPE	27
#define INPUTMAN_TEXT_CODE_INSERT	1
#define INPUTMAN_TEXT_CODE_HOME		2
#define INPUTMAN_TEXT_CODE_PAGEUP	3
#define INPUTMAN_TEXT_CODE_DELETE	4
#define INPUTMAN_TEXT_CODE_END		5
#define INPUTMAN_TEXT_CODE_PAGEDOWN	6

#define INPUTMAN_TEXT_CODE_UP		24
#define INPUTMAN_TEXT_CODE_DOWN		25
#define INPUTMAN_TEXT_CODE_RIGHT	26
#define INPUTMAN_TEXT_CODE_LEFT		28

#define INPUTMAN_TEXT_CODE_F1		128
#define INPUTMAN_TEXT_CODE_F2		129
#define INPUTMAN_TEXT_CODE_F3		130
#define INPUTMAN_TEXT_CODE_F4		131
#define INPUTMAN_TEXT_CODE_F5		132
#define INPUTMAN_TEXT_CODE_F6		133
#define INPUTMAN_TEXT_CODE_F7		134
#define INPUTMAN_TEXT_CODE_F8		135
#define INPUTMAN_TEXT_CODE_F9		136
#define INPUTMAN_TEXT_CODE_F10		137
#define INPUTMAN_TEXT_CODE_F11		138
#define INPUTMAN_TEXT_CODE_F12		139

#define INPUTMAN_TEXT_CODE_CAPSLOCK		140
#define INPUTMAN_TEXT_CODE_SCROLLLOCK	141
#define INPUTMAN_TEXT_CODE_NUMLOCK		142
#define INPUTMAN_TEXT_CODE_SYSRQ		143
#define INPUTMAN_TEXT_CODE_PAUSE		144


#endif