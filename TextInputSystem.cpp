#include "TextInputSystem.h"

/*************************************************************************

  Initializes the Text Input System which can be used for processing
  text input for display.

*************************************************************************/
CTextInputSystem::CTextInputSystem(int maxCharacterInput, int displayWidth, bool overWriteOn)
{
	m_maxCharacterInput = maxCharacterInput;
	m_displayWidth = displayWidth;
	m_isOverWriteOn = overWriteOn;
	
	m_displayedIndex = 0;
	m_cursorPosition = 0;
	m_displayCursor = 0;

	m_pOutput =(char *)malloc(maxCharacterInput+1);
	m_pDisplay =(char *)malloc(displayWidth+1);
	m_pCursor =(char *)malloc(displayWidth+2);
	memset(m_pCursor,TEXTINPUTSYSTEM_CURSOR_SPACER,displayWidth+1);
	memset(m_pOutput,NULL,maxCharacterInput+1);
	m_pCursor[displayWidth+1]=NULL;
	m_cursorOn = true;
	//if (m_timer.Initialize(systemClock))
	{
		m_elapsedTime=0.0f;
		m_timeCount=0.0f;
	}
	//else
		m_elapsedTime=-1.0f;
}
/*************************************************************************

  Frees Alocated System Memory.

*************************************************************************/
CTextInputSystem::~CTextInputSystem()
{
	if (m_pOutput!=NULL)
		free(m_pOutput);
	if (m_pDisplay!=NULL)
		free(m_pDisplay);
	if (m_pCursor!=NULL)
		free(m_pCursor);
}
/*************************************************************************

  Receives text input one character at a time and processes it updating
  the entire system.

*************************************************************************/
void CTextInputSystem::ReadText(const char nextCharacter)
{
	//m_pCursor[m_displayCursor]=TEXTINPUTSYSTEM_CURSOR_SPACER;
	if (m_pOutput==NULL || m_pDisplay==NULL || m_pCursor==NULL)
		return;
	int stringLength = strlen(m_pOutput);
	if (nextCharacter>=32 && nextCharacter<=126 && (m_cursorPosition<m_maxCharacterInput))
	{
		if (m_isOverWriteOn)
		{
			m_pOutput[m_cursorPosition]=nextCharacter;
			if (m_cursorPosition==stringLength)
				m_pOutput[m_cursorPosition+1]=NULL;
		}
		else
			StrUtil_InsertCharacter(m_pOutput,nextCharacter,m_cursorPosition);
		m_cursorPosition++;
		stringLength=strlen(m_pOutput);
	}
	switch (nextCharacter)
	{
	case TEXTINPUTSYSTEM_CURSOR_LEFT:
		m_cursorPosition--;
		break;
	case TEXTINPUTSYSTEM_CURSOR_RIGHT:
		m_cursorPosition++;
		break;
	case TEXTINPUTSYSTEM_CURSOR_DELETE:
		StrUtil_DeleteCharacter(m_pOutput,m_cursorPosition);
		break;
	case TEXTINPUTSYSTEM_CURSOR_BACKSPACE:
		m_cursorPosition--;
		if (m_cursorPosition<0)
			m_cursorPosition=0;
		else
			StrUtil_DeleteCharacter(m_pOutput,m_cursorPosition);
		break;
	case TEXTINPUTSYSTEM_CURSOR_INSERT:
		m_isOverWriteOn=(!m_isOverWriteOn);
		break;
	case TEXTINPUTSYSTEM_CURSOR_END:
		ScrollTo(stringLength);
		break;
	case TEXTINPUTSYSTEM_CURSOR_HOME:
		ScrollTo(0);
		break;
	default:
		break;
	};
	// cursor limits
	if (m_cursorPosition>stringLength)
		m_cursorPosition=stringLength;
	else if (m_cursorPosition<0)
		m_cursorPosition=0;
	// update display
	if (m_cursorPosition>(m_displayedIndex+m_displayWidth))
		m_displayedIndex++;
	else if (m_cursorPosition<m_displayedIndex)
		m_displayedIndex=m_cursorPosition;
	// display limits
	if (m_displayedIndex>stringLength)
		m_displayedIndex=stringLength;
	else if (m_displayedIndex<0)
		m_displayedIndex=0;
	strncpy(m_pDisplay,m_pOutput+m_displayedIndex,m_displayWidth);

	strcpy(m_pCursor,m_pDisplay);
	m_pCursor[strlen(m_pDisplay)+1]=NULL;
		
	
	char cursor = '|';
	if (m_isOverWriteOn)
		cursor='_';
	m_displayCursor=m_cursorPosition-m_displayedIndex;
	if (m_displayCursor>20)
	{
		int a = m_displayCursor+
		m_cursorPosition+
		m_displayedIndex+
		1;
	}
	if (m_elapsedTime!=-1.0f)
	{
		m_elapsedTime = m_timer.GetElapsedSeconds();
		if (m_elapsedTime==0.0f)
			m_elapsedTime=0.1f;
		m_timeCount += m_elapsedTime;
	}
	else
		m_timeCount+=0.1f;
	if (m_cursorOn)
	{
		if (m_isOverWriteOn)
			m_pCursor[m_displayCursor]=cursor;
		else
			StrUtil_InsertCharacter(m_pCursor,cursor,m_displayCursor);
	}
	else if (!m_isOverWriteOn)
		StrUtil_InsertCharacter(m_pCursor,'.',m_displayCursor);

	if (m_timeCount>TEXTINPUTSYSTEM_CURSOR_BLINK_RATE)
	{
		m_cursorOn=(!m_cursorOn);
		m_timeCount=0.0f;
	}
}
/*************************************************************************

  Scrolls cursor and display to specified index of the output.

*************************************************************************/
void CTextInputSystem::ScrollTo(int index)
{
	if (index>=0 && index<=(int)strlen(m_pOutput))
	{
		m_cursorPosition=index;
		m_displayedIndex=m_cursorPosition-m_displayWidth;
		if (m_displayedIndex<0)
			m_displayedIndex=0;
	}
	ReadText(NULL);
}
/*************************************************************************

  Resets the display width (in characters) to specified value.
  Returns false if there is an error.

*************************************************************************/
bool CTextInputSystem::SetDisplayWidth(int displayWidth)
{
	if (displayWidth<1)
		return false;
	m_pDisplay = (char *)realloc(m_pDisplay,displayWidth+1);
	m_pCursor  = (char *)realloc(m_pCursor,displayWidth+2);
	memset(m_pCursor,TEXTINPUTSYSTEM_CURSOR_SPACER,displayWidth);
	m_pCursor[displayWidth]=NULL;
	return (!(m_pOutput==NULL || m_pDisplay==NULL || m_pCursor==NULL));
}
/*************************************************************************

  Resets the max character input to specified value while clearing the
  Ouput string.
  Returns false if there is an error.

*************************************************************************/
bool CTextInputSystem::SetMaxCharacterInput(int maxCharacterInput)
{
	if (maxCharacterInput<1)
		return false;
	m_pOutput = (char *)realloc(m_pOutput,maxCharacterInput+1);
	memset(m_pOutput,NULL,maxCharacterInput+1);
	return (!(m_pOutput==NULL || m_pDisplay==NULL || m_pCursor==NULL));
}
/*************************************************************************

  Turns overwrite off or on.

*************************************************************************/
void CTextInputSystem::SetOverWriteStatus(bool overWriteOn)
{
	m_isOverWriteOn = overWriteOn;
}
/*************************************************************************

  Resets the system clearing all the strings and setting the cursor and
  display to the start of the Output string.
  Will also accept a string to set the as the initial contents of the
  Ouput string.
  Use this to clear the data in the input system.

*************************************************************************/
void CTextInputSystem::ResetSystem()
{
	m_displayedIndex = 0;
	m_cursorPosition = 0;
	m_displayCursor = 0;
	memset(m_pCursor,NULL,m_displayWidth+2);
	memset(m_pDisplay,NULL,m_displayWidth+1);
	memset(m_pOutput,NULL,m_maxCharacterInput+1);
}
/*************************************************************************

  Resets the system clearing all the strings and setting the cursor and
  display to the start of the Output string.
  Will also accept a string to set the as the initial contents of the
  Ouput string.
  Use this to clear the data in the input system.

*************************************************************************/
void CTextInputSystem::ResetSystem(const char *startString)
{
	if (startString==NULL)
	{
		ResetSystem();
		return;
	}
	m_displayedIndex = 0;
	m_cursorPosition = 0;
	m_displayCursor = 0;
	memset(m_pCursor,NULL,m_displayWidth+2);
	memset(m_pDisplay,NULL,m_displayWidth+1);
	memset(m_pOutput,NULL,m_maxCharacterInput+1);
	if ((int)strlen(startString)<m_maxCharacterInput)
		strcpy(m_pOutput,startString);
}
/*************************************************************************

  Returns false if any of the system pointers are invalid.

*************************************************************************/
bool CTextInputSystem::CheckSystemValidity()
{
	return (!(m_pOutput==NULL || m_pDisplay==NULL || m_pCursor==NULL));
}
/*************************************************************************

  Returns the current overwrite status.

*************************************************************************/
bool CTextInputSystem::GetOverWriteStatus()
{
	return m_isOverWriteOn;
}
/*************************************************************************

  Returns the current Display Width setting.

*************************************************************************/
int CTextInputSystem::GetDisplayWidth()
{
	return m_displayWidth;
}
/*************************************************************************

  Returns the current Max Character Input setting.

*************************************************************************/
int CTextInputSystem::GetMaxCharacterInput()
{
	return m_maxCharacterInput;
}
/*************************************************************************

  Returns the start index of the output used in the display string.

*************************************************************************/
int CTextInputSystem::GetDisplayedIndex()
{
	return m_displayedIndex;
}
/*************************************************************************

  Returns the index of the cursor in the output string.
  If isRelative is set to true it returns the index of the cursor
  relative to the display string.

*************************************************************************/
int CTextInputSystem::GetCursorPosition(bool isRelative)
{
	if (isRelative)
		return m_cursorPosition;
	else
		return m_displayCursor;
}
/*************************************************************************

  Returns a constant pointer to the ouput string.
  Does not copy the string.

*************************************************************************/
const char *CTextInputSystem::GetOutput()
{
	return m_pOutput;
}
/*************************************************************************

  Returns a constant pointer to the display string.
  Does not copy the string.

*************************************************************************/
const char *CTextInputSystem::GetDisplayString()
{
	return m_pDisplay;
}
/*************************************************************************

  Returns a constant pointer to the cursor string.
  Does not copy the string.
  This string is used to display the Cursor relative to the display.

*************************************************************************/
const char *CTextInputSystem::GetCursorString()
{
	return m_pCursor;
}