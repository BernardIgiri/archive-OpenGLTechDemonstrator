#define INPUTMAN_NO_UTILITIES
#include "InputManager.h"
/****************************************************************************************************

  Initializes all the varibles used in the system.

  m_buttonList	 - contains info on the definition of button and status dat used internally
				   by the system.
  
  m_buttonStatus - contains info on the current status of a button for use in input processing.

  m_textButtons	 - contains button status data on all keyboard buttons. (Offers alternate
				   method for button proccessing, however it is mainly used in updating text
				   input, and provides no means of abstraction.)
//////////////////////////////////////////////////////////
m_input (Direct Input Wrapper with members ...)

bool Initialize(HWND hwnd,HINSTANCE appInstance,bool isExclusive, DWORD flags);
// DWORD flags contains options for activating devices
// ie IS_USEKEYBOARD | IS_USEMOUSE currently activates
// usage of the keyboard and mouse
// bool isExclusive is use to communicate the usage
// of the mouse to the mouse Direct Input wrapper

Shutdown() // shuts down input system

bool KeyDown(char) // NOTE: KeyDown(unsinged char) would be
// more reliable since the conversion is necessary
// anyway to get all keys
// after receiving a Direct Input constant (or any constant)
// returns true if the requested button is down
// else returns false.

bool ButtonDown(int) // does the same as KeyDown
// expect for mouse buttons

void GetMouseMovement(int &x, int &y) // gets the
// x and y travel since the last call

int GetMouseWheelMovement() // does the same
// as GetMouseMovement except for the mouse wheel

Update() // updates the input wrapper (called once
// every cycle through input system)

****************************************************************************************************/
CInputManager::CInputManager()
{
	if(m_timer.Init())
	{
		m_systemTime = 0.0;
		m_buttonList=NULL;
		m_buttonStatus=NULL;
		m_listCount=0;
		m_initialized=false;
		m_capsLock=false;
		m_shifted=false;
		m_textRepeate=INPUTMAN_TEXT_REPEATE_DEFAULT;
		for(int i =0; i<256; i++)
		{
			m_textButtons[i].buttonValue=false;
			m_textButtons[i].oldValue=false;
			m_textButtons[i].secondsHeld=0.0f;
			m_textButtons[i].secondsInactive=0.0f;
		}
		m_textOutput = NULL;
	}
}
/****************************************************************************************************

  Initializes m_input for usage as the input wrapper for the input manager.

****************************************************************************************************/
void CInputManager::Initialize(HWND hwnd, HINSTANCE appInstance, bool isExclusive, DWORD flags)
{
	m_input.Initialize(hwnd,appInstance,isExclusive,flags);
	m_initialized=true;
}
/****************************************************************************************************

  Frees ram allocated by the system.

****************************************************************************************************/
CInputManager::~CInputManager()
{
	if(m_listCount>0)
	{
		for (int i=0; i<m_listCount; i++)
			free(m_buttonList[i].buttonName);
		delete[] m_buttonList;
		delete[] m_buttonStatus;
	}
	m_input.Shutdown();
}
/****************************************************************************************************

  Adds new buttons to the system.

****************************************************************************************************/
bool CInputManager::CreateButtons(int numberOfButtons, inputManButton_t *buttonList)
{
	int oldListCount = m_listCount;
	m_listCount+=numberOfButtons;
	if (!buttonList || (m_listCount<0) || (oldListCount<0))
		return false;
	size_t listSize = sizeof(inputManButton_t)*m_listCount;
	if (!m_buttonList)
	{
		m_buttonList = new inputManButton_t[m_listCount];
		m_buttonStatus = new inputManButtonStatus_t[m_listCount];
	}
	else
	{
		inputManButton_t *newList = (inputManButton_t *)realloc(m_buttonList,listSize);
		inputManButtonStatus_t *newStatus = (inputManButtonStatus_t *)realloc(m_buttonStatus,sizeof(inputManButtonStatus_t)*m_listCount);
		if (newList==NULL || newStatus==NULL)
			return false;
		m_buttonList = newList;
		m_buttonStatus = newStatus;
	}
	memcpy(m_buttonList + (sizeof(inputManButton_t)*oldListCount),buttonList,listSize-(sizeof(inputManButton_t)*oldListCount));
	for (int i=oldListCount, n=0; i<m_listCount; i++, n++)
	{
		m_buttonList[i].buttonName = (char*)malloc((size_t)(strlen(buttonList[n].buttonName)+1));
		if (m_buttonList[i].buttonName==NULL)
			return false;
		strcpy(m_buttonList[i].buttonName,buttonList[n].buttonName);
	}
	return true;
}
/****************************************************************************************************

  Called only by the ButtonStatus member function, this member converts received input into
  the final value that will be returned as the button's current status.  (limited to -1.0f to 1.0f)
  This function is usually called n times per cycle where n is the number of defined buttons.

****************************************************************************************************/
float CInputManager::GenerateStatus(int received, int listIndex)
{
	switch (m_buttonList[listIndex].buttonType)
	{ // Determins output according to button type and input received
	case INPUTMAN_TYPE_TOGGLE:
		if (m_buttonList[listIndex].secondsHeld!=0.0f && m_buttonList[listIndex].oldValue==0)
		{	// if down and was not down the last time, then toggle
			m_buttonList[listIndex].toggleStatus = (!m_buttonList[listIndex].toggleStatus);
			m_buttonList[listIndex].oldValue = 1; // was pressed
		}
		m_buttonList[listIndex].oldValue=received;
		if (m_buttonList[listIndex].toggleStatus)
			return 1.0f;
		else
			return 0.0f;
		break;

	case INPUTMAN_TYPE_360DEGREE: case INPUTMAN_TYPE_ANALOG:
		{
		float scalar = 360.0f;
		if (m_buttonList[listIndex].buttonType==INPUTMAN_TYPE_ANALOG)
			scalar = 1.0f;
		if (m_buttonList[listIndex].stationary)
			m_buttonList[listIndex].oldValue = received;
		else
		{
			//int travel = abs(m_buttonList[listIndex].oldValue-received);
			m_buttonList[listIndex].oldValue = received;
			received = int((float)m_buttonList[listIndex].travel/m_elapsedTime); //travel;
			if (received>m_buttonList[listIndex].maxValue)
				received=m_buttonList[listIndex].maxValue;
			if (received<(-m_buttonList[listIndex].maxValue))
				received=(-m_buttonList[listIndex].maxValue);
		}
		if (received==0)
			return 0.0f;
		else
			return ((float)received/(float)m_buttonList[listIndex].maxValue)*scalar;
		break;
		}

	case INPUTMAN_TYPE_BUTTON:
		return (received>0 ? 1.0f : 0.0f);
		break;

	case INPUTMAN_TYPE_TERMINATE: // terminate buttons stay after detection until manually turned off
		if ((m_buttonList[listIndex].oldValue>0)||(received>0)) // if received or oldvalue then true
		{
			m_buttonList[listIndex].oldValue=1;
			return 1.0f;
		}
		return 0.0f;
		break;

	default:
		break;
	}
	return -1.0f;
}
/****************************************************************************************************

  Returns the current status of the button (1.0f && -1.0f means at max limit,
  0.0f means neutral/off). Should not be used to get current status.  Instead
  Update should be called.  Update calls this member once for every defined button.

****************************************************************************************************/
float CInputManager::ButtonStatus(int indexValue, float *secondsHeld, float *secondsInactive)
{
	if (!m_initialized)
		return -10;
	float elapsedTime = m_elapsedTime;
	int i = indexValue;
	int received=0;
	bool aButton = false;
	switch(m_buttonList[i].inputDevice)
	{ // processes input and determins return value to be sent to GenerateStatus
	case INPUTMAN_DEVICE_KEYBOARD:
		aButton = true;
		if(m_input.KeyDown(m_buttonList[i].inputKey))
			received = 1;
		else
			received = 0;
		break;

	case INPUTMAN_DEVICE_MOUSE:
		if(m_buttonList[i].inputKey>=INPUTMAN_MOUSE_BUTTON)
		{
			aButton = true;
			received = (int)m_input.ButtonDown(m_buttonList[i].inputKey-INPUTMAN_MOUSE_BUTTON);
		}
		else
		{
			int x, y, wheel;
			m_input.GetMouseMovement(x, y);
			wheel = m_input.GetMouseWheelMovement();
			switch(m_buttonList[i].inputKey)
			{
			case INPUTMAN_MOUSE_X:
				m_buttonList[i].travel=x;
				m_buttonList[i].count+=x;
				if (m_buttonList[i].count<0)
					m_buttonList[i].count=0;
				else if (m_buttonList[i].count>m_buttonList[i].maxValue)
					m_buttonList[i].count=m_buttonList[i].maxValue;
				received=m_buttonList[i].count;
				break;
				
			case INPUTMAN_MOUSE_Y:
				m_buttonList[i].travel=y;
				m_buttonList[i].count+=y;
				if (m_buttonList[i].count<0)
					m_buttonList[i].count=0;
				else if (m_buttonList[i].count>m_buttonList[i].maxValue)
					m_buttonList[i].count=m_buttonList[i].maxValue;
				received=m_buttonList[i].count;
				break;

			case INPUTMAN_MOUSE_WHEEL_UP:
				aButton = true;
				received = (int)(wheel>0);
				break;

			case INPUTMAN_MOUSE_WHEEL_DOWN:
				aButton = true;
				received = (int)(wheel<0);
				break;

			default:
				break;
			}
		}
		break;

	default:
		break;
	}

	if (aButton)
	{ // calculates secondsHeld and secondsInactive
		if(received)
		{
			m_buttonList[i].count=1;
			m_buttonList[i].travel=1;

			m_buttonList[i].secondsHeld+=elapsedTime;
			m_buttonList[i].secondsInactive=0.0f;
		}
		else
		{
			m_buttonList[i].count=0;
			m_buttonList[i].travel=0;

			m_buttonList[i].secondsHeld=0.0f;
			m_buttonList[i].secondsInactive+=elapsedTime;
		}
	}
	else // of device does not just have an off/on value the calculates and
		// sets inactive to the time since last change
		if(received==m_buttonList[i].oldValue)
		{
			m_buttonList[i].secondsHeld=0.0f;
			m_buttonList[i].secondsInactive+=elapsedTime;
		}
		else
		{
			m_buttonList[i].secondsHeld+=elapsedTime;
			m_buttonList[i].secondsInactive=0.0f;
		}
	*secondsHeld = m_buttonList[i].secondsHeld;
	*secondsInactive = m_buttonList[i].secondsInactive;
	if (*secondsHeld>100.0f)
		*secondsHeld=100.0f;
	if (*secondsInactive>100.0f)
		*secondsInactive=100.0f;
	return GenerateStatus(received,i);
}
/****************************************************************************************************

  Calls m_input.Update() and Updates the values within the system to the current status.
  Continue to call to keep system up to date.

****************************************************************************************************/
void CInputManager::Update()
{
	m_input.Update();
	m_elapsedTime = m_timer.GetElapsedSeconds();
	m_systemTime += (long double)m_elapsedTime;
	// UPDATE TEXT OUTPUT
	m_textOutput=NULL;
	for (int i=0; i<256; i++)
	{
		m_textButtons[i].buttonValue=m_input.KeyDown((char)i);
		if (m_textButtons[i].buttonValue)
		{
			m_textButtons[i].secondsHeld+=m_elapsedTime;
			m_textButtons[i].secondsInactive=0.0f;
			if (m_textButtons[i].secondsHeld>100.0f)
				m_textButtons[i].secondsHeld=100.0f;
		}
		else
		{
			m_textButtons[i].repeateCounter=0;
			m_textButtons[i].secondsInactive+=m_elapsedTime;
			m_textButtons[i].secondsHeld=0.0f;
			if (m_textButtons[i].secondsInactive>100.0f)
				m_textButtons[i].secondsInactive=100.0f;
		}
		bool repeateAgain=false;
		if(((m_textButtons[i].secondsHeld-INPUTMAN_TEXT_REPEATE_DELAY)-(m_textButtons[i].repeateCounter*INPUTMAN_TEXT_REPEATE_RATE))>0.0f)
		{
			repeateAgain=true;
			m_textButtons[i].repeateCounter++;
			if (m_textButtons[i].repeateCounter>1000)
			{
				m_textButtons[i].repeateCounter=0;
				repeateAgain=false;
			}
		}
			
		if(m_textButtons[i].buttonValue && m_textButtons[i].oldValue==false || repeateAgain)
		{
			m_textOutput = (unsigned char)i;
			m_textButtons[i].oldValue=true;
			break;
		}
		m_textButtons[i].oldValue = m_textButtons[i].buttonValue;
	}
	if (m_textOutput==(unsigned char)DIK_CAPITAL)
		m_capsLock=(!m_capsLock); // if capsLock is pressed toggle m_capsLock variable
	m_shifted=(m_textButtons[DIK_LSHIFT].buttonValue||m_textButtons[DIK_RSHIFT].buttonValue); // if either shift key is being pressed m_shifted=true
	// UPDATE BUTTONS
	for (i=0; i<m_listCount; i++)
		m_buttonStatus[i].buttonValue = ButtonStatus(i,&m_buttonStatus[i].secondsHeld,&m_buttonStatus[i].secondsInactive);
}
/****************************************************************************************************

  Sets the systems CapsLock status variable.  (Note: does not necessarily represent the actual
  status of CapsLock.)
  
****************************************************************************************************/
void CInputManager::SetCapsLock(bool status)
{
	m_capsLock = status;
}
/****************************************************************************************************

  Sets the text repeat option on or off. (set to false to disable repeating characters)

****************************************************************************************************/
void CInputManager::SetTextRepeate(bool status)
{
	m_textRepeate=status;
}
/****************************************************************************************************

  Returns the status of the caps lock variable. (true == on)

****************************************************************************************************/
bool CInputManager::GetCapsLock()
{
	return m_capsLock;
}
/****************************************************************************************************

  Returns the status of the shift status variable. (true == shift depressed)

****************************************************************************************************/
bool CInputManager::GetShiftStatus()
{
	return m_shifted;
}
/****************************************************************************************************

  Returns the status of the text repeate option (true == on)

****************************************************************************************************/
bool CInputManager::GetTextRepeate()
{
	return m_textRepeate;
}
/****************************************************************************************************

  Sets the counter values of specified buttons to the sent integers.
  The counters are used to count traveled units for use in processing analog input.

  The most likely usage of this is to set the x and y coordinates of a cursor.

****************************************************************************************************/
void CInputManager::SetButtonXYCoordinate(int xIndexValue, int yIndexValue, int newX, int newY)
{
	m_buttonList[xIndexValue].count=newX;
	m_buttonList[yIndexValue].count=newY;
}
/****************************************************************************************************

  Sets the state of toggle and terminate buttons.

****************************************************************************************************/
void CInputManager::SetButtonToggle(int indexValue, bool newStatus)
{
	if (m_buttonList[indexValue].buttonType==INPUTMAN_TYPE_TOGGLE)
		m_buttonList[indexValue].toggleStatus=newStatus;
	if (m_buttonList[indexValue].buttonType==INPUTMAN_TYPE_TERMINATE)
	{
		if (newStatus)
			m_buttonList[indexValue].oldValue=1;
		else
			m_buttonList[indexValue].oldValue=0;
	}
}
/****************************************************************************************************

  Resets the defined values of a button.  Ignored if supplied index represents a button defined
  as a fixed define.

****************************************************************************************************/
void CInputManager::ResetKey(int indexValue, int newKey, int device)
{
	if (m_buttonList[indexValue].fixedDefine)
		return;
	m_buttonList[indexValue].inputKey = newKey;
	m_buttonList[indexValue].inputDevice = device;
}
/****************************************************************************************************

  Resets the defined values of a button.  Ignored if supplied index represents a button defined
  as a fixed define.  Inlcudes the stationary value for analog buttons.

****************************************************************************************************/
void CInputManager::ResetKey(int indexValue, int newKey, int device, bool stationary)
{
	if (m_buttonList[indexValue].fixedDefine)
		return;
	m_buttonList[indexValue].inputKey = newKey;
	m_buttonList[indexValue].inputDevice = device;
	m_buttonList[indexValue].stationary = stationary;
}
/*******************************************************************************************************

  Returns the Size of the button status data stored in system.
  Useful for allocating memory to store a copy of the button status.

*******************************************************************************************************/
size_t CInputManager::GetSizeOfButtonStatusData()
{
	return sizeof(inputManButtonStatus_t)*m_listCount;
}
/*******************************************************************************************************

  Returns -10 if nothing was pressed, else returns the keyboard scancode for the key pressed.

*******************************************************************************************************/
int CInputManager::GetKeyboardKey()
{
	m_input.Update();
	for (int i=0; i<256; i++)
		if (m_input.KeyDown((char)i))
			return i;
	return -10;
}
/*******************************************************************************************************

  Returns -10 if nothing was pressed, else returns the code for the mouse key pressed.

*******************************************************************************************************/
int CInputManager::GetMouseKey()
{
	m_input.Update();
	int wheel = m_input.GetMouseWheelMovement();
	for (int i=0; i<4; i++)
		if (m_input.ButtonDown(i))
			return INPUTMAN_MOUSE_BUTTON+i;
	if (wheel>0)
		return INPUTMAN_MOUSE_WHEEL_UP;
	if (wheel<0)
		return INPUTMAN_MOUSE_WHEEL_DOWN;
	return -10;
}
/*******************************************************************************************************

  Returns the number of buttons defined in the system.

*******************************************************************************************************/
int CInputManager::GetListCount()
{
	return m_listCount;
}
/*******************************************************************************************************

  Returns the index value of the button that matches the provided button name.

*******************************************************************************************************/
int CInputManager::GetButtonIndex(char *buttonName)
{
	for (int i=0; i<m_listCount; i++)
		if(strcmp(buttonName,m_buttonList[i].buttonName)==NULL)
			return i;
	return -11;
}
/*******************************************************************************************************

  Returns a const pointer to the button status array.

*******************************************************************************************************/
const inputManButtonStatus_t *CInputManager::GetStatusPointer()
{
	return m_buttonStatus;
}
/*******************************************************************************************************

  Returns a pointer to the buttonList array which contains the definition data for
  the buttons within the system.

*******************************************************************************************************/
const inputManButton_t *CInputManager::GetButtonList(int *listCount)
{
	*listCount = m_listCount;
	return m_buttonList;
}
/*******************************************************************************************************

  Returns a pointer to the textButtons array which contains the button status of each key board key.

*******************************************************************************************************/
const inputManTextButtons_t *CInputManager::GetKeyboardStatusPointer()
{
	return m_textButtons;
}
/*******************************************************************************************************

  Returns a pointer to the text output variable which contains the last pressd key board key.
  This data is a Direct Input keyboard scan code and must be converted for use in text systems.

*******************************************************************************************************/
const unsigned char *CInputManager::GetTextInputPointer()
{
	return &m_textOutput;
}
/*******************************************************************************************************

  Returns a pointer to the system time variable. (The elapsed time is added to it with every call
  top Update).  NOTE: It works, but is not a good way of handling system time,

*******************************************************************************************************/
const long double *CInputManager::GetSystemTimePointer()
{
	return &m_systemTime;
}

/********************************************************************************
*********************************************************************************

  Utility Functions to make using the input system easier and less code intesive.

*********************************************************************************
********************************************************************************/

/********************************************************************************

  Generates a list of Standard Buttons And Returns the index of the next element
  in the button list.

  NOTE: This function doesn't copy the button names so, it is important that
		the source remains unchanged until the array is passed to the create
		buttons function.

********************************************************************************/
int INPUTMANGenerateStandardButtons (inputManButton_t *buttonList, const int currentIndex, char buttonName[], const int buttonKey,const int buttonType, const int buttonDevice, const bool fixedDefine, const bool stationary, const int maxValue)
{
	int i=currentIndex;
	buttonList[i].buttonName = buttonName;
	buttonList[i].buttonType = buttonType;
	buttonList[i].fixedDefine = fixedDefine;
	buttonList[i].inputDevice = buttonDevice;
	buttonList[i].inputKey = buttonKey;
	buttonList[i].maxValue = maxValue;
	buttonList[i].oldValue = -1;
	buttonList[i].count=0;
	buttonList[i].travel=0;
	buttonList[i].secondsHeld = 0.0f;
	buttonList[i].secondsInactive = 0.0f;
	buttonList[i].stationary = stationary;
	buttonList[i].toggleStatus = false;
	return ++i;
}
int INPUTMANGenerateStandardButtons (inputManButton_t *buttonList, const int startIndex, const int count, char *buttonNames[], const int buttonKeys[],const int buttonType, const int buttonDevice, const bool fixedDefine, const bool stationary, const int maxValue)
{
	for (int i=startIndex, readIndex=0; i<(startIndex+count); i++, readIndex++)
	{
		buttonList[i].buttonName = buttonNames[readIndex];
		buttonList[i].buttonType = buttonType;
		buttonList[i].fixedDefine = fixedDefine;
		buttonList[i].inputDevice = buttonDevice;
		buttonList[i].inputKey = buttonKeys[readIndex];
		buttonList[i].maxValue = maxValue;
		buttonList[i].oldValue = -1;
		buttonList[i].count=0;
		buttonList[i].travel=0;
		buttonList[i].secondsHeld = 0.0f;
		buttonList[i].secondsInactive = 0.0f;
		buttonList[i].stationary = stationary;
		buttonList[i].toggleStatus = false;
	}
	return i;
}
int INPUTMANGenerateStandardButtons (inputManButton_t *buttonList, const int startIndex, const int count, char *buttonNames[], const int buttonKeys[],const int buttonType, const int buttonDevice, const bool fixedDefine, const bool stationary, int maxValue[])
{
	for (int i=startIndex, readIndex=0; i<(startIndex+count); i++, readIndex++)
	{
		buttonList[i].buttonName = buttonNames[readIndex];
		buttonList[i].buttonType = buttonType;
		buttonList[i].fixedDefine = fixedDefine;
		buttonList[i].inputDevice = buttonDevice;
		buttonList[i].inputKey = buttonKeys[readIndex];
		buttonList[i].maxValue = maxValue[readIndex];
		buttonList[i].oldValue = -1;
		buttonList[i].count=0;
		buttonList[i].travel=0;
		buttonList[i].secondsHeld = 0.0f;
		buttonList[i].secondsInactive = 0.0f;
		buttonList[i].stationary = stationary;
		buttonList[i].toggleStatus = false;
	}
	return i;
}

/*******************************************************************************************************

  Converts the Direct Input Key board scan code to a character for use in text systems.
  Uses the capsLock and shift status to map key codes to the appropiate characters.

*******************************************************************************************************/
unsigned char INPUTMANConvertTextInputToCharacters(unsigned const char *textInput, bool capsLock, bool shiftKey)
{
	static unsigned char dinputToAsciiMap[256][2]={{0}};
	static bool firstTime = true;
	unsigned char charCopy = (*textInput);
	unsigned char readChar = NULL;
	if (firstTime)
	{
		firstTime=false;
		/// CHARECTERS
		// formatting charecters
		dinputToAsciiMap[DIK_BACK][0]=8;	dinputToAsciiMap[DIK_BACK][1]=8;
		dinputToAsciiMap[DIK_SPACE][0]=' ';	dinputToAsciiMap[DIK_SPACE][1]=' ';
		dinputToAsciiMap[DIK_TAB][0]=9;		dinputToAsciiMap[DIK_TAB][1]=10;
		dinputToAsciiMap[DIK_RETURN][0]=13;	dinputToAsciiMap[DIK_RETURN][1]=13;

		// alphabet
		dinputToAsciiMap[DIK_A][0]='a';		dinputToAsciiMap[DIK_A][1]='A';
		dinputToAsciiMap[DIK_B][0]='b';		dinputToAsciiMap[DIK_B][1]='B';
		dinputToAsciiMap[DIK_C][0]='c';		dinputToAsciiMap[DIK_C][1]='C';
		dinputToAsciiMap[DIK_D][0]='d';		dinputToAsciiMap[DIK_D][1]='D';
		dinputToAsciiMap[DIK_E][0]='e';		dinputToAsciiMap[DIK_E][1]='E';
		dinputToAsciiMap[DIK_F][0]='f';		dinputToAsciiMap[DIK_F][1]='F';
		dinputToAsciiMap[DIK_G][0]='g';		dinputToAsciiMap[DIK_G][1]='G';
		dinputToAsciiMap[DIK_H][0]='h';		dinputToAsciiMap[DIK_H][1]='H';
		dinputToAsciiMap[DIK_I][0]='i';		dinputToAsciiMap[DIK_I][1]='I';
		dinputToAsciiMap[DIK_J][0]='j';		dinputToAsciiMap[DIK_J][1]='J';
		dinputToAsciiMap[DIK_K][0]='k';		dinputToAsciiMap[DIK_K][1]='K';
		dinputToAsciiMap[DIK_L][0]='l';		dinputToAsciiMap[DIK_L][1]='L';
		dinputToAsciiMap[DIK_M][0]='m';		dinputToAsciiMap[DIK_M][1]='M';
		dinputToAsciiMap[DIK_N][0]='n';		dinputToAsciiMap[DIK_N][1]='N';
		dinputToAsciiMap[DIK_O][0]='o';		dinputToAsciiMap[DIK_O][1]='O';
		dinputToAsciiMap[DIK_P][0]='p';		dinputToAsciiMap[DIK_P][1]='P';
		dinputToAsciiMap[DIK_Q][0]='q';		dinputToAsciiMap[DIK_Q][1]='Q';
		dinputToAsciiMap[DIK_R][0]='r';		dinputToAsciiMap[DIK_R][1]='R';
		dinputToAsciiMap[DIK_S][0]='s';		dinputToAsciiMap[DIK_S][1]='S';
		dinputToAsciiMap[DIK_T][0]='t';		dinputToAsciiMap[DIK_T][1]='T';
		dinputToAsciiMap[DIK_U][0]='u';		dinputToAsciiMap[DIK_U][1]='U';
		dinputToAsciiMap[DIK_V][0]='v';		dinputToAsciiMap[DIK_V][1]='V';
		dinputToAsciiMap[DIK_W][0]='w';		dinputToAsciiMap[DIK_W][1]='W';
		dinputToAsciiMap[DIK_X][0]='x';		dinputToAsciiMap[DIK_X][1]='X';
		dinputToAsciiMap[DIK_Y][0]='y';		dinputToAsciiMap[DIK_Y][1]='Y';
		dinputToAsciiMap[DIK_Z][0]='z';		dinputToAsciiMap[DIK_Z][1]='Z';

		//numbers
		dinputToAsciiMap[DIK_0][0]='0';		dinputToAsciiMap[DIK_0][1]=')';
		dinputToAsciiMap[DIK_1][0]='1';		dinputToAsciiMap[DIK_1][1]='!';
		dinputToAsciiMap[DIK_2][0]='2';		dinputToAsciiMap[DIK_2][1]='@';
		dinputToAsciiMap[DIK_3][0]='3';		dinputToAsciiMap[DIK_3][1]='#';
		dinputToAsciiMap[DIK_4][0]='4';		dinputToAsciiMap[DIK_4][1]='$';
		dinputToAsciiMap[DIK_5][0]='5';		dinputToAsciiMap[DIK_5][1]='%';
		dinputToAsciiMap[DIK_6][0]='6';		dinputToAsciiMap[DIK_6][1]='^';
		dinputToAsciiMap[DIK_7][0]='7';		dinputToAsciiMap[DIK_7][1]='&';
		dinputToAsciiMap[DIK_8][0]='8';		dinputToAsciiMap[DIK_8][1]='*';
		dinputToAsciiMap[DIK_9][0]='9';		dinputToAsciiMap[DIK_9][1]='(';
		dinputToAsciiMap[DIK_NUMPAD0][0]='0';		dinputToAsciiMap[DIK_NUMPAD0][1]=INPUTMAN_TEXT_CODE_INSERT;
		dinputToAsciiMap[DIK_NUMPAD1][0]='1';		dinputToAsciiMap[DIK_NUMPAD1][1]=INPUTMAN_TEXT_CODE_END;
		dinputToAsciiMap[DIK_NUMPAD2][0]='2';		dinputToAsciiMap[DIK_NUMPAD2][1]=INPUTMAN_TEXT_CODE_DOWN;
		dinputToAsciiMap[DIK_NUMPAD3][0]='3';		dinputToAsciiMap[DIK_NUMPAD3][1]=INPUTMAN_TEXT_CODE_PAGEDOWN;
		dinputToAsciiMap[DIK_NUMPAD4][0]='4';		dinputToAsciiMap[DIK_NUMPAD4][1]=INPUTMAN_TEXT_CODE_LEFT;
		dinputToAsciiMap[DIK_NUMPAD5][0]='5';		dinputToAsciiMap[DIK_NUMPAD5][1]='5';
		dinputToAsciiMap[DIK_NUMPAD6][0]='6';		dinputToAsciiMap[DIK_NUMPAD6][1]=INPUTMAN_TEXT_CODE_RIGHT;
		dinputToAsciiMap[DIK_NUMPAD7][0]='7';		dinputToAsciiMap[DIK_NUMPAD7][1]=INPUTMAN_TEXT_CODE_HOME;
		dinputToAsciiMap[DIK_NUMPAD8][0]='8';		dinputToAsciiMap[DIK_NUMPAD8][1]=INPUTMAN_TEXT_CODE_UP;
		dinputToAsciiMap[DIK_NUMPAD9][0]='9';		dinputToAsciiMap[DIK_NUMPAD9][1]=INPUTMAN_TEXT_CODE_PAGEUP;

		//symbols
		dinputToAsciiMap[DIK_GRAVE][0]='`';		dinputToAsciiMap[DIK_GRAVE][1]='~';
		dinputToAsciiMap[DIK_MINUS][0]='-';		dinputToAsciiMap[DIK_MINUS][1]='_';
		dinputToAsciiMap[DIK_EQUALS][0]='=';	dinputToAsciiMap[DIK_EQUALS][1]='+';
		dinputToAsciiMap[DIK_LBRACKET][0]='[';	dinputToAsciiMap[DIK_LBRACKET][1]='{';
		dinputToAsciiMap[DIK_RBRACKET][0]=']';	dinputToAsciiMap[DIK_RBRACKET][1]='}';
		dinputToAsciiMap[DIK_BACKSLASH][0]='\\';dinputToAsciiMap[DIK_BACKSLASH][1]='|';
		dinputToAsciiMap[DIK_SEMICOLON][0]=';';	dinputToAsciiMap[DIK_SEMICOLON][1]=':';
		dinputToAsciiMap[DIK_APOSTROPHE][0]='\'';dinputToAsciiMap[DIK_APOSTROPHE][1]='"';
		dinputToAsciiMap[DIK_COMMA][0]=',';		dinputToAsciiMap[DIK_COMMA][1]='<';
		dinputToAsciiMap[DIK_PERIOD][0]='.';	dinputToAsciiMap[DIK_PERIOD][1]='>';
		dinputToAsciiMap[DIK_SLASH][0]='/';		dinputToAsciiMap[DIK_SLASH][1]='?';

		dinputToAsciiMap[DIK_DIVIDE][0]='/';	dinputToAsciiMap[DIK_DIVIDE][1]='/';
		dinputToAsciiMap[DIK_MULTIPLY][0]='*';	dinputToAsciiMap[DIK_MULTIPLY][1]='*';
		dinputToAsciiMap[DIK_SUBTRACT][0]='-';	dinputToAsciiMap[DIK_SUBTRACT][1]='-';
		dinputToAsciiMap[DIK_ADD][0]='+';		dinputToAsciiMap[DIK_ADD][1]='+';
		dinputToAsciiMap[DIK_DECIMAL][0]='.';	dinputToAsciiMap[DIK_DECIMAL][1]=INPUTMAN_TEXT_CODE_DELETE;
		dinputToAsciiMap[DIK_NUMPADENTER][0]=13;dinputToAsciiMap[DIK_NUMPADENTER][1]=13;

		/// COMMANDS
		dinputToAsciiMap[DIK_INSERT][0]=INPUTMAN_TEXT_CODE_INSERT;	dinputToAsciiMap[DIK_INSERT][1]=INPUTMAN_TEXT_CODE_INSERT;
		dinputToAsciiMap[DIK_HOME][0]=INPUTMAN_TEXT_CODE_HOME;		dinputToAsciiMap[DIK_HOME][1]=INPUTMAN_TEXT_CODE_HOME;
		dinputToAsciiMap[DIK_PRIOR][0]=INPUTMAN_TEXT_CODE_PAGEUP;	dinputToAsciiMap[DIK_PRIOR][1]=INPUTMAN_TEXT_CODE_PAGEUP;
		dinputToAsciiMap[DIK_DELETE][0]=INPUTMAN_TEXT_CODE_DELETE;	dinputToAsciiMap[DIK_DELETE][1]=INPUTMAN_TEXT_CODE_DELETE;
		dinputToAsciiMap[DIK_END][0]=INPUTMAN_TEXT_CODE_END;		dinputToAsciiMap[DIK_END][1]=INPUTMAN_TEXT_CODE_END;
		dinputToAsciiMap[DIK_NEXT][0]=INPUTMAN_TEXT_CODE_PAGEDOWN;	dinputToAsciiMap[DIK_NEXT][1]=INPUTMAN_TEXT_CODE_PAGEDOWN;
		dinputToAsciiMap[DIK_UP][0]=INPUTMAN_TEXT_CODE_UP;			dinputToAsciiMap[DIK_UP][1]=INPUTMAN_TEXT_CODE_UP;
		dinputToAsciiMap[DIK_DOWN][0]=INPUTMAN_TEXT_CODE_DOWN;		dinputToAsciiMap[DIK_DOWN][1]=INPUTMAN_TEXT_CODE_DOWN;
		dinputToAsciiMap[DIK_RIGHT][0]=INPUTMAN_TEXT_CODE_RIGHT;	dinputToAsciiMap[DIK_RIGHT][1]=INPUTMAN_TEXT_CODE_RIGHT;
		dinputToAsciiMap[DIK_LEFT][0]=INPUTMAN_TEXT_CODE_LEFT;		dinputToAsciiMap[DIK_LEFT][1]=INPUTMAN_TEXT_CODE_LEFT;
		dinputToAsciiMap[DIK_ESCAPE][0]=INPUTMAN_TEXT_CODE_ESCAPE;	dinputToAsciiMap[DIK_ESCAPE][1]=INPUTMAN_TEXT_CODE_ESCAPE;
		// LOCKS ANS SYSTEM COMMANDS
		dinputToAsciiMap[DIK_CAPSLOCK][0]=INPUTMAN_TEXT_CODE_CAPSLOCK;	dinputToAsciiMap[DIK_CAPSLOCK][1]=INPUTMAN_TEXT_CODE_CAPSLOCK;
		dinputToAsciiMap[DIK_SCROLL][0]=INPUTMAN_TEXT_CODE_SCROLLLOCK;	dinputToAsciiMap[DIK_SCROLL][1]=INPUTMAN_TEXT_CODE_SCROLLLOCK;
		dinputToAsciiMap[DIK_NUMLOCK][0]=INPUTMAN_TEXT_CODE_NUMLOCK;	dinputToAsciiMap[DIK_NUMLOCK][1]=INPUTMAN_TEXT_CODE_NUMLOCK;
		dinputToAsciiMap[DIK_SYSRQ][0]=INPUTMAN_TEXT_CODE_SYSRQ;		dinputToAsciiMap[DIK_SYSRQ][1]=INPUTMAN_TEXT_CODE_SYSRQ;
		dinputToAsciiMap[DIK_PAUSE][0]=INPUTMAN_TEXT_CODE_PAUSE;		dinputToAsciiMap[DIK_PAUSE][1]=INPUTMAN_TEXT_CODE_PAUSE;
		// FUNCTION KEYS
		dinputToAsciiMap[DIK_F1][0]=INPUTMAN_TEXT_CODE_F1;		dinputToAsciiMap[DIK_F1][1]=INPUTMAN_TEXT_CODE_F1;
		dinputToAsciiMap[DIK_F2][0]=INPUTMAN_TEXT_CODE_F2;		dinputToAsciiMap[DIK_F2][1]=INPUTMAN_TEXT_CODE_F2;
		dinputToAsciiMap[DIK_F3][0]=INPUTMAN_TEXT_CODE_F3;		dinputToAsciiMap[DIK_F3][1]=INPUTMAN_TEXT_CODE_F3;
		dinputToAsciiMap[DIK_F4][0]=INPUTMAN_TEXT_CODE_F4;		dinputToAsciiMap[DIK_F4][1]=INPUTMAN_TEXT_CODE_F4;
		dinputToAsciiMap[DIK_F5][0]=INPUTMAN_TEXT_CODE_F5;		dinputToAsciiMap[DIK_F5][1]=INPUTMAN_TEXT_CODE_F5;
		dinputToAsciiMap[DIK_F6][0]=INPUTMAN_TEXT_CODE_F6;		dinputToAsciiMap[DIK_F6][1]=INPUTMAN_TEXT_CODE_F6;
		dinputToAsciiMap[DIK_F7][0]=INPUTMAN_TEXT_CODE_F7;		dinputToAsciiMap[DIK_F7][1]=INPUTMAN_TEXT_CODE_F7;
		dinputToAsciiMap[DIK_F8][0]=INPUTMAN_TEXT_CODE_F8;		dinputToAsciiMap[DIK_F8][1]=INPUTMAN_TEXT_CODE_F8;
		dinputToAsciiMap[DIK_F9][0]=INPUTMAN_TEXT_CODE_F9;		dinputToAsciiMap[DIK_F9][1]=INPUTMAN_TEXT_CODE_F9;
		dinputToAsciiMap[DIK_F10][0]=INPUTMAN_TEXT_CODE_F10;	dinputToAsciiMap[DIK_F10][1]=INPUTMAN_TEXT_CODE_F10;
		dinputToAsciiMap[DIK_F11][0]=INPUTMAN_TEXT_CODE_F11;	dinputToAsciiMap[DIK_F11][1]=INPUTMAN_TEXT_CODE_F11;
		dinputToAsciiMap[DIK_F12][0]=INPUTMAN_TEXT_CODE_F12;	dinputToAsciiMap[DIK_F12][1]=INPUTMAN_TEXT_CODE_F12;

	}
	readChar=dinputToAsciiMap[charCopy][0];
	int version=0;
	if ((capsLock||shiftKey) &&(readChar<='z'&&readChar>='a') && (!(capsLock&&shiftKey)))
		version=1;
	if ((!(readChar<='z'&&readChar>='a'))&&shiftKey)
		version=1;
	readChar=dinputToAsciiMap[charCopy][version];
	return readChar;
}