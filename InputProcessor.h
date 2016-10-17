#ifndef __INCLUDED_INPUTPROCESSOR__
#define __INCLUDED_INPUTPROCESSOR__
#include "GLGUIWindowGenerator.h"
#include "Consol.h"
#include "Log.h"
#include "StringedInputProcessor.h"
#include "queue.h"

#define INPUTPROC_CHARBUFSIZE 10
#define INPUTPROC_NUMOFMODES  3

#define INPUTPROC_MAXSTRINGEDINPUT 5

#define INPUTPROC_NOTINIT		0
#define INPUTPROC_ISOK			1
#define INPUTPROC_OUTOFMEMORY	2
#define INPUTPROC_DATANOTFOUND	3
#define INPUTPROC_FILENOTFOUND	4
#define INPUTPROC_FAILEDTOINIT	5
#define INPUTPROC_UNKNOWN		10

bool SetupWinGen(CGLGUIWINDOWGENERATOR *cWinGen,CTextureManager *cTexMan);

typedef struct
{
	int command;
	int param1;
	int param2;
} INPUTPROCGAMECOMMAND_t;

class CINPUTPROC
{
public:
	CINPUTPROC();
	~CINPUTPROC();
	bool Initialize(HWND hwnd, HINSTANCE appInstance,int xRes,int yRes,CLog *pLog);
	void SetScrnRes(int xRes,int yRes);
	void Update();
	void PlayerActOff();
	void PlayerActOn();
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
	int		m_screenWidth;
	int		m_screenHeight;
	int		m_error;
	char	m_charBuf[INPUTPROC_CHARBUFSIZE];
	int		m_charBuffPos;
	Queue	m_gameCmdQueue[INPUTPROC_NUMOFMODES];
	bool	m_playerAct;
};
/**************************************************************************************

  Creates instance of CINPUTPROC

**************************************************************************************/
CINPUTPROC::CINPUTPROC()
{
	m_pInputMan=NULL;
	m_pSIP=NULL;
	m_pWinGen=NULL;
	m_pConsol=NULL;
	m_pLog=NULL;
	m_pTexMan=NULL;
	memset(m_charBuf,NULL,INPUTPROC_CHARBUFSIZE);
	m_screenWidth=640;
	m_screenHeight=480;
	m_error=INPUTPROC_NOTINIT;
	m_playerAct=true;
}
/**************************************************************************************

  Destroys instance of CINPUTPROC

**************************************************************************************/
CINPUTPROC::~CINPUTPROC()
{
	if (m_pInputMan!=NULL)
		Shutdown();
}
/**************************************************************************************

  Initializes CINPUTPROC for usage.

**************************************************************************************/
CINPUTPROC::Initialize(HWND hwnd, HINSTANCE appInstance,int xRes,int yRes,CLog *pLog)
{
	if (m_pInputMan!=NULL)
		Shutdown();
	m_error=INPUTPROC_NOTINIT;
	m_pLog=pLog;
	m_screenWidth=xRes;
	m_screenHeight=yRes;
	m_playerAct=true;
	try
	{
		m_pInputMan=new CInputManager;
		if (m_pInputMan==NULL)
		{
			m_pLog->lprintf("Input Processor Error: failed to allocate ram on initialization.");
			m_error=INPUTPROC_OUTOFMEMORY;
			return;
		}
		m_pInputMan->Initialize(hwnd,appInstance,true,IS_USEKEYBOARD|IS_USEMOUSE);
		{//// INPUT SYSTEM BUTTONS
			const cursorRes = 2000;
			inputManButton_t gameButtons[35];
			int i=0;
			char *analogButtonNames[]={"CursorX","CursorY"};
			int analogButtonKeys[]={INPUTMAN_MOUSE_X,INPUTMAN_MOUSE_Y};
			int analogButtonMaxes[]={cursorRes,cursorRes};
			i = INPUTMANGenerateStandardButtons(gameButtons,i,2,analogButtonNames,analogButtonKeys,
				INPUTMAN_TYPE_ANALOG,INPUTMAN_DEVICE_MOUSE,true,true,analogButtonMaxes);
			i = INPUTMANGenerateStandardButtons(gameButtons,i,"MouseButton",INPUTMAN_MOUSE_BUTTON,
				INPUTMAN_TYPE_BUTTON,INPUTMAN_DEVICE_MOUSE, true, false, 1);
			char *actionButtonNames[]={"Up","Down","Left","Right",
									   "Slash","Stab","Guard","Sheild",
									   "Item1","Item2","Item3","Item4,"Item5","Item6",
									   "Spell0","Spell1","Spell2","Spell3","Spell4","Spell5","Spell6",
										"Spell7","Spell8","Spell9","Spell10","Spell11","Spell12","Spell13"
			};
			int actionButtonKeys[]={DIK_UP,DIK_DOWN,DIK_RIGHT,
									DIK_A,DIK_S,DIK_D,DIK_SPACE,
									DIK_DELETE,DIK_END,DIK_PAGEDOWN,DIK_INSERT,DIK_HOME,DIK_PAGEUP,
									DIK_NUMPAD0,DIK_NUMPAD1,DIK_NUMPAD2,DIK_NUMPAD3,DIK_NUMPAD4,DIK_NUMPAD5,
									 DIK_NUMPAD6,DIK_NUMPAD7,DIK_NUMPAD8,DIK_NUMPAD9,DIK_DIVIDE,DIK_MULTIPLY,
									 DIK_SUBTRACT,DIK_ADD
			};
			i = INPUTMANGenerateStandardButtons(gameButtons,i,28,actionButtonNames,actionButtonKeys,
				INPUTMAN_TYPE_BUTTON,INPUTMAN_DEVICE_KEYBOARD,false,false,1);
			char *toggleButtonNames[]={"Menu","Settings","Consol"};
			int toggleButtonKeys[]={DIK_TAB,DIK_ESC,DIK_GRAVE};
			i = INPUTMANGenerateStandardButtons(gameButtons,i,3,toggleButtonNames,toggleButtonKeys,
				INPUTMAN_TYPE_TOGGLEBUTTON,INPUTMAN_DEVICE_KEYBOARD,false,false,1);
			i = INPUTMANGenerateStandardButtons(gameButtons,i,"End",DIK_F12,INPUTMAN_TYPE_TERMINATE,
				INPUTMAN_DEVICE_KEYBOARD, true, false, 1);
			bool success = m_pInputMan->CreateButtons(i,gameButtons);
			if (success)
				m_pInputMan->SetButtonXYCoordinate(m_pInputMan->GetButtonIndex("CursorX"),m_pInputMan->GetButtonIndex("CursorY"),cursorRes/2,cursorRes/2);
			else
			{
				m_pLog->lprintf("Input Processor Error: failed to initialize CInputManager buttons.");
				m_error=INPUTPROC_FAILEDTOINIT;
				return;
			}
		}
		m_pSIP=new CStringedInputProcessor;
		if (m_pSIP==NULL)
		{
			m_pLog->lprintf("Input Processor Error: failed to allocate ram on initialization.");
			m_error=INPUTPROC_OUTOFMEMORY;
			return;
		}
		{////// STRIGNED BUTTONS
			SIPstringedButton_t stringedButtons[8];
			stringedButtons[0].buttonIndex=m_pInputMan->GetButtonIndex("Up");
			stringedButtons[0].buttonSymbol='A';
			stringedButtons[1].buttonIndex=m_pInputMan->GetButtonIndex("Down");
			stringedButtons[1].buttonSymbol='V';
			stringedButtons[2].buttonIndex=m_pInputMan->GetButtonIndex("Left");
			stringedButtons[2].buttonSymbol='<';
			stringedButtons[3].buttonIndex=m_pInputMan->GetButtonIndex("Right");
			stringedButtons[3].buttonSymbol='>';
			stringedButtons[4].buttonIndex=m_pInputMan->GetButtonIndex("Slash");
			stringedButtons[4].buttonSymbol='/';
			stringedButtons[5].buttonIndex=m_pInputMan->GetButtonIndex("Stab");
			stringedButtons[5].buttonSymbol='*';
			stringedButtons[6].buttonIndex=m_pInputMan->GetButtonIndex("Guard");
			stringedButtons[6].buttonSymbol='#';
			stringedButtons[7].buttonIndex=m_pInputMan->GetButtonIndex("Sheild");
			stringedButtons[7].buttonSymbol='0';
			if (!m_pSIP->Initialize(stringedButtons,8,'+',0.2f,INPUTPROC_MAXSTRINGEDINPUT,
					m_pInputMan->GetKeyboardStatusPointer(),m_pInputMan->GetSystemTimePointer()))
			{
				m_pLog->lprintf("Input Processor Error: failed to initialize CSIP buttons.");
				m_error=INPUTPROC_FAILEDTOINIT;
				return;
			}
		}
		m_pTexMan=new CTextureManager;
		if (m_pTexMan==NULL)
		{
			m_pLog->lprintf("Input Processor Error: failed to allocate ram on initialization.");
			m_error=INPUTPROC_OUTOFMEMORY;
			return;
		}
		{
			char files={"init1","init2"};
			char names={"inproc1","inproc2"};
			m_pTexMan->InitializeSystem(TEXMAN_DEVIL,2,files,names);
		}
		m_pWinGen=new CGLGUIWINDOWGENERATOR;
		if (m_pWinGen==NULL)
		{
			m_pLog->lprintf("Input Processor Error: failed to allocate ram on initialization.");
			m_error=INPUTPROC_OUTOFMEMORY;
			return;
		}
		m_pWinGen->Initialize(m_pLog,m_pTexMan);
		if (!SetupWinGen(m_pWinGen))
		{
			m_pLog->lprintf("Input Processor Error: failed to initialize CWinGen settings.");
			m_error=INPUTPROC_FAILEDTOINIT;
			return;
		}
		unsigned int fontListBase=CreateBitmapFont("Times New Roman",16);
		m_pConsol=new CConsol(fontListBase,13,13,m_screenWidth,m_screenHeight,&m_pLog);
		if (m_pConsol==NULL)
		{
			m_pLog->lprintf("Input Processor Error: failed to allocate ram on initialization.");
			m_error=INPUTPROC_OUTOFMEMORY;
			return;
		}
		memset(m_charBuf,NULL,INPUTPROC_CHARBUFSIZE);
		for (int i=0;i<INPUTPROC_NUMOFMODES;i++)
			queue_init(m_gameCmdQueue[i],free,NULL);
	}
	catch (...)
	{
		m_pLog->lprintf("Input Processor Error: failed to initialize.");
		m_error=INPUTPROC_FAILEDTOINIT;
		throw;
	}
	m_error=INPUTPROC_ISOK;
}
/**************************************************************************************

  Returns error code of last error.
  INPUTPROC_ISOK if ok

**************************************************************************************/
void CINPUTPROC::GetError()
{
	return m_error;
}
/**************************************************************************************

  Updates CINPUTPROC with specified screen resolution.

**************************************************************************************/
void CINPUTPROC::SetScrnRes(int xRes,int yRes)
{
	m_screenWidth=xRes;
	m_screenHeight=yRes;
}

void CINPUTPROC::Update()
{
	static inputManButtonStatus_t *buttonStatus=m_pInputMan->GetStatusPointer();
	static bMenu	=m_pInputMan->GetButtonIndex("Menu"),
		   bSettings=m_pInputMan->GetButtonIndex("Settings"),
		   bConsol	=m_pInputMan->GetButtonIndex("Consol");
	m_pInputMan->Update();

	static char strSIPCur[INPUTPROC_MAXSTRINGEDINPUT+1]={0}, strSIPFin[INPUTPROC_MAXSTRINGEDINPUT+1]={0};
	m_pSIP->Output(strSIPCur,strSIPFin);
	INPUTPROCGAMECOMMAND_t *nextCommand=NULL;
	nextCommand=(INPUTPROCGAMECOMMAND_t *)malloc(sizeof(INPUTPROCGAMECOMMAND_t));
	if (nextCommand==NULL)
	{
		m_pLog->lprintf("Input Processor Error: failed to allocate ram on initialization.");
		m_error=INPUTPROC_OUTOFMEMORY;
		return;
	}
	/*if (buttonStatus[bMenu].buttonValue==1.0f)
	{
		//nextCommand->command=
	}*
	if (strcmp(strSIPFin,"<"))
	{
		*/
}

/**************************************************************************************

  Setups up WinGen with the settings used for window creation.

**************************************************************************************/
bool SetupWinGen(CGLGUIWINDOWGENERATOR *cWinGen,CTextureManager *cTexMan)
{
	cTexMan->AddTextures(	"(gui\\window.jpg,				GUIDefWindow),"
							"(gui\\downButton.jpg,			GUIDownButton),"
							"(gui\\buttonUp.jpg,			GUIButtonUp),"
							"(gui\\buttonDown.jpg,			GUIButtonDown),"
							"(gui\\buttonGlow.jpg,			GUIButtonGlow),"
							"(gui\\checkboxChecked.jpg,		GUIChecked),"
							"(gui\\checkboxUnchecked.jpg,	GUIUnchecked),"
							"(gui\\openDef.jpg,				GUIOpenButtonDef),"
							"(gui\\openMOver.jpg,			GUIOpenButtonMOver),"
							"(gui\\openMDown.jpg,			GUIOpenButtonMDown),"
							"(gui\\scrollUpDef.jpg,			GUIScrollUpButtonDef),"
							"(gui\\scrollUpMOver.jpg,		GUIScrollUpButtonMOver),"
							"(gui\\scrollUpMDown.jpg,		GUIScrollUpButtonMDown),"
							"(gui\\scrollDownDef.jpg,		GUIScrollDownButtonDef),"
							"(gui\\scrollDownMOver.jpg,		GUIScrollDownButtonMOver),"
							"(gui\\scrollDownMDown.jpg,		GUIScrollDownButtonMDown),"
							"(gui\\sliderHorizontal.jpg,	GUIHorizontalSlider),"
							"(gui\\sliderVertical.jpg,		GUIVerticalSlider),"
							"(gui\\slider.jpg,				GUISliderButton),"
							"(gui\\window.png,				GUIWindow2),"
							"(gui\\cyanWindow.png,			GUICyanWindow),"
							"(gui\\blueButtonDef.jpg,		GUIBlueButtonDef),"
							"(gui\\blueButtonMOver.jpg,		GUIBlueButtonMOver),"
							"(gui\\blueButtonMDown.jpg,		GUIBlueButtonMDown),"
							"(gui\\blueOpenDef.jpg,			GUIBlueOpenButtonDef),"
							"(gui\\blueOpenMOver.jpg,		GUIBlueOpenButtonMOver),"
							"(gui\\blueOpenMDown.jpg,		GUIBlueOpenButtonMDown),"
							"(gui\\blueScrollUpDef.jpg,		GUIBlueScrollUpButtonDef),"
							"(gui\\blueScrollUpMOver.jpg,	GUIBlueScrollUpButtonMOver),"
							"(gui\\blueScrollUpMDown.jpg,	GUIBlueScrollUpButtonMDown),"
							"(gui\\blueScrollDownDef.jpg,	GUIBlueScrollDownButtonDef),"
							"(gui\\blueScrollDownMOver.jpg,	GUIBlueScrollDownButtonMOver),"
							"(gui\\blueScrollDownMDown.jpg,	GUIBlueScrollDownButtonMDown),"
							"(gui\\blueSliderV.png,			GUIBlueSliderV),"
							"(gui\\blueSliderH.png,			GUIBlueSliderH),"
							"(gui\\blueSliderButton.png,	GUIBlueSliderButton),"
							"(gui\\pointer.png,				GUIPointer)",
							TEXMAN_DEVIL,false);

	cWinGen->RegisterColorScheme("Blue",0.0f,0.0f,0.25f,
									   0.0f,0.0f,0.75f,
									   0.0f,0.0f,0.5f,
									   0.0f,0.0f,1.0f,
									   0.0f,0.0f,0.0f);
	cWinGen->RegisterColorScheme("Black",0.0f,0.0f,0.0f,
									   0.0f,0.0f,0.0f,
									   0.0f,0.0f,0.0f,
									   0.0f,0.0f,0.0f,
									   0.0f,0.0f,0.0f);
	cWinGen->RegisterColorScheme("Red", 0.25f,0.0f,0.0f,
									   0.75f,0.0f,0.0f,
									   0.5f,0.0f,0.0f,
									   1.0f,0.0f,0.0f,
									   0.0f,0.0f,0.0f);		
	cWinGen->RegisterColorScheme( "Lava", 0.5f,0.0f,0.0f,
									   0.5f,0.5f,0.0f,
									   1.0f,0.0f,0.0f,
									   1.0f,1.0f,0.0f,
									   1.0f,1.0f,0.0f);
	cWinGen->RegisterFontType("Times New Roman",listBase,13,13);
	cWinGen->RegisterFontType("Times New Roman optimized",listBase,8,13);
	cWinGen->RegisterWindowStyle("Gray Box",8,7,7,7,1,"backTile");
	cWinGen->RegisterWindowStyle( "Blue",7,7,6,7,1,"GUIWindow2");
	cWinGen->RegisterWindowStyle("Cyan",5,5,5,5,1,"GUICyanWindow");

	cWinGen->RegisterFullSettings("Cyan Scheme","Blue",NULL,"Blue","DEFAULTBUTTONmDEFTEX='GUIBlueButtonDef' "
		"DEFAULTBUTTONMDOWNTEX='GUIBlueButtonMDown' DEFAULTBUTTONMOVERTEX='GUIBlueButtonMOver'"
		"standardButtonHeight'25',standardButtonWidth='80'"
		"openButtonMDefTex='GUIBlueOpenButtonDef'"
		"openButtonMOverTex='GUIBlueOpenButtonMOver'"
		"openButtonMDownTex='GUIBlueOpenButtonMDown'"
		"scrollDownButtonMDefTex='GUIBlueScrollDownButtonDef'"
		"scrollDownButtonMOverTex='GUIBlueScrollDownButtonMOver'"
		"scrollDownButtonMDownTex='GUIBlueScrollDownButtonMDown'"
		"scrollUpButtonMDefTex='GUIBlueScrollUpButtonDef'"
		"scrollUpButtonMOverTex='GUIBlueScrollUpButtonMOver'"
		"scrollUpButtonMDownTex='GUIBlueScrollUpButtonMDown'"
		"sliderBackTexV='GUIBlueSliderV'"
		"sliderBackTexH='GUIBlueSliderH'"
		"sliderButtonTex='GUIBlueSliderButton'");

	GLGUIWINDOWGENERATORSETUP setup;
	setup.elmtSpacing=2;
	setup.alignment=GLGUI_ALIGNCENTER;
	setup.checkBoxOffTex="GUIUnchecked";
	setup.checkBoxOnTex="GUIChecked";
	setup.standardButtonHeight=28;
	setup.standardButtonWidth=90;
	setup.defaultButtonMDefTex="GUIButtonUp";
	setup.defaultButtonMDownTex="GUIButtonDown";
	setup.defaultButtonMOverTex="GUIButtonGlow";
	setup.dropDownHeight=10;
	setup.dropDownPadding=1;
	setup.openButtonHeight=16;
	setup.openButtonWidth=16;
	setup.openButtonMDefTex="GUIOpenButtonDef";
	setup.openButtonMDownTex="GUIOpenButtonMDown";
	setup.openButtonMOverTex="GUIOpenButtonMOver";
	setup.scrollButtonHeight=8;
	setup.scrollButtonWidth=8;
	setup.scrollDownButtonMDefTex="GUIScrollDownButtonDef";
	setup.scrollDownButtonMDownTex="GUIScrollDownButtonMDown";
	setup.scrollDownButtonMOverTex="GUIScrollDownButtonMOver";
	setup.scrollUpButtonMDefTex="GUIScrollUpButtonDef";
	setup.scrollUpButtonMDownTex="GUIScrollUpButtonMDown";
	setup.scrollUpButtonMOverTex="GUIScrollUpButtonMOver";
	setup.scrollBoxPadding=0;
	setup.shortScrollBoxHeight=5;
	setup.tallScrollBoxHeight=10;
	setup.narrowScrollBoxWidth=20;
	setup.wideScrollBoxWidth=40;
	setup.sliderBackTexV="GUIVerticalSlider";
	setup.sliderBackTexH="GUIHorizontalSlider";
	setup.sliderButtonTex="GUISliderButton";
	setup.textFeildPadding=0;
	setup.topScrollButtonHeight=8;
	setup.topScrollButtonWidth=32;
	cWinGen->SetDefaults(&setup);
	return true;
}

#endif