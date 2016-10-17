#ifndef __INCLUDED_GLGUIWINLIB__
#define __INCLUDED_GLGUIWINLIB__
#include "InputManager.h"
#include "GLGUIWindowGenerator.h"

bool GLGUIRenderWin(CGLGUIWindow *&window,int windowType, void *data, int x, int y,int screenWidth=-1,int screenHeight=-1);
bool GLGUIInitWinRenderer(CInputManager *cInputMan,CGLGUIWINDOWGENERATOR *cWinGen);

bool GLGUIUpdateWin(CGLGUIWindow *window,int cX, int cY, char currentChar, bool mouseButton,float elapsedTime,int windowType, void *data);
CGLGUIWindow *GLGUIMakeWindow(int windowType, CGLGUIWINDOWGENERATOR *cWinGen,void *data, int x, int y);
void GLGUIKillWin(CGLGUIWindow *&window);

#endif
