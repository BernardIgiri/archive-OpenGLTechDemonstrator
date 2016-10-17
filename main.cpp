#define WIN32_LEAN_AND_MEAN
#define BITMAP_ID 0x4D42
#include <windows.h>
#include <winuser.h>
/*#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>*/
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <math.h>
#include "texture manager.h"
#include "globals.h"
#include "InputManager.h"
#include "StringUtilities.h"
#include "TextInputSystem.h"
#include "Consol.h"
#include "SystemTimer.h"
#include "StringedInputProcessor.h"
#include "PlayerStateMachine.h"
#include "TextFeildRenderer.h"
#include "SelectionListRenderer.h"
#include "ButtonRenderer.h"
#include "GLGUIWindowGenerator.h"
#include "FaceClass.h"
#include "TexFaceClass.h"

//void PrintString(unsigned int base, char *str);
//#include "text input.h"
//#include "input manager.h"
///// Global Varibles
unsigned int listBase;
const int width = 1280;
const int height= 1024;
const int bits = 32;
BOOL      g_isActive = TRUE;
HDC g_HDC;
bool fullScreen = true;
bool addedObject = false;
bool addedObjectS = false;
bool consolOn = false;
bool g_in3DMode=true;
//// L I G H T  Varibles
/**/
float lightPosition[]={0.0f, 0.0f, 15.0f, 0.0f};
float diffuseLight[]={0.0f,0.0f,0.5f,1.0f};
float diffuseMat[]={1.0f, 1.0f,1.0f,1.0f};
float ballDiffuse[]={0.5f, 0.5f,0.0f,1.0f};
float ballSpecular[]={1.0f, 1.0f,1.0f,1.0f};
float ballPosition[]={1.0f, 1.0f,0.0f,1.0f}; // */
CTextureManager allTextures;
//bool g_isActive = true;
CInputManager g_inputMan;
#define SIPMAXCHAR 12
CStringedInputProcessor g_SIP;
//CInputManager g_InputManagement;
// Initialize
/**/

bool GLGUIUpdateWin(CGLGUIWindow *window,int cX, int cY, char currentChar, bool mouseButton,float elapsedTime,int windowType, void *data);
void GLGUIKillWin(CGLGUIWindow *&window)
{
	if (window==NULL)
		return;
	delete window;
	window=NULL;
}


#define GLGUIWIN_IMAGEDISPLAY 100

typedef struct
{
	int width;
	int height;
	char *texName;
} GLGUIWIN_IMAGEDISPLAYparameters;

typedef struct
{
	char *textMsg;
	int numOfResponses;
	char **responses;
} GLGUIWIN_SPEECHWINparameters;

CGLGUIWindow *GLGUIMakeWindow(int windowType, CGLGUIWINDOWGENERATOR *cWinGen,void *data, int x, int y)
{
	CGLGUIWindow *window=NULL;
	switch (windowType)
	{
	case 0:
		{
			char theFileList[1024]={0};
			int theFileListCount = StrUtil_GetFileList(theFileList,"*.bmp",sizeof(theFileList));
			char theJPGFileList[1024]={0};
			StrUtil_GetFileList(theJPGFileList,"*.jpg",sizeof(theJPGFileList));

			cWinGen->SelectFullSettings("Cyan Scheme");

			cWinGen->AddStandardButton("OkButton","Ok");
			cWinGen->AddScrollingText("Text1","Hello World\nWelcome to the test window of the new CGLGUIWINDOWGENERATOR class.",GLGUI_FLAGTALL);
			cWinGen->SelectColorScheme("Black");
			cWinGen->SelectFontType("Times New Roman optimized");
//			cWinGen->AddTextDisplay("Text2","Test of window generator. Let us see just how well this system works. So far the system seems to be just fine. But a test of long text entry such as this should also help in the finding of its ability to handle the possible scenerios.",GLGUI_FLAGNOBACKGROUND);
			cWinGen->SelectColorScheme("Blue");
			cWinGen->SelectFontType("Times New Roman");
			cWinGen->AddImage("Image1","TestWindowImg",256,64);
			break;
		}
	case 1:
		{
			char theFileList[1024]={0};
			int theFileListCount = StrUtil_GetFileList(theFileList,"*.bmp",sizeof(theFileList));
			char theJPGFileList[1024]={0};
			StrUtil_GetFileList(theJPGFileList,"*.jpg",sizeof(theJPGFileList));

			cWinGen->RestoreDefaults();

			cWinGen->SelectColorScheme("Red");
			cWinGen->SelectWindowStyle("Gray Box");
			cWinGen->AddStandardButton("OkButton","Ok");
//			cWinGen->AddScrollingText("Text1","Hello World\nWelcome to the test window of the new CGLGUIWINDOWGENERATOR class.");
			cWinGen->SelectColorScheme("Black");
			cWinGen->SelectFontType("Times New Roman optimized");
			cWinGen->AddTextDisplay("Text2","Test of window generator. Let us see just how well this system works. So far the system seems to be just fine. But a test of long text entry such as this should also help in the finding of its ability to handle the possible scenerios.",GLGUI_FLAGNOBACKGROUND);
			cWinGen->SelectColorScheme("Blue");
			cWinGen->SelectFontType("Times New Roman");
			cWinGen->AddImage("Image1","TestWindowImg",256,64);
			break;
		}
	case 2:
		{
			char theFileList[1024]={0};
			int theFileListCount = StrUtil_GetFileList(theFileList,"*.bmp",sizeof(theFileList));
			char theJPGFileList[1024]={0};
			StrUtil_GetFileList(theJPGFileList,"*.jpg",sizeof(theJPGFileList));

			cWinGen->RestoreDefaults();

			cWinGen->SelectWindowStyle("Cyan");
			cWinGen->AddStandardButton("OkButton","Ok");
			cWinGen->AddCheckBox("CheckTest","Turn Test On",GLGUI_FLAGNOBACKGROUND);
			cWinGen->AddScrollingText("ScrollingText","Test of window generator. Let us see just how well this system works. So far the system seems to be just fine. But a test of long text entry such as this should also help in the finding of its ability to handle the possible scenerios. WWWWWWWWWWWWWWWWWWWW",0);
			cWinGen->AddSlider("Test Slider",100,50);
			cWinGen->SelectColorScheme("Lava");
			cWinGen->AddTextFeild("Text Feild",NULL,20,100);
			cWinGen->AddDropMenu("BMPFileList",theFileList,0);
			cWinGen->NextColumn();
			cWinGen->BeginRow();
			cWinGen->SetAlignment(GLGUI_ALIGNRIGHT);
			cWinGen->AddStandardButton("Button1","Yes");
			cWinGen->AddStandardButton("Button2","No");
			cWinGen->EndRow();
			cWinGen->AddSelectList("JPGFileList",theJPGFileList,GLGUI_FLAGTALL);
			cWinGen->NextColumn();
			cWinGen->SetAlignment(GLGUI_ALIGNLEFT);
			cWinGen->AddStandardButton("Button2","Cancel");
			cWinGen->AddCustomButton("Custom Button","Custom Button",100,15,"Rock","Shell","Eye",1.0f,1.0f,0.0f);
			cWinGen->AddTextDisplay("Text1","Hue",GLGUI_FLAGNOBACKGROUND);
			cWinGen->AddSlider("HueControl",50,40);
			break;
		}
	case 3:
		{
			cWinGen->SelectFullSettings("FadeOut");
			cWinGen->BeginRow();
			cWinGen->AddStandardButton("OkButton","Ok");
			cWinGen->AddStandardButton("Cancel","Cancel");
			cWinGen->EndRow();
			cWinGen->SelectFontType("Times New Roman optimized");
			cWinGen->SelectColorScheme("WhiteOnBlack");
			cWinGen->AddScrollingText("DescriptionBx","Magic - Determins how powerful the effect of your spells will be.\n \n"
													  "Agility - Determins how swiftly your evasions will be, and your ability to move on your feet.\n \n"
													  "Speed - Determins how quick you will be with your hands in the weilding of your weapon.\n \n"
													  "Strength - Determins how easily you will be able to handle hefty objects and strike with power.\n \n"
													  "Spirit - Determins how resilient you will be to the effects of magic attacks.\n \n"
													  "Vitality - Determins how resilient you will be to the effects of physical attacks and poisons.\n \n");
			cWinGen->SelectColorScheme("Gray");
			cWinGen->AddTextDisplay("HlpTxt"," \nHelp",GLGUI_FLAGNOBACKGROUND);
			const barWidth=240;
			cWinGen->AddSlider("MagicBar",barWidth,0);
			cWinGen->AddTextDisplay("MagicTxt","Magic  0",GLGUI_FLAGNOBACKGROUND);
			cWinGen->AddSlider("AgilityBar",barWidth,0);
			cWinGen->AddTextDisplay("AgilityTxt","Agility  0",GLGUI_FLAGNOBACKGROUND);
			cWinGen->AddSlider("SpeedBar",barWidth,0);
			cWinGen->AddTextDisplay("SpeedTxt","Speed  0",GLGUI_FLAGNOBACKGROUND);
			cWinGen->AddSlider("StrengthBar",barWidth,0);
			cWinGen->AddTextDisplay("StrengthTxt","Strength  0",GLGUI_FLAGNOBACKGROUND);
			cWinGen->AddSlider("SpiritBar",barWidth,0);
			cWinGen->AddTextDisplay("SpiritTxt","Spirit  0",GLGUI_FLAGNOBACKGROUND);
			cWinGen->AddSlider("VitalityBar",barWidth,0);
			cWinGen->AddTextDisplay("VitalityTxt","Vitality  0",GLGUI_FLAGNOBACKGROUND);
			cWinGen->AddTextDisplay("Spacer1"," ",GLGUI_FLAGNOBACKGROUND);
			cWinGen->AddSlider("PointsBar",barWidth,barWidth);
			cWinGen->AddTextDisplay("PointsTxt","Points Left 60",GLGUI_FLAGNOBACKGROUND);
			cWinGen->AddImage("TitleImg","GUITitleWindowGeneration",128,32);
			cWinGen->SelectFontType("Times New Roman");
			break;
		}
	case 4:
		{
			cWinGen->SelectFullSettings("Cyan Scheme");
			cWinGen->SelectWindowStyle("IntroWindowStyle");
			cWinGen->AddCustomButton("Exit",NULL,256,64,
				"GUIIntroWinExitUp","GUIIntroWinExitOver","GUIIntroWinExitDown",
				0.0f,0.0f,0.0f);
			cWinGen->AddCustomButton("Settings",NULL,256,64,
				"GUIIntroWinSettingsUp","GUIIntroWinSettingsOver","GUIIntroWinSettingsDown",
				0.0f,0.0f,0.0f);
			cWinGen->AddCustomButton("Continue",NULL,256,64,
				"GUIIntroWinContinueUp","GUIIntroWinContinueOver","GUIIntroWinContinueDown",
				0.0f,0.0f,0.0f);
			cWinGen->AddCustomButton("New",NULL,256,64,
				"GUIIntroWinBeginUp","GUIIntroWinBeginOver","GUIIntroWinBeginDown",
				0.0f,0.0f,0.0f);
			break;
		}
	case 5:
		{
			if (data==NULL)
			{
				cWinGen->AddStandardButton("OkButton","Ok");
				break;
			}
			GLGUIWIN_SPEECHWINparameters *params=(GLGUIWIN_SPEECHWINparameters *)data;
			cWinGen->SelectFullSettings("SpeechWin");
			char buff[1024]={0};
			char buff2[1024]={0};
			if (params->numOfResponses)
			{
				for (int i=(params->numOfResponses-1);i>=0;i--)
				{
					sprintf(buff+i*16,"Response%i",i);
					cWinGen->BeginRow();
					cWinGen->AddStandardButton(buff+i*16,NULL);
					cWinGen->AddTextDisplay("NoName",params->responses[i],GLGUI_FLAGNOBACKGROUND);
					cWinGen->EndRow();
				}
			}
			cWinGen->AddTextDisplay("Text",params->textMsg,GLGUI_FLAGNOBACKGROUND);
			break;
		}
	case GLGUIWIN_IMAGEDISPLAY:
		{
			GLGUIWIN_IMAGEDISPLAYparameters *param=(GLGUIWIN_IMAGEDISPLAYparameters *)data;
			cWinGen->RestoreDefaults();
			cWinGen->AddStandardButton("OkButton","Ok");
			cWinGen->AddImage("Image",param->texName,param->width,param->height);
			break;
		}
	default:
		break;
	}
	window=cWinGen->ReturnWindow(x,y);
	return window;
}

void ClearFont(unsigned int base)
{
	if (base!=0)
		glDeleteLists(base,96);
}
unsigned int CreateBitmapFont(char *fontName, int fontSize)
{
	HFONT hFont;
	unsigned int base;
	base = glGenLists(96);
	if (stricmp(fontName,"symbol")==0)
	{
		hFont = CreateFont(fontSize,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,
						   SYMBOL_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
						   ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH,
						   fontName);
	}
	else
	{
		hFont = CreateFont(fontSize,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,
						   ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
						   ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH,
						   fontName);
	}
	if(!hFont)
		return 0;
	SelectObject(g_HDC,hFont);
	wglUseFontBitmaps(g_HDC,32,96,base);
	return base;
}
void PrintString(unsigned int base, char *str)
{
	if((base==0)||(str==NULL))
		return;
	glPushAttrib(GL_LIST_BIT);
		glListBase(base-32);
		glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);
	glPopAttrib();
}
/**/
bool SetupButtons()
{
	inputManButton_t gameButtons[15];
	int i=0;
	char *analogButtonNames[]={"CursorX","CursorY"};
	int analogButtonKeys[]={INPUTMAN_MOUSE_X,INPUTMAN_MOUSE_Y};
	int analogButtonMaxes[]={2000,2000};//g_screenWidth-1,g_screenHeight-1
	i = INPUTMANGenerateStandardButtons(gameButtons,i,2,analogButtonNames,analogButtonKeys,
		INPUTMAN_TYPE_ANALOG,INPUTMAN_DEVICE_MOUSE,true,true,analogButtonMaxes);
	
	i = INPUTMANGenerateStandardButtons(gameButtons,i,"End",DIK_ESCAPE,INPUTMAN_TYPE_TERMINATE,
		INPUTMAN_DEVICE_KEYBOARD, true, false, 1);

	i = INPUTMANGenerateStandardButtons(gameButtons,i,"Render",DIK_F12,INPUTMAN_TYPE_TOGGLE,
		INPUTMAN_DEVICE_KEYBOARD, true, false, 1);

	char *pushButtonNames[]={"Add1","AddList","ConsolOn"};
	int pushButtonKeys[]={DIK_A,DIK_B,DIK_GRAVE};
	i = INPUTMANGenerateStandardButtons(gameButtons,i,3,pushButtonNames,pushButtonKeys,
		INPUTMAN_TYPE_BUTTON,INPUTMAN_DEVICE_KEYBOARD,false,false,1);

	char *actionButtonNames[]={"Up","Down","Left","Right","Punch","Kick"};
	int actionButtonKeys[]={DIK_W,DIK_S,DIK_A,DIK_D,DIK_P,DIK_L};
	i = INPUTMANGenerateStandardButtons(gameButtons,i,6,actionButtonNames,actionButtonKeys,
		INPUTMAN_TYPE_BUTTON,INPUTMAN_DEVICE_KEYBOARD,false,false,1);

	i = INPUTMANGenerateStandardButtons(gameButtons,i,"MouseButton",INPUTMAN_MOUSE_BUTTON,
		INPUTMAN_TYPE_BUTTON,INPUTMAN_DEVICE_MOUSE, false, false, 1);

	i = INPUTMANGenerateStandardButtons(gameButtons,i,"GUIToggle",INPUTMAN_MOUSE_BUTTON+1,
		INPUTMAN_TYPE_TOGGLE,INPUTMAN_DEVICE_MOUSE, false, false, 1);

	bool success = g_inputMan.CreateButtons(i,gameButtons);
	if (success)
		g_inputMan.SetButtonXYCoordinate(g_inputMan.GetButtonIndex("CursorX"),g_inputMan.GetButtonIndex("CursorY"),2000/2,2000/2);
	return success;
}/***/
bool SetupSIP()
{
	SIPstringedButton_t buttons[6];
	int i=0;
	buttons[i].buttonIndex=g_inputMan.GetButtonIndex("Up");
	buttons[i].buttonSymbol='^';
	i++;
	buttons[i].buttonIndex=g_inputMan.GetButtonIndex("Down");
	buttons[i].buttonSymbol='V';
	i++;
	buttons[i].buttonIndex=g_inputMan.GetButtonIndex("Left");
	buttons[i].buttonSymbol='<';
	i++;	
	buttons[i].buttonIndex=g_inputMan.GetButtonIndex("Right");
	buttons[i].buttonSymbol='>';
	i++;
	buttons[i].buttonIndex=g_inputMan.GetButtonIndex("Punch");
	buttons[i].buttonSymbol='P';
	i++;
	buttons[i].buttonIndex=g_inputMan.GetButtonIndex("Kick");
	buttons[i].buttonSymbol='K';
	i++;
	const inputManButtonStatus_t *buttonStatus = g_inputMan.GetStatusPointer();
	const long double *systemClock = g_inputMan.GetSystemTimePointer();
	return g_SIP.Initialize(buttons,i,'+',0.2f,SIPMAXCHAR,buttonStatus,systemClock);
}
void ProcessSIP()
{
	static char currentString[SIPMAXCHAR+1]={0};
	static char finishedString[SIPMAXCHAR+1]={0};
	g_SIP.Output(currentString,finishedString);
	if (strcmp(finishedString,"<<P+K")==NULL)
	{
		memset(finishedString,NULL,SIPMAXCHAR+1);
		consolOn=true;
	}
}
void InitializeLighting()
{
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, diffuseLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, ballDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, ballSpecular);
	glEnable(GL_LIGHT1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMat);
	glEnable(GL_COLOR_MATERIAL);
}
bool InitializeTexture()
{
	glEnable(GL_TEXTURE_2D);
	char *textureFiles[3] = {"green.bmp","brown.bmp","water.bmp"};
	char *textureNames[3] = {"Green","Brown","Water"};
	return allTextures.InitializeSystem(TEXMAN_BMP,3,textureFiles,textureNames);
}

bool Initialize()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	InitializeLighting();
	glShadeModel(GL_SMOOTH);
	glFrontFace(GL_CCW);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	if(!InitializeTexture())
		return false;
	return SetupButtons()&&SetupSIP();
}

bool WinGenInitialize(CGLGUIWINDOWGENERATOR *cWinGen)
{
	allTextures.AddTextures("(gui\\window.jpg,				backTile),"
							"(gui\\downButton.jpg,			DownButton),"
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
							"(testWindow.png,				TestWindowImg),"
							"(testWindow2.png,				TestWindow2Img),"
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
							"(gui\\fadeWindow.jpg,			GUIFadeOut),"//

							"(gui\\introWindow.png,						GUIIntroWin),"
							"(gui\\introWinButtons\\beginDown.png,		GUIIntroWinBeginDown),"
							"(gui\\introWinButtons\\beginUp.png,		GUIIntroWinBeginUp),"
							"(gui\\introWinButtons\\beginOver.png,		GUIIntroWinBeginOver),"
							"(gui\\introWinButtons\\continueDown.png,	GUIIntroWinContinueDown),"
							"(gui\\introWinButtons\\continueUp.png,		GUIIntroWinContinueUp),"
							"(gui\\introWinButtons\\continueOver.png,	GUIIntroWinContinueOver),"
							"(gui\\introWinButtons\\exitDown.png,		GUIIntroWinExitDown),"
							"(gui\\introWinButtons\\exitUp.png,			GUIIntroWinExitUp),"
							"(gui\\introWinButtons\\exitOver.png,		GUIIntroWinExitOver),"
							"(gui\\introWinButtons\\settingsDown.png,	GUIIntroWinSettingsDown),"
							"(gui\\introWinButtons\\settingsUp.png,		GUIIntroWinSettingsUp),"
							"(gui\\introWinButtons\\settingsOver.png,	GUIIntroWinSettingsOver),"

							"(gui\\titles\\windowGeneration.png,	GUITitleWindowGeneration),"//
							"(gui\\cyanButtonMDef.png,		GUICyanButtonMDef),"
							"(gui\\cyanButtonMOver.png,		GUICyanButtonMOver),"
							"(gui\\cyanButtonMDown.png,		GUICyanButtonMDown),"//

							"(gui\\pointer.png,Arrow)");

	cWinGen->Initialize(&g_cLog,&allTextures);
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
	cWinGen->RegisterFontType("Times New Roman max narrow optimized",listBase,6,13);
	cWinGen->RegisterWindowStyle("Gray Box",8,7,7,7,1,"backTile");
	cWinGen->RegisterWindowStyle( "Blue",7,7,6,7,1,"GUIWindow2");
	cWinGen->RegisterWindowStyle("Cyan",8,8,8,8,1,"GUICyanWindow");
	//
	cWinGen->RegisterColorScheme("Gray",0.25f,0.25f,0.25f,0.0f,0.0f,0.0f,0.5f,0.5f,0.5f,1.0f,1.0f,1.0f,
		0.0f,0.0f,0.0f);
	cWinGen->RegisterColorScheme("WhiteOnBlack",0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.5f,0.5f,0.5f, 1.0f,1.0f,1.0f,
		0.0f,0.0f,0.0f);
	cWinGen->RegisterWindowStyle("FadeWindow",2,2,2,2,1,"GUIFadeOut");
	cWinGen->RegisterFullSettings("FadeOut","FadeWindow",NULL,"Gray",NULL);

	cWinGen->RegisterWindowStyle("IntroWindow",36*5,30*5,36*5,26*5,1,"GUIIntroWin");
	cWinGen->RegisterWindowStyle("IntroWindowStyle",36,30,36,26,1,"GUIIntroWin");

	cWinGen->RegisterFullSettings("SpeechWin","Cyan","Times New Roman max narrow optimized","Blue",
		"defaultButtonMDefTex='GUICyanButtonMDef'"
		"defaultButtonMOverTex='GUICyanButtonMOver'"
		"defaultButtonMDownTex='GUICyanButtonMDown'"
		"standardButtonWidth='32'"
		"standardButtonHeight='16'"
		"alignment='left'"
		"narrowScrollBoxWidth='35'"
		"wideScrollBoxWidth='50'");
	//

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

void WinGenKill(CGLGUIWINDOWGENERATOR *cWinGen)
{
	cWinGen->KillSystem();
}
/**
void textInput()
{
	nothing
}
/**/
void PositionLights()
{
	glLightfv(GL_LIGHT1, GL_POSITION, ballPosition);
}
void DrawTextureCube(float xPos, float yPos, float zPos)
{
	glPushMatrix();
		glTranslatef(xPos,yPos,zPos);
		glBegin(GL_QUADS);
			glNormal3f(0.0f,1.0f,0.0f);
			glTexCoord2f(0.0f, 0.0f);glVertex3f(-0.5f, 0.5f, 0.5f);
			glTexCoord2f(1.0f, 0.0f);glVertex3f( 0.5f, 0.5f, 0.5f);
			glTexCoord2f(1.0f, 1.0f);glVertex3f( 0.5f, 0.5f,-0.5f);
			glTexCoord2f(0.0f, 1.0f);glVertex3f(-0.5f, 0.5f,-0.5f);
		glEnd();
		glBegin(GL_QUADS);
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(0.0f, 0.0f);glVertex3f( 0.5f,-0.5f, 0.5f);
			glTexCoord2f(1.0f, 0.0f);glVertex3f( 0.5f, 0.5f, 0.5f);
			glTexCoord2f(1.0f, 1.0f);glVertex3f(-0.5f, 0.5f, 0.5f);
			glTexCoord2f(0.0f, 1.0f);glVertex3f(-0.5f,-0.5f, 0.5f);
		glEnd();
		glBegin(GL_QUADS);
			glNormal3f(1.0f,0.0f,0.0f);
			glTexCoord2f(1.0f, 0.0f);glVertex3f( 0.5f, 0.5f, 0.5f);
			glTexCoord2f(1.0f, 1.0f);glVertex3f( 0.5f,-0.5f, 0.5f);
			glTexCoord2f(0.0f, 1.0f);glVertex3f( 0.5f,-0.5f,-0.5f);
			glTexCoord2f(0.0f, 0.0f);glVertex3f( 0.5f, 0.5f,-0.5f);
		glEnd();
		glBegin(GL_QUADS);
			glNormal3f(-1.0f,0.0f,0.0f);
			glTexCoord2f(0.0f, 0.0f);glVertex3f(-0.5f,-0.5f, 0.5f);
			glTexCoord2f(1.0f, 0.0f);glVertex3f(-0.5f, 0.5f, 0.5f);
			glTexCoord2f(1.0f, 1.0f);glVertex3f(-0.5f, 0.5f,-0.5f);
			glTexCoord2f(0.0f, 1.0f);glVertex3f(-0.5f,-0.5f,-0.5f);
		glEnd();
		glBegin(GL_QUADS);
			glNormal3f(0.0f,-1.0f,0.0f);
			glTexCoord2f(0.0f, 0.0f);glVertex3f( 0.5f,-0.5f, 0.5f);
			glTexCoord2f(1.0f, 0.0f);glVertex3f(-0.5f,-0.5f, 0.5f);
			glTexCoord2f(1.0f, 1.0f);glVertex3f(-0.5f,-0.5f,-0.5f);
			glTexCoord2f(0.0f, 1.0f);glVertex3f( 0.5f,-0.5f,-0.5f);
		glEnd();
		glBegin(GL_QUADS);
			glNormal3f(0.0f,0.0f,-1.0f);
			glTexCoord2f(0.0f, 1.0f);glVertex3f( 0.5f,-0.5f,-0.5f);
			glTexCoord2f(0.0f, 0.0f);glVertex3f(-0.5f,-0.5f,-0.5f);
			glTexCoord2f(1.0f, 0.0f);glVertex3f(-0.5f, 0.5f,-0.5f);
			glTexCoord2f(1.0f, 1.0f);glVertex3f( 0.5f, 0.5f,-0.5f);
		glEnd();
	glPopMatrix();
}
void DrawObjects(float elapsedTime)
{
	static const float elapseLimit = 0.2f;
	if (elapsedTime>elapseLimit)
		elapsedTime=elapseLimit;
	static const inputManButtonStatus_t* buttonStatus = g_inputMan.GetStatusPointer();
	static int cursorX = g_inputMan.GetButtonIndex("CursorX"),
			   cursorY = g_inputMan.GetButtonIndex("CursorY");
	static float pos = 0;
	const float radius = 65.0f;
	pos+=100.0f*elapsedTime;
	if (pos>720.0f)
		pos=pos-720.0f;
	float radians = float(3.141592654*(pos-(pos*0.5f))/180.0f);
	float posX = float(0.0f + sin(radians)*radius);
	float posZ = float(0.0f + cos(radians)*radius);
	float posY = float(-20.0f + radius/2.0f);

	glPushMatrix();
		glColor3f(0.0f,1.0f,1.0f);
		glTranslatef(posX,posY,posZ);
		glRotatef((float)pos,0.0f,1.0f,0.0f);
		//allTextures.SelectTexture(0);
		allTextures.SelectTexture((int)(0+pos/60)%3);
		glScalef(5.0f,5.0f,5.0f);
		DrawTextureCube(0.0f,0.0f,0.0f);
	glPopMatrix();/**/

	/*glPushMatrix();
		glColor3f(0.0f,1.0f,1.0f);
		glTranslatef((buttonStatus[cursorX].buttonValue-0.5f)*200.0f,
					 (buttonStatus[cursorY].buttonValue-0.5f)*-150.0f,
					 0.1f);
		DrawTextureCube(0.0f,0.0f,0.0f);
	glPopMatrix();*/

	glPushMatrix();
		glColor3f(0.0f,1.0f,1.0f);
		glTranslatef(posX,posY+10,posZ);
		glRotatef((float)pos,0.0f,1.0f,0.0f);
		//allTextures.SelectTexture(1);
		allTextures.SelectTexture((int)(1+pos/60)%3);
		glScalef(5.0f,5.0f,5.0f);
		DrawTextureCube(0.0f,0.0f,0.0f);
	glPopMatrix();

	glPushMatrix();
		glColor3f(0.0f,1.0f,1.0f);
		glTranslatef(posX,posY+20,posZ);
		glRotatef((float)pos,0.0f,1.0f,0.0f);
		//allTextures.SelectTexture(2);
		allTextures.SelectTexture((int)(2+pos/60)%3);
		glScalef(5.0f,5.0f,5.0f);
		DrawTextureCube(0.0f,0.0f,0.0f);
	glPopMatrix();

	/*glPushMatrix();
		glColor3f(0.0f,1.0f,1.0f);
		glTranslatef(posX,posY-10,posZ);
		glRotatef((float)pos,0.0f,0.5f,0.5f);
		allTextures.SelectTexture("Brown");
		glScalef(5.0f,5.0f,5.0f);
		DrawTextureCube(0.0f,0.0f,0.0f);
	glPopMatrix();*/

	glPushMatrix();
	{
		static bool firstTime=true;
		static CFace testFace;
		const nVertices=4;
		static VERTEX pVertices[nVertices];
		if (firstTime)
		{			
			float fVertexes[nVertices*3]= {0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,0.5f,-0.5f,0.5f,0.5f,-0.5f};
			memcpy(pVertices,fVertexes,sizeof(fVertexes));
			face_set(&testFace,GL_TRIANGLE_FAN,nVertices,pVertices);
		}
		/*glTranslatef(posX,posY-60,posZ);
		glRotatef((float)pos,0.0f,0.5f,0.5f);
		glNormal3f(0.0f,0.0f,-1.0f);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f,0.0f,1.0f);
		glScalef(5.0f,5.0f,5.0f);
		testFace.Draw();
		glEnable(GL_TEXTURE_2D);*/
		//
		glColor3f(0.0f,1.0f,1.0f);
		glTranslatef(posX,posY-10,posZ);
		glRotatef(180.0f,0.0f,1.0f,0.0f);
		glDisable(GL_TEXTURE_2D);
		//glDisable(GL_LIGHTING);
		//glDisable(GL_CULL_FACE);
		glNormal3f(0.0f,0.0f,0.0f);
		glScalef(5.0f,5.0f,5.0f);
		testFace.Draw();
		//glEnable(GL_CULL_FACE);
		//glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		//
		firstTime=false;
	}
	glPopMatrix();

	glPushMatrix();
	{
		static bool firstTime=true;
		static CTexFace texFace;
		if (firstTime)
		{
			firstTime=false;
			const nVertices=4;
			static float fTexCoords[nVertices*2]={0.0f,1.0f,0.0f,0.0f,1.0f,0.0f,1.0f,1.0f};
			static float fVertexes[nVertices*3]= {0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,0.5f,-0.5f,0.5f,0.5f,-0.5f};
			texface_set(&texFace,GL_TRIANGLE_FAN,nVertices,(VERTEX *)fVertexes,(TEXCOORD *)fTexCoords);
			//static VERTEXDATA VertexDataSpace[nVertices]={0};
			//texFace.SetData(GL_TRIANGLE_FAN,nVertices,VertexDataSpace,(VERTEX *)fVertexes,(TEXCOORD *)fTexCoords);
		}
		glColor3f(0.0f,1.0f,1.0f);
		glTranslatef(posX,posY-20,posZ);
		glRotatef(180.0f,0.0f,1.0f,0.0f);
		glNormal3f(0.0f,0.0f,0.0f);
		glScalef(5.0f,5.0f,5.0f);
		texFace.Draw();
	}
	glPopMatrix();

	if(addedObject)
	{
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glPushMatrix();
			glColor3f(0.0f,1.0f,1.0f);
			glTranslatef(posX,posY-20,posZ);
			glRotatef((float)-pos,0.0f,0.5f,0.5f);
			allTextures.SelectTexture("Chess");
			glScalef(5.0f,5.0f,5.0f);
			DrawTextureCube(0.0f,0.0f,0.0f);
		glPopMatrix();
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}
	if(addedObjectS)
	{
		glPushMatrix();
			glColor3f(0.0f,1.0f,1.0f);
			glTranslatef(posX,posY-30,posZ);
			glRotatef((float)-pos,0.0f,1.0f,0.0f);
			allTextures.SelectTexture("Rock");
			glScalef(5.0f,5.0f,5.0f);
			DrawTextureCube(0.0f,0.0f,0.0f);
		glPopMatrix();
		glPushMatrix();
			glColor3f(0.0f,1.0f,1.0f);
			glTranslatef(posX,posY-40,posZ);
			glRotatef((float)-pos,0.0f,1.0f,0.0f);
			allTextures.SelectTexture("Shell");
			glScalef(5.0f,5.0f,5.0f);
			DrawTextureCube(0.0f,0.0f,0.0f);
		glPopMatrix();
		glPushMatrix();
			glColor3f(0.0f,1.0f,1.0f);
			glTranslatef(posX,posY-50,posZ);
			glRotatef((float)-pos,0.0f,1.0f,0.0f);
			allTextures.SelectTexture("Eye");
			glScalef(5.0f,5.0f,5.0f);
			DrawTextureCube(0.0f,0.0f,0.0f);
		glPopMatrix();
	}
}
/*void ProcessInput()
{
	float status=0.0f,secondsHeld=0.0f,secondsInactive=0.0f;
	char buffer[100]={0};
	
	glPushMatrix();
		glTranslatef(0.0f,0.0f,-1.0f);
		glRasterPos2f(-0.65f,0.35f);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f,1.0f,1.0f);/*
		PrintString(1,buffer);
		glEnable(GL_TEXTURE_2D);
	glPopMatrix();*

	status = g_InputManagement.ButtonStatus("Jump",&secondsHeld,&secondsInactive);
	if (status)
		sprintf(buffer,"Jump for %02.2f seconds",secondsHeld);
	else
		sprintf(buffer,"Jump %02.2f seconds ago.",secondsInactive);
	//
	PrintString(listBase,buffer);

	//
	status = g_InputManagement.ButtonStatus("Left",&secondsHeld,&secondsInactive);
	if (status)
		sprintf(buffer,"Left for %02.2f seconds",secondsHeld);
	else
		sprintf(buffer,"Left %02.2f seconds ago.",secondsInactive);
	//
	glRasterPos2f(-0.65f,0.30f);
	PrintString(listBase,buffer);
	//

	//
	status = g_InputManagement.ButtonStatus("Right",&secondsHeld,&secondsInactive);
	if (status)
		sprintf(buffer,"Right for %02.2f seconds",secondsHeld);
	else
		sprintf(buffer,"Right %02.2f seconds ago.",secondsInactive);
	//
	glRasterPos2f(-0.65f,0.25f);
	PrintString(listBase,buffer);
	//

	//
	status = g_InputManagement.ButtonStatus("Booster",&secondsHeld,&secondsInactive);
	if (status)
		sprintf(buffer,"Booster for %02.2f seconds",secondsHeld);
	else
		sprintf(buffer,"Booster %02.2f seconds ago.",secondsInactive);
	//
	glRasterPos2f(-0.65f,0.30f);
	PrintString(listBase,buffer);
	//
		glEnable(GL_TEXTURE_2D);
	glPopMatrix();
}*/
// Render
void Render(float elapsedTime)
{
	//ProcessInput();
	ProcessSIP();
	glTranslatef(0.0f,0.0f,-150.0f);
	PositionLights();
	DrawObjects(elapsedTime);
}

void SetupPixelFormat(HDC hDC)
{
	int nPixelFormat;
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0,0,0,0,0,0,
		0,
		0,
		0,
		0,0,0,0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0 };
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}

void CleanUp()
{
	ClearFont(listBase);
	allTextures.KillSystem();
	g_SIP.KillSystem();
	return;
}

/*unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE				*filePtr;
	BITMAPFILEHEADER	bitmapFileHeader;
	unsigned char		*bitmapImage;
	int					imageIdx = 0;
	unsigned char		tempRGB;
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(filePtr);
		return NULL;
	}
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
	if(bitmapImage==NULL)
	{
		fclose(filePtr);
		return NULL;
	}
	for (imageIdx = 0; (unsigned)imageIdx < bitmapInfoHeader->biSizeImage; imageIdx+=3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx+2];
		bitmapImage[imageIdx+2] = tempRGB;
	}
	fclose(filePtr);
	return bitmapImage;
}*/


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HGLRC hRC;
	static HDC hDC;
	int width, height;

	switch(message)
	{
	case WM_ACTIVATE:  // watch for the window being minimized and restored
    {
      if (!HIWORD(wParam))
      {
        // program was restored or maximized
        g_isActive = TRUE;
		g_inputMan.m_input.AcquireAll();
      }
      else
      {
        // program was minimized
        g_isActive=FALSE;
		g_inputMan.m_input.UnacquireAll();
      }

      return 0;
    }
	case WM_CREATE:
		hDC = GetDC(hwnd);
		g_HDC = hDC;
		SetupPixelFormat(hDC);
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);

		return 0;
		break;

	case WM_CLOSE:
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);
		PostQuitMessage(0);

		return 0;
		break;

	case WM_SIZE:
		height = HIWORD(lParam);
		width = LOWORD(lParam);
		if (height==0)
		{
			height=1;
		}

		glViewport(0,0,width,height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		gluPerspective(54.0f,(GLfloat)width/(GLfloat)height,1.0f,1000.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		return 0;
		break;

	default:
		break;
	}
	return (DefWindowProc(hwnd,message,wParam,lParam));
}
void ConsolCommandProcessor(CConsol &cCommandConsol, CSystemTimer sysTimers[], bool *bDone,
						const char **ReceivedCommand, int consolB,
					const unsigned char **pInputChar,unsigned int listBase,CGLGUIWINDOWGENERATOR *cWinGen)
{
	const unsigned char *pInputCharacter=(*pInputChar);
	bool done=false;
	const char *command=(*ReceivedCommand);
	
	if (strncmp("say ",command,4)==NULL)
	{
		char buffer[200]={0};
		sprintf(buffer,"\"%s\"",command+4);
		cCommandConsol.EchoPrintF(buffer);
	}
	else if (strncmp("word wrapS ",command,11)==NULL)
	{
		char *newString=NULL;
		int *lineIndexes=NULL;
		int lines = StrUtil_WordWrapS(" ,;'\"-+*/",command+11,20,
									  &newString,lineIndexes);
		char lineString[23]={0};
		int i=0, oldI=0, length=strlen(newString);
		while (i<length)
		{
			if (StrUtil_ReadNextLine(lineString,newString+i,&i,22))
				cCommandConsol.EchoPrintF(lineString);
			if (i==oldI)
				break;
			oldI=i;
		}
		free(newString);
		free(lineIndexes);
	}
	else if (strcmp("word wrapSTest",command)==NULL)
	{ // successfully passed
		char *testString="This is a test string with newline characters"
			" such as\nThat\nand That to see if this thing handles these"
			"\nproperly or to see if\nfails to register them right.";
		char *newString=NULL;
		int *lineIndexes=NULL;
		int lines = StrUtil_WordWrapS(" ,;'\"-+*/",testString,22,
									  &newString,lineIndexes);
		char lineString[25]={0};
		int i=0, oldI=0, length=strlen(newString);
		while (i<length)
		{
			if (StrUtil_ReadNextLine(lineString,newString+i,&i,24))
				cCommandConsol.EchoPrintF(lineString);
			if (i==oldI)
				break;
			oldI=i;
		}
		free(newString);
		free(lineIndexes);
	}
	else if (strcmp("testConsol FEcho",command)==NULL)
	{
		char *testString="This is a test string with newline characters"
			" such as\nThat\nand That to see if this thing handles these"
			"\nproperly or to see if\nfails to register them right.";
		cCommandConsol.EchoPrintF(testString,true);
	}
	else if (strcmp("testConsol EchoPrintF",command)==NULL)
	{
		cCommandConsol.EchoPrintF("Test of printf methods on the consol\n"
			"this should work like a %i point dream down to the %3.3f percentage "
			"point, with so much ease it comes through like magic butter "
			"on a hot knife.",5,99.99f);
	}
	else if (strncmp("word wrap ",command,10)==NULL)
	{ // not perfect, avoid
		static CStringList wrappedList;
		static bool beenHere=false;
		static char **pListPtr=NULL;
		int lines = 0;
		if (!beenHere)
		{
			wrappedList.Initialize();
			pListPtr=wrappedList.GetList();
			beenHere=true;
		}
		StrUtil_WordWrap(" ,;'\"-+*/",command+10,10,&wrappedList,0,&lines);
		int listCount = wrappedList.GetListCount();
		if (lines<listCount)
			listCount=lines;
		if (listCount>=2)
			for (int i=0; i<listCount; i++)
				cCommandConsol.EchoPrintF(pListPtr[i]);
	}
	else if (strncmp("find file ",command,10)==NULL)
	{ // don't use this method
		CStringList cFileList;
		_finddatai64_t fileData;
		long handle = _findfirsti64(command+10,&fileData);
		if (handle!=-1 && cFileList.Initialize())
		{
			char **pFileList = NULL;
			pFileList = cFileList.GetList();
			cFileList.AddStringToList(fileData.name);
			while (_findnexti64(handle,&fileData)!=-1)
				if (fileData.name!=NULL && fileData.name[0]!=NULL)
					bool result = cFileList.AddStringToList(fileData.name);
			int listCount = cFileList.GetListCount();
			if (pFileList!=NULL)
				if (listCount==1)
					cCommandConsol.EchoPrintF(pFileList[0]);
				else
					for (int i=0; i<listCount; i++)
						cCommandConsol.EchoPrintF(pFileList[i]);
		}
	}
	else if (strncmp("find files ",command,11)==NULL)
	{ // even better
		char fileList[1024]={0};
		int count = StrUtil_GetFileList(fileList,command+11,sizeof(fileList));
		if (count)
			cCommandConsol.EchoPrintF(fileList);
		if (count>1)
			cCommandConsol.EchoPrintF("%i Files Found.",count);
		else if (1==count)
			cCommandConsol.EchoPrintF("%i File Found.",count);
		else
			cCommandConsol.EchoPrintF("No Files found.",true);
	}
	else if (strncmp("old find files ",command,15)==NULL)
	{ // works well
		char fileList[1024]={0};
		_finddatai64_t fileData;
		long handle = _findfirsti64(command+15,&fileData);
		strcpy(fileList,fileData.name);
		strcat(fileList,"\n");
		if (handle!=-1)
		{
			while (_findnexti64(handle,&fileData)!=-1)
				if (fileData.name!=NULL && fileData.name[0]!=NULL)
				{
					strcat(fileList,fileData.name);
					strcat(fileList,"\n");
				}
			int i=0, oldI=0, length=strlen(fileList);
			const int displayWidth = 70;
			char lineString[displayWidth+2]={0};
			while (i<length)
			{
				if (StrUtil_ReadNextLine(lineString,fileList+i,&i,displayWidth))
				cCommandConsol.EchoPrintF(lineString);
				if (i==oldI)
					break;
				oldI=i;
			}
		}
	}
	else if (strncmp("set display ",command,12)==NULL)
	{
		int lines=0;
		sscanf(command+12,"%i",&lines);
		int oldLines=cCommandConsol.GetDisplayLines();
		if (lines<oldLines)
			Render(0.0f);
		cCommandConsol.SetDisplayLines(lines);
	}
	else if (strncmp("encode ",command,7)==NULL)
	{
		char string[200], key[200];
		strcpy(string,command+7);
		int index = StrUtil_FindChar(',',string);
		if (index!=-1)
		{
			strcpy(key,string+index+1);
			string[index]=NULL;
			StrUtil_Encript(string,key);
			cCommandConsol.EchoPrintF(string);
			StrUtil_Decript(string,key);
			cCommandConsol.EchoPrintF(string);
		}
		else
			cCommandConsol.EchoPrintF("Use the ',' char to divide the strings");
	}
	else if (strncmp("loadImage ",command,10)==NULL)
	{
		int len=strlen(command);
		if (len>12)
		{
			allTextures.AddTextures((char *)(command+10));
			GLGUIWIN_IMAGEDISPLAYparameters param;
			/*for (int i=10; i<len; i++)
				if (strlen*/
			param.texName=(char *)allTextures.FindTextureName(allTextures.GetListCount()-1);
			allTextures.SelectTexture(param.texName);
			allTextures.GetCurTexDimensions(param.width,param.height);
			CGLGUIWindow *window = GLGUIMakeWindow(GLGUIWIN_IMAGEDISPLAY,cWinGen,&param,300,100);
			if (window!=NULL)
			{
				if (g_in3DMode)
				{
					glMatrixMode(GL_PROJECTION);
					//glClear(GL_DEPTH_BUFFER_BIT || GL_COLOR_BUFFER_BIT);
					glLoadIdentity();
					glDisable(GL_DEPTH_TEST);

					// set the perspective with the appropriate aspect ratio
					gluOrtho2D(0, width, 0, height);


					// select modelview matrix
					glMatrixMode(GL_MODELVIEW);
				
					glLoadIdentity();

					g_in3DMode=false;
				}
				const inputManButtonStatus_t* buttonStatus = g_inputMan.GetStatusPointer();
				int endButton = g_inputMan.GetButtonIndex("End"),
					addOneB	  = g_inputMan.GetButtonIndex("Add1"),
					addListB  = g_inputMan.GetButtonIndex("AddList"),
					consolB   = g_inputMan.GetButtonIndex("ConsolOn"),
					cursorX	  = g_inputMan.GetButtonIndex("CursorX"),
					cursorY	  = g_inputMan.GetButtonIndex("CursorY"),
					renderB	  = g_inputMan.GetButtonIndex("Render"),
					mouseB	  = g_inputMan.GetButtonIndex("MouseButton"),
					guiB	  = g_inputMan.GetButtonIndex("GUIToggle");
				///cX,cY,current,(buttonStatus[mouseB].buttonValue==1.0f),sysTimers[3].GetElapsedSeconds(),i,NULL)
				{
					glEnable(GL_TEXTURE_2D);
					int arrowIndex = allTextures.FindTextureIndex("Arrow");
					int arrowWidth=0, arrowHeight=0;
					allTextures.SelectTexture(arrowIndex);
					bool isAlhpa = (allTextures.GetTexBpp(arrowIndex)>=32);
					if (arrowWidth==0)
					{
						allTextures.GetCurTexDimensions(arrowWidth,arrowHeight);
						/**if (pixelBuffer==NULL)
						{
							pixelBuffer=malloc(arrowWidth*arrowHeight*3);
							memset(pixelBuffer,0,arrowWidth*arrowHeight*3);
						}/**/
					}
					const unsigned char *textInputChar = g_inputMan.GetTextInputPointer();
					bool finished=false;
					while (!finished)
					{
						g_inputMan.Update();
						unsigned char current = INPUTMANConvertTextInputToCharacters(textInputChar,g_inputMan.GetCapsLock(),g_inputMan.GetShiftStatus());
						int cX = (int)(buttonStatus[cursorX].buttonValue*(width-1)),
							cY = (int)(-buttonStatus[cursorY].buttonValue*(height-2))+(height-1);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color/depth buffer
						glLoadIdentity();
						finished=GLGUIUpdateWin(window,cX,cY,current,(buttonStatus[mouseB].buttonValue==1.0f),sysTimers[3].GetElapsedSeconds(),GLGUIWIN_IMAGEDISPLAY,&param);
						allTextures.SelectTexture(arrowIndex);
						GLGUIRenderCursor(arrowWidth,arrowHeight,cX,cY,NULL,isAlhpa);
						glFlush();
						SwapBuffers(g_HDC);
					}
				}
			}


		}
	}
	else if (strcmp("RedrawScreen",command)==NULL)
	{
		Render(0.0f);
	}
	else if (strcmp("create text input feild",command)==NULL)
	{

		// select the projection matrix and clear it out
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// set the perspective with the appropriate aspect ratio
		gluOrtho2D(0, width, 0, height);

		// select modelview matrix
		glMatrixMode(GL_MODELVIEW);

		char copyOfText[101]={0};
		bool finished=false;
		CLog *pcLogPtr=&g_cLog;
		CTextFeildRenderer textBox1(listBase,10,100,20,25,200,200,5,&pcLogPtr);
			/*(int fontListBase, int displayWidth, int maxChars,
									   int textHeight, int textWidth, int top, int left,
									   int padding, CLog **cLog)*/
		const unsigned char *textInputChar = g_inputMan.GetTextInputPointer();
		while (!finished)
		{
			g_inputMan.Update();
			unsigned char current = INPUTMANConvertTextInputToCharacters(textInputChar,g_inputMan.GetCapsLock(),g_inputMan.GetShiftStatus());
			const char *enteredText = textBox1.UpdateAndRender(current);
			if (enteredText!=NULL)
			{
				finished=true;
				strcpy(copyOfText,enteredText);				
			}
			glFlush();
			SwapBuffers(g_HDC);
		}
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		gluPerspective(54.0f,(GLfloat)width/(GLfloat)height,1.0f,1000.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Render(0.0f);
		cCommandConsol.EchoPrintF(copyOfText);
	}
	else if (strcmp("get display",command)==NULL)
	{
		char buffer[100]={0};
		sprintf(buffer,"Currently only %i lines are shown",cCommandConsol.GetDisplayLines());
		cCommandConsol.EchoPrintF(buffer);
	}
	else if (strcmp("get format sizes",command)==NULL)
	{
		char buffer[100]={0};
		sprintf(buffer,"Size of PlayerStaticState %i Size of PlayerVolatileState %i",
			sizeof(plyrStaticState_t),sizeof(plyrVolatileState_t));
		cCommandConsol.EchoPrintF(buffer);
	}
	else if (strcmp("getTime1",command)==NULL)
	{
		char buffer[100]={0};
		sprintf(buffer,"Timer1 Time elapsed: %3.3f FPS: %3.1f.",
		sysTimers[0].GetElapsedSeconds(),sysTimers[0].GetFPS());
		cCommandConsol.EchoPrintF(buffer);
	}
	else if (strcmp("getTime2",command)==NULL)
	{
		char buffer[100]={0};
		sprintf(buffer,"Timer2 Time elapsed: %3.3f FPS: %3.1f.",
		sysTimers[1].GetElapsedSeconds(),sysTimers[1].GetFPS());
		cCommandConsol.EchoPrintF(buffer);
	}
	else if (strcmp("test Error Checking",command)==NULL)
	{ // will cause an error, but it does work
		char *buffer=NULL;
		try
		{
			char string[]="Crash this thing.";
			strcpy(buffer,string);
		}
		catch(...)
		{
			g_cLog.lprintf("Usage of null pointer in error causing test, caused an error.");
			throw;
		}
	}
	else if (strcmp("getTime2FPS",command)==NULL)
	{
		char buffer[100]={0};
		sprintf(buffer,"Timer2 Time elapsed: %3.3f FPS: %3.1f.",
		sysTimers[1].GetElapsedSeconds(),sysTimers[1].GetFPS());
		cCommandConsol.EchoPrintF(buffer);
		sprintf(buffer,"Timer2 Time elapsed: %3.3f FPS: %3.1f.",
		sysTimers[1].GetElapsedSeconds(),sysTimers[1].GetFPS());
		cCommandConsol.EchoPrintF(buffer);
	}
	else if (strcmp("getTime3FPS",command)==NULL)
	{
		char buffer[100]={0};
		sprintf(buffer,"Timer 3 FPS: %3.1f.",sysTimers[2].GetFPS());
		cCommandConsol.EchoPrintF(buffer);
	}
	else if (strcmp("get texture list count",command)==NULL)
	{
		char buffer[100]={0};
		sprintf(buffer,"The current texture list count is %i.",allTextures.GetListCount());
		cCommandConsol.EchoPrintF(buffer);
	}
	else if (strcmp("get bpp",command)==NULL)
	{
		cCommandConsol.EchoPrintF("Tex1 Bpp %i Chess Bpp %i",allTextures.GetTexBpp(1),allTextures.GetTexBpp("Chess"));
	}
	else if (strcmp("get texture memory size",command)==NULL)
	{
		char buffer[100]={0};
		cCommandConsol.EchoPrintF(" Size of Texture Data %i\n"
			" Texture List Count: %i\n"
			" Texture List Size: %i\n"
			" Total Texture(s) Size: %i\n"
			" Texture System Size: %i\n"
			" Total Memory Usage: %i\n",
			sizeof(texture_t),
			allTextures.GetListCount(),
			allTextures.GetListCount()*sizeof(texture_t),
			allTextures.GetTotalAllocatedTextureSize(),
			sizeof(CTextureManager),
			allTextures.GetListCount()*sizeof(texture_t)+allTextures.GetTotalAllocatedTextureSize()+sizeof(CTextureManager));
	}
	else if (strcmp("change consol key",command)==NULL)
	{ // works however system must check to see if entering key has been released
		bool finished=false, accept=false;
		int device=INPUTMAN_DEVICE_KEYBOARD, key,timeOut;
		while (g_inputMan.GetKeyboardKey()!=-10
			&& g_inputMan.GetMouseKey()!=-10 &&
			((timeOut++)<10000))
			g_inputMan.Update(); // wait for activity to stop
		while (!finished)
		{
			key=g_inputMan.GetMouseKey();
			if(key!=-10)
			{
				device=INPUTMAN_DEVICE_MOUSE;
				finished=true;
			}
			else
			{
				key=g_inputMan.GetKeyboardKey();
				if (key!=-10&&accept)
					finished=true;
				if (key!=DIK_RETURN) // do not accept until key has been released
					accept=true;
			}
		}
		cCommandConsol.EchoPrintF("Key Reset To User Input.");
		g_inputMan.ResetKey(consolB,key,device,true);
	}
	else if (strcmp("exit",command)==NULL)
	{
		consolOn=false;
	}
	else if (strcmp("end",command)==NULL)
	{
		consolOn=false;
		done=true;
	}
	else if (strcmp("",command)==NULL)
	{
	}
	else
	{
		cCommandConsol.EchoPrintF("Command not understood.");
	}
	*bDone=done;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
				   int nShowCmd)
{
	WNDCLASSEX windowClass;
	HWND hwnd;
	MSG msg;
	bool done;
	DWORD dwExStyle;
	DWORD dwStyle;
	RECT windowRect;

	windowRect.left=(long)0;
	windowRect.right=(long)width;
	windowRect.top=(long)0;
	windowRect.bottom=(long)height;

	windowClass.cbSize			= sizeof(WNDCLASSEX);
	windowClass.style			= CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc		= WndProc;
	windowClass.cbClsExtra		= 0;
	windowClass.cbWndExtra		= 0;
	windowClass.hInstance		= hInstance;
	windowClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground	= NULL;
	windowClass.lpszMenuName	= NULL;
	windowClass.lpszClassName	= "MyClass";
	windowClass.hIconSm			= LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&windowClass))
		return 0;

	if (fullScreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize	= sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = width;
		dmScreenSettings.dmPelsHeight = height;
		dmScreenSettings.dmBitsPerPel = bits;
		dmScreenSettings.dmFields= DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) !=
			DISP_CHANGE_SUCCESSFUL)
		{
			MessageBox(NULL, "Display mode failed", NULL, MB_OK);
			fullScreen=FALSE;
		}
	}

	if (fullScreen)
	{
		dwExStyle=WS_EX_APPWINDOW;
		dwStyle=WS_POPUP;
		ShowCursor(FALSE);
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle=WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	hwnd = CreateWindowEx(NULL, "MyClass",
						  "My OpenGL Robot",
						  dwStyle | WS_CLIPCHILDREN |
						  WS_CLIPSIBLINGS,
						  0,0,
						  windowRect.right - windowRect.left,
						  windowRect.bottom - windowRect.top,
						  NULL,
						  NULL,
						  hInstance,
						  NULL);
	if(!hwnd)
		return 0;

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	g_inputMan.Initialize(hwnd,hInstance,true,IS_USEKEYBOARD | IS_USEMOUSE);
	done = false;
	if (!Initialize())
		return 0;
	listBase=CreateBitmapFont("Times New Roman",16);
	const inputManButtonStatus_t* buttonStatus = g_inputMan.GetStatusPointer();
	int endButton = g_inputMan.GetButtonIndex("End"),
		addOneB	  = g_inputMan.GetButtonIndex("Add1"),
		addListB  = g_inputMan.GetButtonIndex("AddList"),
		consolB   = g_inputMan.GetButtonIndex("ConsolOn"),
		cursorX	  = g_inputMan.GetButtonIndex("CursorX"),
		cursorY	  = g_inputMan.GetButtonIndex("CursorY"),
		renderB	  = g_inputMan.GetButtonIndex("Render"),
		mouseB	  = g_inputMan.GetButtonIndex("MouseButton"),
		guiB	  = g_inputMan.GetButtonIndex("GUIToggle");
	const unsigned char *pInputCharacter = g_inputMan.GetTextInputPointer();
	const long double *gameTime = g_inputMan.GetSystemTimePointer();
	long double gameStartTime = *gameTime;
	//CConsol cCommandConsol(g_HDC,listBase);
	CConsol cCommandConsol(listBase,13,13,width,height,&g_cLog);


	void *pixelBuffer=NULL;


	static CSystemTimer sysTimers[4];
	sysTimers[0].Initialize(gameTime);
	sysTimers[1].Initialize(gameTime);
	sysTimers[2].Initialize(gameTime);
	sysTimers[3].Initialize(gameTime);
	sysTimers[3].GetElapsedSeconds();
	char theFileList[1024]={0};
	int theFileListCount = StrUtil_GetFileList(theFileList,"*.bmp",sizeof(theFileList));
	strcat(theFileList,"\n");
	//theFileListCount += StrUtil_GetFileList(theFileList+(strlen(theFileList)),"*.jpg",sizeof(theFileList));
	char theJPGFileList[1024]={0};
	StrUtil_GetFileList(theJPGFileList,"*.jpg",sizeof(theJPGFileList));
	Render(0.0f);
	float delay=0.0f;
	sysTimers[0].GetElapsedSeconds();
	while (delay<1.0f)
	{
		delay+=sysTimers[0].GetElapsedSeconds();
		g_inputMan.Update();
	}/**/
//CGLGUIWINDOWGENERATOR *cWinGen

	CGLGUIWINDOWGENERATOR cWinGen;
	WinGenInitialize(&cWinGen);
	CGLGUIWindow *testWindow=NULL;
	testWindow = GLGUIMakeWindow(0,&cWinGen,NULL,100,200);

	while (!done)
	{
		PeekMessage(&msg, hwnd, NULL, NULL, PM_REMOVE);

		if (msg.message == WM_QUIT || buttonStatus[endButton].buttonValue==1.0f)
		{
			done = true;
		}
		else if (g_isActive)
		{
			if(!consolOn)
			{
				if (buttonStatus[addOneB].buttonValue==1.0f && !addedObject)
				{
					addedObject = true;
					//allTextures.AddTexture(TEXMAN_BMP,"chess.bmp","Chess");
					//allTextures.AddTexture(TEXMAN_BMP_RGBA,"snowstorm.bmp","Chess");
					allTextures.AddTexture(TEXMAN_TGA,"tgatester copy.tga","Chess");
				}
				if (buttonStatus[addListB].buttonValue==1.0f && !addedObjectS)
				{
					addedObjectS = true;
					char *files[] = {"Terrain_.bmp","EYE.bmp","mother_pearl.bmp"};
					char *names[] = {"Rock","Eye","Shell"};
					allTextures.AddTextures(TEXMAN_BMP,3,files,names);/**/
				}
				if (buttonStatus[consolB].buttonValue==1.0f)
					consolOn=true;

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color/depth buffer
				glLoadIdentity();

				g_inputMan.Update();

				if (!g_in3DMode)
				{
					glEnable(GL_TEXTURE_2D);
					glEnable(GL_DEPTH_TEST);

					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();

					gluPerspective(54.0f,(GLfloat)width/(GLfloat)height,1.0f,1000.0f);

					glMatrixMode(GL_MODELVIEW);
					//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color/depth buffer
					glLoadIdentity();
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color/depth buffer
					g_in3DMode=true;
				}

				Render(sysTimers[2].GetElapsedSeconds());
	
				glFlush();
				SwapBuffers(g_HDC);				
			}
			else
			{
				glEnable(GL_TEXTURE_2D);
				glEnable(GL_DEPTH_TEST);

				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();

				gluPerspective(54.0f,(GLfloat)width/(GLfloat)height,1.0f,1000.0f);

				glMatrixMode(GL_MODELVIEW);
				//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color/depth buffer
				glLoadIdentity();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color/depth buffer
				Render(sysTimers[2].GetElapsedSeconds());

				glMatrixMode(GL_PROJECTION);
				//glClear(GL_DEPTH_BUFFER_BIT || GL_COLOR_BUFFER_BIT);
				glLoadIdentity();
				glDisable(GL_DEPTH_TEST);

				// set the perspective with the appropriate aspect ratio
				gluOrtho2D(0, width, 0, height);


				// select modelview matrix
				glMatrixMode(GL_MODELVIEW);
				
				glLoadIdentity();

				g_in3DMode=false;

				static bool consolState = consolOn;
				g_inputMan.Update();

				unsigned char current = INPUTMANConvertTextInputToCharacters(pInputCharacter,g_inputMan.GetCapsLock(),g_inputMan.GetShiftStatus());
				unsigned char send = NULL;
				if (buttonStatus[guiB].buttonValue!=1.0f)
					send = current;
				if (send=='~')
					cCommandConsol.Hide();
				else if (send=='-')
					cCommandConsol.Show();
				const char *command = NULL;
				if (buttonStatus[guiB].buttonValue!=1.0f)
					command=cCommandConsol.UpdateAndRender(send,true);
				if (command!=NULL)				
					ConsolCommandProcessor(cCommandConsol,sysTimers,&done,&command,consolB,
					&pInputCharacter,listBase,&cWinGen);
				if (current==INPUTMAN_TEXT_CODE_F4)
					consolOn=false;
				consolState = consolOn;
				if (buttonStatus[guiB].buttonValue==1.0f)
				{//////////////////////////////////////////////////////////////////GUI/////////////

					///////////////
					
					int cX = (int)(buttonStatus[cursorX].buttonValue*(width-1)),
						cY = (int)(-buttonStatus[cursorY].buttonValue*(height-2))+(height-1);
					{
						const char max=5;
						static int i=0;
						static void *data=NULL;

						if (GLGUIUpdateWin(testWindow,cX,cY,current,(buttonStatus[mouseB].buttonValue==1.0f),sysTimers[3].GetElapsedSeconds(),i,data))
						{
							i++;
							switch (i)
							{
							case 5:
								{
									static GLGUIWIN_SPEECHWINparameters testSpeech;
									testSpeech.textMsg="This Is Why I always say no to you. You always fail on"
										" your end of the bargain. So what now? What do we do now?";
									testSpeech.numOfResponses=4;
									char *responses[]={"Stand here and wait.","Continue on.","Give up.","Uh...",
									"Ureathra! I got it! We could climb over the wall, kill the dragon, open the"
									" gate, pick the lock, and steal the key!"};
									testSpeech.responses=responses;
									data = (void *)&testSpeech;
								}
							default:
								break;
							}
							if (i>max)
								i=0;
							testWindow=GLGUIMakeWindow(i,&cWinGen,data,100,150);
						}
					}

					glEnable(GL_TEXTURE_2D);
					static int arrowIndex = allTextures.FindTextureIndex("Arrow");
					allTextures.SelectTexture(arrowIndex);
					static int arrowWidth=0, arrowHeight=0;
					static bool isAlhpa = (allTextures.GetTexBpp(arrowIndex)>=32);
					if (arrowWidth==0)
					{
						allTextures.GetCurTexDimensions(arrowWidth,arrowHeight);
						/**if (pixelBuffer==NULL)
						{
							pixelBuffer=malloc(arrowWidth*arrowHeight*3);
							memset(pixelBuffer,0,arrowWidth*arrowHeight*3);
						}/**/
					}
					GLGUIRenderCursor(arrowWidth,arrowHeight,cX,cY,pixelBuffer,isAlhpa);

					///////////////
					///
				}//////////////////////////////////////////////////////////////////GUI/////////////
				glFlush();
				SwapBuffers(g_HDC);
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	GLGUIKillWin(testWindow);
	WinGenKill(&cWinGen);
	if (pixelBuffer!=NULL)
		free(pixelBuffer);
	 if (fullScreen)
  {
    ChangeDisplaySettings(NULL, 0);
    ShowCursor(TRUE);
  }
	CleanUp();
	  // release the DC if we have one
  if (g_HDC && !ReleaseDC(hwnd, g_HDC))
  {
    MessageBox(NULL, "Unable to release device context", "Error", MB_OK | MB_ICONINFORMATION);
    g_HDC = NULL;
  }

  // destroy the window if we have a valid handle
  if (hwnd && !DestroyWindow(hwnd))
  {
    MessageBox(NULL, "Unable to destroy window", "Error", MB_OK | MB_ICONINFORMATION);
    hwnd = NULL;
  }

  // unregister our class so we can create a new one if we need to
  if (!UnregisterClass("MyClass", hInstance))
  {
    MessageBox(NULL, "Unable to unregister window class", "Error", MB_OK | MB_ICONINFORMATION);
    hInstance = NULL;
  }
	return msg.wParam;
}

bool GLGUIUpdateWin(CGLGUIWindow *window,int cX, int cY, char currentChar, bool mouseButton,float elapsedTime,int windowType, void *data)
{
	if (currentChar==';')
		int donothing=0;
	if (window!=NULL)
	{
		int okIdx=window->FindElement("OkButton");
		int imgIdx=window->FindElement("Image1");
		if (imgIdx>=0)
		{			
			switch (currentChar)
			{
			case '0':
				window->ChangeElmtData(imgIdx,"TestWindow2Img");
				break;
			case '2':
				window->ChangeElmtData(imgIdx,NULL);
				break;
			case '1':
				window->ChangeElmtData(imgIdx,"TestWindowImg");
				break;
			default:
				break;
			}
		}
		switch (windowType)
		{
		case 3:
			{
				static descBxIdx=window->FindElement("DescriptionBx"),

					   magicBar		=window->FindElement("MagicBar"),
					   magicTxt		=window->FindElement("MagicTxt"),

					   agilityBar	=window->FindElement("AgilityBar"),
					   agilityTxt	=window->FindElement("AgilityTxt"),
					   
					   speedBar		=window->FindElement("SpeedBar"),
					   speedTxt		=window->FindElement("SpeedTxt"),
					   
					   strengthBar	=window->FindElement("StrengthBar"),
					   strengthTxt	=window->FindElement("StrengthTxt"),

					   spiritBar	=window->FindElement("SpiritBar"),
					   spiritTxt	=window->FindElement("SpiritTxt"),
					   
					   vitalityBar	=window->FindElement("VitalityBar"),
					   vitalityTxt	=window->FindElement("VitalityTxt"),
					   
					   pointsBar	=window->FindElement("PointsBar"),
					   pointsTxt	=window->FindElement("PointsTxt"),

					   cancelBtn	=window->FindElement("Cancel"),
					   okButton		=window->FindElement("OkButton");

				const adjustFactor = 4;
				int magicLvl = (window->GetElementStatus(magicBar)*240/SLIDERRENDERER_MAXVALUE)/adjustFactor;
				int agilityLvl = (window->GetElementStatus(agilityBar)*240/SLIDERRENDERER_MAXVALUE)/adjustFactor;
				int speedLvl = (window->GetElementStatus(speedBar)*240/SLIDERRENDERER_MAXVALUE)/adjustFactor;
				int strengthLvl = (window->GetElementStatus(strengthBar)*240/SLIDERRENDERER_MAXVALUE)/adjustFactor;
				int spiritLvl = (window->GetElementStatus(spiritBar)*240/SLIDERRENDERER_MAXVALUE)/adjustFactor;
				int vitalityLvl = (window->GetElementStatus(vitalityBar)*240/SLIDERRENDERER_MAXVALUE)/adjustFactor;
				int pointsLvl = (window->GetElementStatus(pointsBar)*240/SLIDERRENDERER_MAXVALUE)/4;
				bool fix=true;
				bool adjusted=false;
				int tally=magicLvl+agilityLvl+speedLvl+strengthLvl+spiritLvl+vitalityLvl;
				if ((60-(magicLvl+agilityLvl+speedLvl+strengthLvl+spiritLvl+vitalityLvl))!=pointsLvl)
					fix=true;
				pointsLvl=60-(magicLvl+agilityLvl+speedLvl+strengthLvl+spiritLvl+vitalityLvl);
				if (pointsLvl<0)
				{
					fix=true;
					while ((60-(magicLvl+agilityLvl+speedLvl+strengthLvl+spiritLvl+vitalityLvl))<0)
					{
						magicLvl--;
						if (magicLvl<0)
							magicLvl=0;
						if ((60-(magicLvl+agilityLvl+speedLvl+strengthLvl+spiritLvl+vitalityLvl))>=0)
							break;
						agilityLvl--;
						if (agilityLvl<0)
							agilityLvl=0;
						if ((60-(magicLvl+agilityLvl+speedLvl+strengthLvl+spiritLvl+vitalityLvl))>=0)
							break;
						speedLvl--;
						if (speedLvl<0)
							speedLvl=0;
						if ((60-(magicLvl+agilityLvl+speedLvl+strengthLvl+spiritLvl+vitalityLvl))>=0)
							break;
						strengthLvl--;
						if (strengthLvl<0)
							strengthLvl=0;
						if ((60-(magicLvl+agilityLvl+speedLvl+strengthLvl+spiritLvl+vitalityLvl))>=0)
							break;
						spiritLvl--;
						if (spiritLvl<0)
							spiritLvl=0;
						if ((60-(magicLvl+agilityLvl+speedLvl+strengthLvl+spiritLvl+vitalityLvl))>=0)
							break;
						vitalityLvl--;
						if (vitalityLvl<0)
							vitalityLvl=0;
					}
					adjusted=true;
				}
				pointsLvl=60-(magicLvl+agilityLvl+speedLvl+strengthLvl+spiritLvl+vitalityLvl);
				if (fix)
				{
					{
						if (adjusted)
							window->ChangeElmtData(magicBar,(magicLvl*adjustFactor*SLIDERRENDERER_MAXVALUE)/240);
						char buff[20]={0};
						sprintf(buff,"Magic %i",magicLvl);
						window->ChangeElmtData(magicTxt,buff);
					}
					{
						if (adjusted)
							window->ChangeElmtData(agilityBar,(agilityLvl*adjustFactor*SLIDERRENDERER_MAXVALUE)/240);
						char buff[20]={0};
						sprintf(buff,"Agility %i",agilityLvl);
						window->ChangeElmtData(agilityTxt,buff);
					}
					{
						if (adjusted)
							window->ChangeElmtData(speedBar,(speedLvl*adjustFactor*SLIDERRENDERER_MAXVALUE)/240);
						char buff[20]={0};
						sprintf(buff,"Speed %i",speedLvl);
						window->ChangeElmtData(speedTxt,buff);
					}
					{
						if (adjusted)
							window->ChangeElmtData(strengthBar,(strengthLvl*adjustFactor*SLIDERRENDERER_MAXVALUE)/240);
						char buff[20]={0};
						sprintf(buff,"Strength %i",strengthLvl);
						window->ChangeElmtData(strengthTxt,buff);
					}
					{
						if (adjusted)
							window->ChangeElmtData(spiritBar,(spiritLvl*adjustFactor*SLIDERRENDERER_MAXVALUE)/240);
						char buff[20]={0};
						sprintf(buff,"Spirit %i",spiritLvl);
						window->ChangeElmtData(spiritTxt,buff);
					}
					{
						if (adjusted)
							window->ChangeElmtData(vitalityBar,(vitalityLvl*adjustFactor*SLIDERRENDERER_MAXVALUE)/240);
						char buff[20]={0};
						sprintf(buff,"Vitality %i",vitalityLvl);
						window->ChangeElmtData(vitalityTxt,buff);
					}
					{
						window->ChangeElmtData(pointsBar,(pointsLvl*4*SLIDERRENDERER_MAXVALUE)/240);
						char buff[20]={0};
						sprintf(buff,"Points Left %i",pointsLvl);
						window->ChangeElmtData(pointsTxt,buff);
					}
				}
				break;
			}
		default:
			break;
		}
		if (window->UpdateAndRender(currentChar,cX,cY,mouseButton,elapsedTime,true))
			if (window->GetPressedButton()==okIdx||currentChar==INPUTMAN_TEXT_CODE_ENTER)
			{
				// Get Window Data
				///
				GLGUIKillWin(window);
				return true;
			}
	}
	return false;
}
	