#ifndef __STRINGEDINPUTPROCESSOR_INCLUDED__
#define __STRINGEDINPUTPROCESSOR_INCLUDED__
#include "InputManager.h"
#include "SystemTimer.h"

typedef struct
{
	int buttonIndex; // the index value of the button in the input system
	char buttonSymbol; // the symbol to be used to represent the button
	bool oldValue; // used by system to detect button changes
} SIPstringedButton_t;

class CStringedInputProcessor
{
public:
	CStringedInputProcessor();
	~CStringedInputProcessor();
	bool Initialize(SIPstringedButton_t *buttons,int numberOfButtons,char linkSymbol,
					float timeOut,int maxStringLength,
					const inputManButtonStatus_t *buttonStatus,
					const long double *systemClock);
	void Output(char currentString[], char finishedString[]);
	bool KillSystem();
private:
	SIPstringedButton_t			 *m_buttonList;
	const inputManButtonStatus_t *m_buttonStatus;
	int							  m_buttonListCount,
								  m_maxStringLength;
	char						  m_linkSymbol;
	float						  m_timeOut;
	CSystemTimer				  m_cTimer;
};
typedef CStringedInputProcessor CSIP;
#endif