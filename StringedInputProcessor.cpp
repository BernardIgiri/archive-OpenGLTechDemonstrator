#include "StringedInputProcessor.h"
/******************************************************************************************

  CSIP class constructor.

******************************************************************************************/
CStringedInputProcessor::CStringedInputProcessor()
{
	m_buttonList	  = NULL;
	m_buttonStatus	  = NULL;
	m_buttonListCount = 0;
	m_maxStringLength = 0;
	m_linkSymbol	  = NULL;
	m_timeOut		  = 0.0f;
}
/******************************************************************************************

  CSIP class deconstructor.

******************************************************************************************/
CStringedInputProcessor::~CStringedInputProcessor()
{
	if (m_buttonList!=NULL)
		KillSystem();
	m_buttonList	  = NULL;
	m_buttonStatus	  = NULL;
	m_buttonListCount = 0;
	m_maxStringLength = 0;
	m_linkSymbol	  = NULL;
	m_timeOut		  = 0.0f;
}
/******************************************************************************************

  Initializes system with the necessary data.
  
	SIPstringedButton_t *buttons // info on buttons to be used by system
	int numberOfButtons	// number of buttons in system
	char linkSymbol // represents buttons pressed at same time
	float timeOut // max time of inactivity before system resets output string
	int maxStringLength // max length of output string
	inputManButtonStatus_t **buttonStatus // pointer to button status
	const long double **systemClock // pointer to system clock

/******************************************************************************************/
bool CStringedInputProcessor::Initialize(SIPstringedButton_t *buttons,int numberOfButtons,
									char linkSymbol,float timeOut,int maxStringLength,
									const inputManButtonStatus_t *buttonStatus,
									const long double *systemClock)
{
	bool result = true;
	try
	{
		if (NULL==buttons || NULL==buttonStatus || NULL==systemClock || numberOfButtons<2
			|| NULL==linkSymbol || timeOut<=0.0f || maxStringLength<1)
		{
			result = false;
			throw;
		}
		size_t memorySize = sizeof(SIPstringedButton_t)*numberOfButtons;
		if (m_buttonList!=NULL)
		{
			free(m_buttonList);
			m_buttonList = (SIPstringedButton_t *)malloc(memorySize);
		}
		else
		{
			m_cTimer.Initialize(systemClock);
			m_buttonList = (SIPstringedButton_t *)malloc(memorySize);
		}
		if (NULL==m_buttonList)
		{
			result = false;
			throw;
		}
		memcpy(m_buttonList,buttons,memorySize);
		m_buttonStatus	  = buttonStatus;
		m_buttonListCount = numberOfButtons;
		m_maxStringLength = maxStringLength;
		m_linkSymbol	  = linkSymbol;
		m_timeOut		  = timeOut;
	}
	catch (...)
	{
		result = false;
		throw;
	}
	return result;
}
/******************************************************************************************

  Updates and sends system output.
  Use the second char [] for command processing.
  The other represents the current state of the currently building string.
  Both must be preallocated to maxStringLength + 1 bytes before use.

******************************************************************************************/
void CStringedInputProcessor::Output(char currentString[],char finishedString[])
{
	static float inactiveTime = 0.0f;
	int length = strlen(currentString);
	bool isLinked = false;
	for (int i=0; i<m_buttonListCount; i++)
	{
		if (length>=m_maxStringLength)
			break;
		if ((m_buttonStatus[m_buttonList[i].buttonIndex].buttonValue==1.0f ||
			m_buttonStatus[m_buttonList[i].buttonIndex].buttonValue==-1.0f) &&
			!m_buttonList[i].oldValue)
		{
			m_buttonList[i].oldValue=true;
			if (isLinked)
			{
				char buffer[]={m_linkSymbol,NULL};
				if (length>0)
					strcat(currentString,buffer);
				else
					strcpy(currentString,buffer);
				length++;
			}
			char buffer[]={m_buttonList[i].buttonSymbol,NULL};
			if (length>0)
				strcat(currentString,buffer);
			else
				strcpy(currentString,buffer);
			length++;
			isLinked=true;
		}
		else
			m_buttonList[i].oldValue=false;
		/*if (m_buttonStatus[m_buttonList[i].buttonIndex].buttonValue==1.0f ||
			m_buttonStatus[m_buttonList[i].buttonIndex].buttonValue==-1.0f)
			isLinked=true;*/
	}
	if (!isLinked)
		inactiveTime+=m_cTimer.GetElapsedSeconds();
	else
		inactiveTime=0.0f;
	if (inactiveTime>m_timeOut)
	{
		strcpy(finishedString,currentString);
		if (currentString[0]!=NULL)
			memset(currentString,NULL,m_maxStringLength+1);
	}
}
/******************************************************************************************

  Free's all the memory allocated by the system
  and returns the success of the procedure.

******************************************************************************************/
bool CStringedInputProcessor::KillSystem()
{
	if (NULL==m_buttonList)
		return false;
	free(m_buttonList);
	m_buttonList=NULL;
	return true;
}