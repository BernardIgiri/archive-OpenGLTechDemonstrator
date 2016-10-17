/****************************************************************************
 input.cpp
 
 Program demonstrating the use of DirectInput for keyboard and mouse input.
  
 Author   :   Dave Astle
 Date     :   4/6/2001

 Written for OpenGL Game Programming
*****************************************************************************/


/********************************* Includes *********************************/
#define WIN32_LEAN_AND_MEAN   // get rid of Windows things we don't need

#include <windows.h>          // included in all Windows apps
#include <winuser.h>          // Windows constants
#include <gl/gl.h>            // OpenGL include
#include <gl/glu.h>           // OpenGL utilty library
#include "InputSystem.h"
#include "bitmap.h"
#include "InputManager.h"
#include "StringUtilities.h"
#include "TextInputSystem.h"
/**/
/**/


#define WND_CLASS_NAME  "OpenGL Window Class"


/*************************** Constants and Macros ***************************/
const int   SCREEN_WIDTH    = 1024;
const int   SCREEN_HEIGHT   = 768;
const int   SCREEN_BPP      = 24;
const bool  USE_FULLSCREEN  = true; 
const char  *APP_TITLE       = "Input Demo";

#define NUM_TEXTURES  4
#define CURSOR_SIZE   32


/********************************* Globals **********************************/
HDC       g_hdc;                  // device context
HGLRC     g_hrc;                  // rendering context
BOOL      g_isFullscreen = TRUE;  // toggles fullscreen and windowed display
BOOL      g_isActive = TRUE;      // false if window is minimized
HWND      g_hwnd = NULL;          // main window handle
HINSTANCE g_hInstance;            // application instance


// input variables
CInputSystem  g_input;
CInputManager g_inputMan;

int     g_mouseX        = SCREEN_WIDTH / 2;
int     g_mouseY        = SCREEN_HEIGHT / 2;
GLuint  g_mouseTexture;
int     g_textureIndex  = 0;
char    g_lastKey[84]    = "F1";

int     g_screenWidth   = SCREEN_WIDTH;
int     g_screenHeight  = SCREEN_HEIGHT;

unsigned int g_listBase = 0;


/******************************** Prototypes ********************************/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL    SetupWindow(const char *title, int width, int height, int bits, bool isFullscreen);
BOOL    KillWindow();

GLvoid  ResizeScene(GLsizei width, GLsizei height);
BOOL    InitializeScene();
BOOL    DisplayScene();
BOOL    Cleanup();
void    ProcessInput();
void    DrawMouseCursor();

void    LoadTexture(char *filename, GLuint &texture);

unsigned int CreateBitmapFont(char *fontName, int fontSize);
void    ClearFont(unsigned int base);
void    PrintString(unsigned int base, char *msg);


/*****************************************************************************
 WinMain()

 Windows entry point
*****************************************************************************/
int WINAPI WinMain(HINSTANCE g_hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  MSG   msg;       // message
  BOOL  isDone;    // flag indicatingen the app is done

  // if the window is set up correctly, we can proceed with the message loop
  if (SetupWindow(APP_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, USE_FULLSCREEN))
    isDone = FALSE;
  // otherwise, we need to never enter the loop and proceed to exit
  else
    isDone = TRUE;

  // main message loop
  while (!isDone)
  {
    if(PeekMessage(&msg, g_hwnd, NULL, NULL, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)   // do we receive a WM_QUIT message?
      {
        isDone = TRUE;              // if so, time to quit the application
      }
      else
      {
        TranslateMessage(&msg);     // translate and dispatch to event queue
        DispatchMessage(&msg);
      }
    }

    // don't update the scene if the app is minimized
    if (g_isActive)
    {
      // update the scene every time through the loop
      DisplayScene();
      // switch the front and back buffers to display the updated scene
      SwapBuffers(g_hdc);
    }
  }

  Cleanup();
  KillWindow();

  return msg.wParam;
} // end WinMain()


/*****************************************************************************
 WndProc()

 Windows message handler
*****************************************************************************/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message)
  {
  case WM_ACTIVATE:  // watch for the window being minimized and restored
    {
      if (!HIWORD(wParam))
      {
        // program was restored or maximized
        g_isActive = TRUE;
        g_input.AcquireAll();
		g_inputMan.m_input.AcquireAll();
      }
      else
      {
        // program was minimized
        g_isActive=FALSE;
        g_input.UnacquireAll();
		g_inputMan.m_input.UnacquireAll();
      }

      return 0;
    }

  case WM_SYSCOMMAND:  // look for screensavers and powersave mode
    {
      switch (wParam)
      {
      case SC_SCREENSAVE:     // screensaver trying to start
      case SC_MONITORPOWER:   // monitor going to powersave mode
        // returning 0 prevents either from happening
        return 0;
      default:
        break;
      }
    } break;

  case WM_CLOSE:    // window is being closed
    {
      // send WM_QUIT to message queue
      PostQuitMessage(0);

      return 0;
    }

  case WM_SIZE:
    {
      // update perspective with new width and height
      ResizeScene(LOWORD(lParam), HIWORD(lParam));
      return 0;
    }

  case WM_CHAR:
    {
      switch (toupper(wParam))
      {
      case VK_ESCAPE:
        {
          // send WM_QUIT to message queue
          PostQuitMessage(0);
          return 0;
        }
      default:
        break;
      };
    } break;

  default:
    break;
  }

  return (DefWindowProc(hwnd, message, wParam, lParam));
} // end WndProc()


/*****************************************************************************
 SetupWindow()

 Create the window and everything else we need, including the device and
 rendering context. If a fullscreen window has been requested but can't be
 created, the user will be prompted to attempt windowed mode. Finally,
 InitializeScene is called for application-specific setup.

 Returns TRUE if everything goes well, or FALSE if an unrecoverable error
 occurs. Note that if this is called twice within a program, KillWindow needs
 to be called before subsequent calls to SetupWindow.
*****************************************************************************/
BOOL SetupWindow(const char *title, int width, int height, int bits, bool isFullscreen)
{
  // set the global flag
  g_isFullscreen = isFullscreen;

  // get our instance handle
  g_hInstance = GetModuleHandle(NULL);

  WNDCLASSEX  wc;    // window class

  // fill out the window class structure
  wc.cbSize         = sizeof(WNDCLASSEX);
  wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc    = WndProc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = 0;
  wc.hInstance      = g_hInstance;
  wc.hIcon          = LoadIcon(NULL, IDI_APPLICATION);  // default icon
  wc.hIconSm        = LoadIcon(NULL, IDI_WINLOGO);      // windows logo small icon
  wc.hCursor        = LoadCursor(NULL, IDC_ARROW);      // default arrow
  wc.hbrBackground  = NULL;     // no background needed
  wc.lpszMenuName   = NULL;     // no menu
  wc.lpszClassName  = WND_CLASS_NAME;
  
  // register the windows class
  if (!RegisterClassEx(&wc))
  {
    MessageBox(NULL,"Unable to register the window class", "Error", MB_OK | MB_ICONEXCLAMATION);

    // exit and return FALSE
    return FALSE;       
  }

  // if we're in fullscreen mode, set the display up for it
  if (g_isFullscreen)
  {
    // set up the device mode structure
    DEVMODE screenSettings;
    memset(&screenSettings,0,sizeof(screenSettings));

    screenSettings.dmSize       = sizeof(screenSettings);
    screenSettings.dmPelsWidth  = width;    // screen width
    screenSettings.dmPelsHeight = height;   // screen height
    screenSettings.dmBitsPerPel = bits;     // bits per pixel
    screenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // attempt to switch to the resolution and bit depth we've selected
    if (ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
    {
      // if we can't get fullscreen, let them choose to quit or try windowed mode
      if (MessageBox(NULL, "Cannot run in the fullscreen mode at the selected resolution\n"
                           "on your video card. Try windowed mode instead?",
                           "OpenGL Game Programming",
                           MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
      {
        g_isFullscreen = FALSE;
      }
      else
      {
        return FALSE;
      }
    }
  }

  DWORD dwExStyle;
  DWORD dwStyle;

  // set the window style appropriately, depending on whether we're in fullscreen mode
  if (g_isFullscreen)
  {
    dwExStyle = WS_EX_APPWINDOW;
    dwStyle = WS_POPUP;           // simple window with no borders or title bar
    ShowCursor(FALSE);            // hide the cursor for now
  }
  else
  {
    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    dwStyle = WS_OVERLAPPEDWINDOW;
  }

  // set up the window we're rendering to so that the top left corner is at (0,0)
  // and the bottom right corner is (height,width)
  RECT  windowRect;
  windowRect.left = 0;
  windowRect.right = (LONG) width;
  windowRect.top = 0;
  windowRect.bottom = (LONG) height;

  // change the size of the rect to account for borders, etc. set by the style
  AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

  // class registered, so now create our window
  g_hwnd = CreateWindowEx(dwExStyle,          // extended style
                          WND_CLASS_NAME,     // class name
                          title,              // app name
                          dwStyle |           // window style
                          WS_CLIPCHILDREN |   // required for
                          WS_CLIPSIBLINGS,    // using OpenGL
                          0, 0,               // x,y coordinate
                          windowRect.right - windowRect.left, // width
                          windowRect.bottom - windowRect.top, // height
                          NULL,               // handle to parent
                          NULL,               // handle to menu
                          g_hInstance,        // application instance
                          NULL);              // no extra params

  // see if our window handle is valid
  if (!g_hwnd)
  {
    MessageBox(NULL, "Unable to create window", "Error", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  // get a device context
  if (!(g_hdc = GetDC(g_hwnd)))
  {
    MessageBox(NULL,"Unable to create device context", "Error", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  // set the pixel format we want
  PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),  // size of structure
    1,                              // default version
    PFD_DRAW_TO_WINDOW |            // window drawing support
    PFD_SUPPORT_OPENGL |            // OpenGL support
    PFD_DOUBLEBUFFER,               // double buffering support
    PFD_TYPE_RGBA,                  // RGBA color mode
    bits,                           // 32 bit color mode
    0, 0, 0, 0, 0, 0,               // ignore color bits, non-palettized mode
    0,                              // no alpha buffer
    0,                              // ignore shift bit
    0,                              // no accumulation buffer
    0, 0, 0, 0,                     // ignore accumulation bits
    16,                             // 16 bit z-buffer size
    0,                              // no stencil buffer
    0,                              // no auxiliary buffer
    PFD_MAIN_PLANE,                 // main drawing plane
    0,                              // reserved
    0, 0, 0 };                      // layer masks ignored
      
  GLuint  pixelFormat;

  // choose best matching pixel format
  if (!(pixelFormat = ChoosePixelFormat(g_hdc, &pfd)))
  {
    MessageBox(NULL, "Can't find an appropriate pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  // set pixel format to device context
  if(!SetPixelFormat(g_hdc, pixelFormat,&pfd))
  {
    MessageBox(NULL, "Unable to set pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  // create the OpenGL rendering context
  if (!(g_hrc = wglCreateContext(g_hdc)))
  {
    MessageBox(NULL, "Unable to create OpenGL rendering context", "Error",MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  // now make the rendering context the active one
  if(!wglMakeCurrent(g_hdc, g_hrc))
  {
    MessageBox(NULL,"Unable to activate OpenGL rendering context", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  // show the window in the forground, and set the keyboard focus to it
  ShowWindow(g_hwnd, SW_SHOW);
  SetForegroundWindow(g_hwnd);
  SetFocus(g_hwnd);

  // set up the perspective for the current screen size
  ResizeScene(width, height);

  // do one-time initialization
  if (!InitializeScene())
  {
    MessageBox(NULL, "Initialization failed", "Error", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  return TRUE;
} // end SetupWindow()


/*****************************************************************************
 KillWindow()

 Deletes the DC, RC, and Window, and restores the original display.
*****************************************************************************/
BOOL KillWindow()
{
  // restore the original display if we're in fullscreen mode
  if (g_isFullscreen)
  {
    ChangeDisplaySettings(NULL, 0);
    ShowCursor(TRUE);
  }

  // if we have an RC, release it
  if (g_hrc)
  {
    // release the RC
    if (!wglMakeCurrent(NULL,NULL))
    {
      MessageBox(NULL, "Unable to release rendering context", "Error", MB_OK | MB_ICONINFORMATION);
    }

    // delete the RC
    if (!wglDeleteContext(g_hrc))
    {
      MessageBox(NULL, "Unable to delete rendering context", "Error", MB_OK | MB_ICONINFORMATION);
    }

    g_hrc = NULL;
  }

  // release the DC if we have one
  if (g_hdc && !ReleaseDC(g_hwnd, g_hdc))
  {
    MessageBox(NULL, "Unable to release device context", "Error", MB_OK | MB_ICONINFORMATION);
    g_hdc = NULL;
  }

  // destroy the window if we have a valid handle
  if (g_hwnd && !DestroyWindow(g_hwnd))
  {
    MessageBox(NULL, "Unable to destroy window", "Error", MB_OK | MB_ICONINFORMATION);
    g_hwnd = NULL;
  }

  // unregister our class so we can create a new one if we need to
  if (!UnregisterClass(WND_CLASS_NAME, g_hInstance))
  {
    MessageBox(NULL, "Unable to unregister window class", "Error", MB_OK | MB_ICONINFORMATION);
    g_hInstance = NULL;
  }

  return TRUE;
} // end KillWindow()


/*****************************************************************************
 ResizeScene()

 Called once when the application starts and again every time the window is
 resized by the user.
*****************************************************************************/
GLvoid ResizeScene(GLsizei width, GLsizei height)
{
  // avoid divide by zero
  if (height==0)  
  {
    height=1;
  }

  g_screenWidth = width;
  g_screenHeight = height;

  // set the viewport to the new dimensions
  glViewport(0, 0, width, height);

  // select the projection matrix and clear it out
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // set the perspective with the appropriate aspect ratio
  gluOrtho2D(0, g_screenWidth, 0, g_screenHeight);

  // select modelview matrix
  glMatrixMode(GL_MODELVIEW);
} // end ResizeScene()


/****************
*****************/
bool InitializeButtons()
{
	inputManButton_t gameButtons[7];

	char *pushButtonNames[]={"Jump","Left","Right"};
	int pushButtonKeys[]={DIK_J,DIK_H,DIK_K};
	int i = INPUTMANGenerateStandardButtons(gameButtons,0,3,pushButtonNames,pushButtonKeys,
		INPUTMAN_TYPE_BUTTON,INPUTMAN_DEVICE_KEYBOARD,true,false,1);/**/
	/*char *pushButtonNames[]={"Jump","Left","Right"};
	int pushButtonKeys[]={INPUTMAN_MOUSE_WHEEL_UP,INPUTMAN_MOUSE_WHEEL_DOWN,INPUTMAN_MOUSE_BUTTON};
	int i = INPUTMANGenerateStandardButtons(gameButtons,0,3,pushButtonNames,pushButtonKeys,
		INPUTMAN_TYPE_BUTTON,INPUTMAN_DEVICE_MOUSE,true,false,1);*/

	i = INPUTMANGenerateStandardButtons(gameButtons,i,"Booster",DIK_U,INPUTMAN_TYPE_TOGGLE,
		INPUTMAN_DEVICE_KEYBOARD, true, false, 1);

	i = INPUTMANGenerateStandardButtons(gameButtons,i,"End",DIK_X,INPUTMAN_TYPE_TERMINATE,
		INPUTMAN_DEVICE_KEYBOARD, true, false, 1);

	char *analogButtonNames[]={"CursorX","CursorY"};
	int analogButtonKeys[]={INPUTMAN_MOUSE_X,INPUTMAN_MOUSE_Y};
	int analogButtonMaxes[]={1000,1000};//g_screenWidth-1,g_screenHeight-1
	i = INPUTMANGenerateStandardButtons(gameButtons,i,2,analogButtonNames,analogButtonKeys,
		INPUTMAN_TYPE_ANALOG,INPUTMAN_DEVICE_MOUSE,true,false,analogButtonMaxes);

	bool success = g_inputMan.CreateButtons(i,gameButtons);
	if (success)
		g_inputMan.SetButtonXYCoordinate(g_inputMan.GetButtonIndex("CursorX"),g_inputMan.GetButtonIndex("CursorY"),g_screenWidth/2,g_screenHeight/2);
	return success;

}

/*****************************************************************************
 InitializeScene()

 Performs one-time application-specific setup. Returns FALSE on any failure.
*****************************************************************************/
BOOL InitializeScene()
{
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  LoadTexture("arrows.bmp", g_mouseTexture);
  g_input.Initialize(g_hwnd, g_hInstance, true, IS_USEKEYBOARD | IS_USEMOUSE);
  g_inputMan.Initialize(g_hwnd, g_hInstance, true, IS_USEKEYBOARD | IS_USEMOUSE);

  g_listBase = CreateBitmapFont("Times New Roman", 12);

  return InitializeButtons();//TRUE;
} // end InitializeScene()


/*****************************************************************************
 DisplayScene()

 The work of the application is done here. This is called every frame, and
 handles the actual rendering of the scene.
*****************************************************************************/
BOOL DisplayScene()
{
  glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // update and process input data
  g_input.Update();
  g_inputMan.Update();
  ProcessInput();

  // draw text
  glTranslatef(0.0, 0.0, -1.0);
	glColor3f(0.0f, 0.66f, 0.99f);
	//glRasterPos2i(g_screenWidth/2 - 100, g_screenHeight/2 - 50);
	glRasterPos2i(g_screenWidth/2 - 120, g_screenHeight/2 - 50);
	PrintString(g_listBase, g_lastKey);
	{
	static const inputManButtonStatus_t* buttonStatus = g_inputMan.GetStatusPointer();
	static int buttonKill = g_inputMan.GetButtonIndex("End"),
			   cursorX	  = g_inputMan.GetButtonIndex("CursorX"),
			   cursorY	  = g_inputMan.GetButtonIndex("CursorY");
	static const unsigned char *receivedText = g_inputMan.GetTextInputPointer();
	static unsigned char last = NULL;
	static char inString[30]={0};
	static int position = 0;
	static CStringList outputList;
	static CTextInputSystem textEntry(100,40,false);
	static bool initSuccess = outputList.Initialize(), firstTime=true;
	static const char *enteredText = textEntry.GetDisplayString();
	static const char *cursorString= textEntry.GetCursorString();
	static int maxCharacterInput = textEntry.GetMaxCharacterInput();
	//static char *textBuffer=new char[maxCharacterInput+1];/**/
	char textBuffer[100]={0};
	char string[100];
	unsigned char current = INPUTMANConvertTextInputToCharacters(receivedText,g_inputMan.GetCapsLock(),g_inputMan.GetShiftStatus());
	textEntry.ReadText(current);
	sprintf(string,"MouseCode %i X %02.2f Y %02.2f Current Char %03i;  Last Char %3i",g_inputMan.GetMouseKey(),buttonStatus[cursorX].buttonValue,buttonStatus[cursorY].buttonValue,(int)current, (int)last);
	if (current!=NULL)
	{
		last=current;
		if (current>=32 && current<=126)
			inString[position++]=current;
	}
	if (current==INPUTMAN_TEXT_CODE_TAB)
	{
		StrUtil_DeleteCharacter(inString,0);
		if (position>0)
			position--;
	}
	if (current==INPUTMAN_TEXT_CODE_SHIFTTAB)
		StrUtil_InsertCharacter(inString,'|',4);
	//ouputList
	if (firstTime)
		outputList.AddStringToList(string);
	else
		outputList.ChangeString(0,string);

	if (strlen(inString)>29)
	{
		position=0;
		for(int i=0;i<30;i++)
			inString[i]=NULL;
	}
	if (firstTime)
		outputList.AddStringToList(inString);
	else
		outputList.ChangeString(1,inString);
	char **outList=outputList.GetList();
	glRasterPos2i(g_screenWidth/2 - 120, g_screenHeight/2 - 65);
	PrintString(g_listBase, outList[0]);
	glRasterPos2i(g_screenWidth/2 - 120, g_screenHeight/2 - 80);
	PrintString(g_listBase, outList[1]);

	glRasterPos2i(g_screenWidth/2 - 120, g_screenHeight/2 - 95);
	strcpy(textBuffer,enteredText);
	PrintString(g_listBase,textBuffer);
	glRasterPos2i(g_screenWidth/2 - 120, g_screenHeight/2 - 110);
	strcpy(textBuffer,cursorString);
	PrintString(g_listBase,textBuffer);	
	firstTime=false;
	}
  DrawMouseCursor();

  return TRUE;
} // end DisplayScene()


/*****************************************************************************
 Cleanup()

 Called at the end of successful program execution.
*****************************************************************************/
BOOL Cleanup()
{
  g_input.Shutdown();

	ClearFont(g_listBase);

  glDeleteTextures(1, &g_mouseTexture);
  return TRUE;
} // end Cleanup()


/****************************************************************************
 LoadTexture()

 Loads the texture from the specified file and stores it in iTexture.
*****************************************************************************/
void LoadTexture(char *filename, GLuint &texture)
{
  // get a texture object
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // load the bitmap
  BITMAPINFOHEADER bitmapInfoHeader;
  unsigned char *buffer = LoadBitmapFileWithAlpha(filename, &bitmapInfoHeader);

  // set up the texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

  // we're done with the bitmap data
  free(buffer);
} // end LoadTexture()



void ProcessInput2(char *buffer)
{
	static const inputManButtonStatus_t* buttonStatus = g_inputMan.GetStatusPointer();
	static int buttonJump = g_inputMan.GetButtonIndex("Jump"),
			   buttonLeft = g_inputMan.GetButtonIndex("Left"),
			   buttonRight = g_inputMan.GetButtonIndex("Right"),
			   buttonBooster = g_inputMan.GetButtonIndex("Booster");
	int i = 0;
	if (buttonStatus[buttonJump].buttonValue)
		i =sprintf(buffer,"Jump for %.1f seconds",buttonStatus[buttonJump].secondsHeld);
	else
		i+=sprintf(buffer+i,"Jump %.1f seconds ago.",buttonStatus[buttonJump].secondsInactive);

	
	if (buttonStatus[buttonBooster].buttonValue)
		i+=sprintf(buffer+i,"Booster for %.1f seconds",buttonStatus[buttonBooster].secondsHeld);
	else
		i+=sprintf(buffer+i,"Booster %.1f seconds ago.",buttonStatus[buttonBooster].secondsInactive);

	if (buttonStatus[buttonLeft].buttonValue)
		i+=sprintf(buffer+i,"Left for %.1f seconds",buttonStatus[buttonLeft].secondsHeld);
	else
		i+=sprintf(buffer+i,"Left %.1f seconds ago.",buttonStatus[buttonLeft].secondsInactive);

	if (buttonStatus[buttonRight].buttonValue)
		i+=sprintf(buffer+i,"Right for %.1f seconds",buttonStatus[buttonRight].secondsHeld);
	else
		i+=sprintf(buffer+i,"Right %.1f seconds ago.",buttonStatus[buttonRight].secondsInactive);

}


/*****************************************************************************
 ProcessInput()

 Reads data from the input system and updates globals accordingly
*****************************************************************************/
void ProcessInput()
{
  static bool leftButtonDown = false;
  static bool rightButtonDown = false;

  // rotate through mouse cursors on mouseclicks or wheel movement
  if (g_input.ButtonDown(0))
    leftButtonDown = true;

  if (g_input.ButtonDown(1))
    rightButtonDown = true;

  if (g_input.GetMouseWheelMovement() < 0 || (leftButtonDown && g_input.ButtonUp(0)))
  {
    leftButtonDown = false;
    g_textureIndex--;
    if (g_textureIndex < 0)
      g_textureIndex = NUM_TEXTURES - 1;
  }

  if (g_input.GetMouseWheelMovement() > 0 || (rightButtonDown && g_input.ButtonUp(1)))
  {
    rightButtonDown = false;
    g_textureIndex++;
    if (g_textureIndex == NUM_TEXTURES)
      g_textureIndex = 0;
  }

  // update the mouse position
  int dx, dy;
  g_input.GetMouseMovement(dx, dy);

  // keep the cursor within the window
  g_mouseX += dx;
  if (g_mouseX >= g_screenWidth)
    g_mouseX = g_screenWidth - 1;
  if (g_mouseX < 0)
    g_mouseX = 0;
  g_mouseY -= dy;
  if (g_mouseY >= g_screenHeight)
    g_mouseY = g_screenHeight - 1;
  if (g_mouseY < 0)
    g_mouseY = 0;

  // check to see if one of the function keys was pressed
  if (g_input.KeyDown(DIK_F1))
    strcpy(g_lastKey, "F1");
  if (g_input.KeyDown(DIK_F2))
    strcpy(g_lastKey, "F2");
  if (g_input.KeyDown(DIK_F3))
    strcpy(g_lastKey, "F3");
  if (g_input.KeyDown(DIK_F4))
    strcpy(g_lastKey, "F4");
  if (g_input.KeyDown(DIK_F5))
    strcpy(g_lastKey, "F5");
  if (g_input.KeyDown(DIK_F6))
    strcpy(g_lastKey, "F6");
  if (g_input.KeyDown(DIK_F7))
    strcpy(g_lastKey, "F7");
  if (g_input.KeyDown(DIK_F8))
    strcpy(g_lastKey, "F8");
  if (g_input.KeyDown(DIK_F9))
    strcpy(g_lastKey, "F9");
  if (g_input.KeyDown(DIK_F10))
    strcpy(g_lastKey, "F10");
  if (g_input.KeyDown(DIK_F11))
    strcpy(g_lastKey, "F11");
  if (g_input.KeyDown(DIK_F12))
    strcpy(g_lastKey, "F12");
  char buffer[100]={0};
  ProcessInput2(buffer);
  //strcpy(g_lastKey+2," 2.7");
  buffer[82]=NULL;
  strcpy(g_lastKey+2,buffer);
  // check for the exit key
  if (g_input.KeyDown(DIK_ESCAPE))
    PostQuitMessage(0);

} // end ProcessInput()


/*****************************************************************************
 DrawMouseCursor()

 Draw the mouse cursor as a textured quad.
*****************************************************************************/
void DrawMouseCursor()
{
  // determine the starting mouse texture u coordinate
  float texStart = (float)g_textureIndex/(float)NUM_TEXTURES;

  // make sure the transparent portions of the cursor aren't shown
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0);

  // draw the cursor
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0, 1.0, 1.0);
  glBindTexture(GL_TEXTURE_2D, g_mouseTexture);
  glBegin(GL_QUADS);
    glTexCoord2f(texStart, 0.0);
    glVertex2i(g_mouseX, g_mouseY - CURSOR_SIZE);
    glTexCoord2f(texStart + 1.0f/NUM_TEXTURES, 0.0);
    glVertex2i(g_mouseX + CURSOR_SIZE, g_mouseY - CURSOR_SIZE);
    glTexCoord2f(texStart + 1.0f/NUM_TEXTURES, 1.0);
    glVertex2i(g_mouseX + CURSOR_SIZE, g_mouseY);
    glTexCoord2f(texStart, 1.0);
    glVertex2i(g_mouseX, g_mouseY);
  glEnd();
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_TEXTURE_2D);
} // end DrawMouseCursor()


/*****************************************************************************
 CreateBitmapFont()

 Use native wgl support to create display list based fonts
*****************************************************************************/
unsigned int CreateBitmapFont(char *fontName, int fontSize)
{
	HFONT hFont;         // windows font
	unsigned int base;

	base = glGenLists(96);      // create storage for 96 characters

	if (stricmp(fontName, "symbol") == 0)
	{
    hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, SYMBOL_CHARSET, 
                       OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                       FF_DONTCARE | DEFAULT_PITCH, fontName);
	}
	else
	{
    hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 
                       OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                       FF_DONTCARE | DEFAULT_PITCH, fontName);
	}

	if (!hFont)
		return 0;

	SelectObject(g_hdc, hFont);
	wglUseFontBitmaps(g_hdc, 32, 96, base);

	return base;
} // end CreateBitmapFont


/*****************************************************************************
 ClearFont()

 Free the display lists for the font
*****************************************************************************/
void ClearFont(unsigned int base)
{
	if (base != 0)
		glDeleteLists(base, 96);
} // end ClearFont


/*****************************************************************************
 PrintString()

 Display the text in msg at the current raster position
*****************************************************************************/
void PrintString(unsigned int base, char *msg)
{
	if ((base == 0) || (msg == NULL))
		return;

	glPushAttrib(GL_LIST_BIT);
		glListBase(base - 32);
		glCallLists(strlen(msg), GL_UNSIGNED_BYTE, msg);
	glPopAttrib();
} // end PrintString()


