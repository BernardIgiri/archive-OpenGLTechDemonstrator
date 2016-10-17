#ifndef __INCLUDED_INPUTPROCESSOR__
#define __INCLUDED_INPUTPROCESSOR__
#include "GLGUIWindowGenerator.h"
#include "Consol.h"
#include "StringedInputProcessor.h"
#include "queue.h"

#define INPUTPROC_CHARBUFSIZE 10
#define INPUTPROC_NUMOFMODES  3

class CINPUTPROC
{
public:
	CINPUTPROC();
	~CINPUTPROC();
	bool Initialize(HWND hwnd, HINSTANCE appInstance, bool isExclusive, DWORD flags =0);
	void SetScrnRes(int xRes,int yRes);
	void Update();
	int	 GetGameCommand(int mode,int &param1,int &param2);
	char GetChar();
	bool UpdateWin(int handle, void *data);
	int	 MakWin(int windowType,int x, int y, void *data);
	void Shutdown();
protected:
	CInputManager			*m_InputMan;
	CStringedInputProcessor	*m_SIP;
	CGLGUIWINDOWGENERATOR	*m_WinGen;
	CConsol					*m_Consol;
	char	m_charBuf[INPUTPROC_CHARBUFSIZE];
	int		m_charBuffPos;
	int		m_screenWidth;
	int		m_screenHeight;
	Queue	m_gameCmdQueue[INPUTPROC_NUMOFMODES];
};

CINPUTPROC::CINPUTPROC()
{
	m_InputMan=NULL;
	m_SIP=NULL;
	m_WinGen=NULL;
	m_Consol=NULL;
	m_charBuffPos=0;
	memset(m_charBuffPos,NULL,INPUTPROC_CHARBUFSIZE);
	for (int i=0;i<INPUTPROC_NUMOFMODES;i++)
		queue_init(m_gameCmdQueue[i],free,NULL);
}
CINPUTPROC::~CINPUTPROC()
{
	Shutdown();
}

#endif