#ifndef __CONSOL_INCLUDED__
#define __CONSOL_INCLUDED__
#include "StringUtilities.h"
#include "SelectionListRenderer.h"
#include "TextFeildRenderer.h"
#include "CLog.h"

#define CONSOLBUFFERSIZE 1024
#define CONSOLDISPLAYWIDTH 100
#define CONSOLMAXCHARACTERINPUT 199
#define CONSOLSWITCHCOMMANDCHAR INPUTMAN_TEXT_CODE_TAB
#define CONSOLENTERCOMMANDCHAR INPUTMAN_TEXT_CODE_ENTER

#define CONSOLCOMMANDCLEARALLCHAR INPUTMAN_TEXT_CODE_F1
#define CONSOLCOMMANDGETLASTSTRINGCHAR INPUTMAN_TEXT_CODE_F2
#define CONSOLCOMMANDDROPDOWNCHAR INPUTMAN_TEXT_CODE_F3

class CConsol
{
public:
	CConsol(int fontListBase, int textWidth, int textHeight,
			int screenWidth, int screenHeight, CLog *cLog,
			int textBufferSize=CONSOLBUFFERSIZE,
			int maxCharacterDisplay=CONSOLDISPLAYWIDTH,
			int maxCharacterInput=CONSOLMAXCHARACTERINPUT);
	~CConsol();
	const char *UpdateAndRender(const unsigned char nextCharacter, bool renderAll=false);
	bool EchoPrintF(const char*format,...);
	bool EchoMessage(const char*msg);
	int  GetDisplayLines();
	void SetDisplayLines(int lines);
	void SetTextStringPF(const char *format,...);
	void SendThru();
	bool GetIsOk();
private:
	bool  m_sendThru,
		  m_isOk,
		  m_redrawAll;
	int	  m_minX,
		  m_minY,
		  m_maxX,
		  m_maxY,
		  m_fontListBase,
		  m_buffSize,
		  m_maxLines,
		  m_maxCharDisplay,
		  m_displayedLines;
	CLog				   *m_cLog;
	CSelectionListRenderer *m_cDispList;
	CTextFeildRenderer	   *m_cTextFeild;	
};

#endif