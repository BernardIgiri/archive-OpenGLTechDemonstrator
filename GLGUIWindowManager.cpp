#include "GLGUIWindowManager.h"
#include "SystemTimer.h"

typedef struct
{
	CInputManager *cInputMan;
	CGLGUIWINDOWGENERATOR *cWinGen;
} GLGGUISWINITDATA_t;

bool GLGUIRenderWin(CGLGUIWindow *&window,int windowType, void *data, int x, int y, int screenWidth, int screenHeight)
{
	static width=640,height=480;
	if (screenWidth>0)
		width = screenWidth;
	if (screenHeight>0)
		height = screenHeight;
	static bool intialized=false;
	static GLGGUISWINITDATA_t winData;
	static const inputManButtonStatus_t* buttonStatus = NULL;
	static int cursorX = 0,cursorY = 0,mouseB  = 0;
	static const unsigned char *textInputChar = NULL;
	static CSystemTimer timer;
	if (!intialized)
	{
		if (window==NULL&&windowType==-1&&data!=NULL&&x==0&&y==0)
		{
			memcpy(&winData,data,sizeof(GLGGUISWINITDATA_t));
			buttonStatus = winData.cInputMan->GetStatusPointer();
			cursorX	  = winData.cInputMan->GetButtonIndex("CursorX");
			cursorY	  = winData.cInputMan->GetButtonIndex("CursorY");
			mouseB	  = winData.cInputMan->GetButtonIndex("MouseButton");
			textInputChar = winData.cInputMan->GetTextInputPointer();
			intialized=timer.Initialize(winData.cInputMan->GetSystemTimePointer());
		}
		return false;
	}
	if (window!=NULL)
	{
		unsigned char current = INPUTMANConvertTextInputToCharacters(textInputChar,winData.cInputMan->GetCapsLock(),winData.cInputMan->GetShiftStatus());
		int cX = (int)(buttonStatus[cursorX].buttonValue*(width-1)),
			cY = (int)(-buttonStatus[cursorY].buttonValue*(height-2))+(height-1);
		bool result = GLGUIUpdateWin(window,cX,cY,current,(buttonStatus[mouseB].buttonValue==1.0f),timer.GetElapsedSeconds(),windowType,data);
		if (result)
			int donothing=0;
		return result;
	}
	else
	{
		window=GLGUIMakeWindow(windowType,winData.cWinGen,data,x,y);
	}
	return false;
}

bool GLGUIInitWinRenderer(CInputManager *cInputMan,CGLGUIWINDOWGENERATOR *cWinGen)
{
	if (cInputMan==NULL||cWinGen==NULL)
		return false;
	GLGGUISWINITDATA_t winData;
	winData.cInputMan=cInputMan;
	winData.cWinGen=cWinGen;
	CGLGUIWindow *window=NULL;
	return GLGUIRenderWin(window,-1,&winData,0,0);
}


