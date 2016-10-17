#ifndef __INCLUDED_INPUTPROCESSOR__
#define __INCLUDED_INPUTPROCESSOR__
#include "GLGUIWindowGenerator.h"
#include "Consol.h"
#include "Log.h"
#include "StringedInputProcessor.h"

#define INPUTPROC_CHARBUFSIZE 10
#define INPUTPROC_NUMOFMODES  3

#define INPUTPROC_NOTINIT		0
#define INPUTPROC_ISOK			1
#define INPUTPROC_OUTOFMEMORY	2
#define INPUTPROC_DATANOTFOUND	3
#define INPUTPROC_FILENOTFOUND	4
#define INPUTPROC_UNKNOWN		10

class CINPUTPROC
{
public:
	CINPUTPROC();
	~CINPUTPROC();
	bool Initialize(HWND hwnd, HINSTANCE appInstance,int xRes,int yRes,CLog *pLog,CTextureManager *pTexMan);
	void SetScrnRes(int xRes,int yRes);
	void Update();
	int	 GetGameCommand(int mode,int &param1,int &param2);
	char GetChar();
	bool UpdateWin(int handle, void *data);
	int	 MakWin(int windowType,int x, int y, void *data);
	int	 GetError();
	void Shutdown();
protected:
	CInputManager			*m_pInputMan;
	CStringedInputProcessor	*m_pSIP;
	CGLGUIWINDOWGENERATOR	*m_pWinGen;
	CConsol					*m_pConsol;
	CLog					*m_pLog;
	CTextureManager			*m_pTexMan;
	char	m_charBuf;
	int		m_screenWidth;
	int		m_screenHeight;
	int		m_gameCmd;
	int		m_error;
};

CINPUTPROC::CINPUTPROC()
{
	m_pInputMan=NULL;
	m_pSIP=NULL;
	m_pWinGen=NULL;
	m_pConsol=NULL;
	m_pLog=NULL;
	m_pTexMan=NULL;
	m_charBuf=NULL;
	m_gameCmd=-1; // change to nothing
	m_screenWidth=640;
	m_screenHeight=480;
	m_error=INPUTPROC_NOTINIT;
}
CINPUTPROC::~CINPUTPROC()
{
	if (m_pInputMan!=NULL)
		Shutdown();
}
CINPUTPROC::Initialize(HWND hwnd, HINSTANCE appInstance,int xRes,int yRes,CLog *pLog,CTextureManager *pTexMan)
{
	if (m_pInputMan!=NULL)
		Shutdown();
	m_error=INPUTPROC_NOTINIT;
	m_pLog=pLog;
	m_pTexMan=pTexMan;
	m_screenWidth=xRes;
	m_screenHeight=yRes;
	try
	{
		m_pInputMan=new CInputManager();
		if (m_pInputMan==NULL)
		{
			m_pLog->lprintf("Input Processor Error: failed to allocate ram on initialization.");
			m_error=INPUTPROC_OUTOFMEMORY;
			return;
		}
		m_pInputMan->Initialize(hwnd,appInstance,true,IS_USEKEYBOARD|IS_USEMOUSE);
		m_pSIP=new CStringedInputProcessor;
		if (m_pSIP==NULL)
		{
			m_pLog->lprintf("Input Processor Error: failed to allocate ram on initialization.");
			m_error=INPUTPROC_OUTOFMEMORY;
			return;
		}
		// m_pSIP MUST BE INITILIZED
		m_pWinGen=new CGLGUIWINDOWGENERATOR;
		if (m_pWinGen==NULL)
		{
			m_pLog->lprintf("Input Processor Error: failed to allocate ram on initialization.");
			m_error=INPUTPROC_OUTOFMEMORY;
			return;
		}
		m_pWinGen->Initialize(m_pLog,m_pTexMan);
		unsigned int fontListBase=CreateBitmapFont("Times New Roman",16);
		m_pConsol=new CConsol(fontListBase,13,13,m_screenWidth,m_screenHeight,&m_pLog);
		if (m_pConsol==NULL)
		{
			m_pLog->lprintf("Input Processor Error: failed to allocate ram on initialization.");
			m_error=INPUTPROC_OUTOFMEMORY;
			return;
		}
	}
	catch (...)
	{
		m_pLog->lprintf("Input Processor Error: failed to initialize.");
		m_error=INPUTPROC_UNKNOWN;
		throw;
	}
}


#endif