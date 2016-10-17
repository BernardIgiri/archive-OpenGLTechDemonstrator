#ifndef __GLGUIWINDOWRENDERER_INCLUDED__
#define __GLGUIWINDOWRENDERER_INCLUDED__
#include "TextFeildRenderer.h"
#include "SelectionListRenderer.h"
#include "ButtonRenderer.h"
#include "SliderRenderer.h"
#include "GUIImageRenderer.h"

#define GLGUI_ELEMENT_TEXTFEILD		0
#define GLGUI_ELEMENT_BUTTON		1
#define GLGUI_ELEMENT_TOGGLEBUTTON	2
#define GLGUI_ELEMENT_SELECTLIST	3
#define GLGUI_ELEMENT_DROPDOWNLIST	4
#define GLGUI_ELEMENT_SLIDER		5
#define GLGUI_ELEMENT_IMAGE			6
#define GLGUI_SCROLLDELAY 0.25f

typedef struct
{
	CTextFeildRenderer *pElement; // set to NULL
	int displayWidth; // display width in chars
	int maxChars; // maximum charecter entry
	int padding; // padding
	char *initialText; // set to NULL if none
	bool drawBackground;
	bool setColor; // set to true for custom colors
	float backgroundColor[3];
	float textColor[3];
} glguiElementTextFeild_t;

typedef struct
{
	CSelectionListRenderer *pElement; // set to NULL
	int displayWidth; // display width in chars
	int displayHeight; // amount of displayed lines
	int scrollUpIndex; // index of button to scroll up -1 if none
	int scrollDownIndex; // index of button to scroll down -1 if none
	int sliderIndex; // index of slider -1 if none
	int padding; // padding
	bool drawBackground;
	bool drawHighlight;
	bool setColor; // set to true for custom colors
	float backgroundColor[3];
	float textColor[3];
	float highlightBackgroundColor[3];
	float highlightTextColor[3];
	char *listText;
} glguiElementSelectList_t;

typedef struct
{
	CSelectionListRenderer *pElement; // set to NULL
	//CButtonRenderer *pTriggerElement; // set to NULL
	int displayWidth; // display width in chars
	int displayHeight; // amount of displayed lines when minimized
	int maxHeight; // amount of displayed lines when down
	int openIndex; // index of opening button set to -1 disable
	int scrollUpIndex; // index of button to scroll up -1 if none
	int scrollDownIndex; // index of button to scroll down -1 if none
	int padding; // padding
	int y2; // ignore, used by system
	int bX; // ignore, used by system
	
/*	char *defaultTexName; // texture for trigger button if trigger is -1
	char *downTexName; // depressed texture for trigger button if trigger is -1*/

	bool drawBackground;
	bool drawHighlight;
	bool setColor; // set to true for custom colors
	float backgroundColor[3];
	float textColor[3];
	float highlightBackgroundColor[3];
	float highlightTextColor[3];
	char *listText;

	bool state; // set to false
	bool oldState; // set to false
} glguiElementDropDownList_t;

typedef struct
{
	CGUIImageRenderer *pElement; // set to NULL
	int width;
	int height;
	char *texName;
} glguiElementImage_t;

typedef struct
{
	CButtonRenderer *pElement; // set to NULL
	int width;
	int height;
	char *buttonText;
	char *defaultTexName;
	char *mouseOverTexName;
	char *mouseDownTexName;
	bool setColor; // set to true for custom colors
	float textColor[3];
	bool status; // true if on or depressed
	float depressedTime; // set to 0.0f
	int selectIndex; // ignore Set by system
} glguiElementButton_t;

typedef struct
{
	CButtonRenderer *pElement; // set to NULL
	int width;
	int height;
	const char *buttonText;
	char *defaultTexName;
	char *OnTexName;
	bool setColor; // set to true for custom colors
	float textColor[3];
	bool status; // set to true if on, else set to false
} glguiElementToggleButton_t;

typedef struct
{
	CSliderRenderer *pElement; // set to NULL
	int width;
	int height;
	bool isHorizontal;
	const char *backTexName;
	const char *buttonTexName;
	int position; // start position of slider
	int selectIndex; // ignore Set by system
} glguiElementSlider_t;

typedef struct
{
	char *elementName; // referencing name for element
	int elementType; // GLGUI_ELEMENT_x TEXTFEILD/SELECTLIST/BUTTON/TOGGLEBUTTON/DROPDOWNLIST/SLIDER
	unsigned int font; // font list base
	int textWidth; // width of text in pixels (for buttons use total display width of string)
	int textHeight; // height text in pixels
	int posY; // Y position relative to the window
	int posX; // X position relative to the window
	bool submitButton; // set true if this button returns an output
	union// glguiElementUnion
	{
		glguiElementTextFeild_t		textFeild; // data for a text feild
		glguiElementSelectList_t	selectList; // data for a select list
		glguiElementDropDownList_t	dropDownList; // data for a drop down list
		glguiElementButton_t		button; // data for a button
		glguiElementToggleButton_t	toggleButton; // data for a toggle button
		glguiElementSlider_t		slider; // data for a slider
		glguiElementImage_t			image; // data for an image
	} element; // element specific data
} glguiElementData_t;

class CGLGUIWindow
{
public:
	CGLGUIWindow(CLog *cLog, CTextureManager *cTexMan, int left,int bottom, int width, int height,
		char *backgroundTexture, int backgroundTileFactor,glguiElementData_t *windowDescription,int numberOfElements,
		int initialSelect=0);
	~CGLGUIWindow();
	bool UpdateAndRender(const char nextCharacter,int cursorX,
						 int cursorY, bool mouseButton, float elapsedTime, bool redrawAll=true);
	bool IsMouseOver(int cursorX, int cursorY);
	int	 FindElement(const char *elmtName);
	bool IsMouseOverElmt(int elmtIndex,int cursorX,int cursorY);
	int	 GetSelectedIndex();
	int	 GetPressedButton();
	int	 GetElementStatus(int elmtIndex);
	const char *GetReturnedString(int elmtIndex);
	bool MoveTo(int x, int y);
	void GetDimensions(int &width, int &height);
	void GetElmtPosition(int elmtIndex, int &x, int &y);
	void GetButtonCenter(int elmtIndex, int &x, int &y);
	void GetImageCoordinates(int elmtIndex, int &left,int &top,int &right,int &bottom);
	void ChangeElmtData(int elmtIndex, const char *string);
	void ChangeElmtData(int elmtIndex, int value);
protected:
	glguiElementData_t *m_elementList;
	CLog			   *m_cLog;
	CTextureManager	   *m_cTexMan;
	int					m_bottom;
	int					m_left;
	int					m_width;
	int					m_height;
	int					m_backTexID;
	int					m_selectedIndex;
	int					m_pressedButton;
	int					m_numOfElements;
	int					m_tileFactor;
	bool				m_redrawAll;
	bool				m_isAlpha;
};
void GLGUIRenderCursor(int cursorWidth,int cursorHeight, int cursorX, int cursorY,void *pxBuffer, bool isAlpha);
void GLGUICreateTextFeild(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int displayWidth, int maxChars,int padding,char *initialText,
						  bool drawBackground,bool setColor,float bgColorR,float bgColorG,float bgColorB,
						  float textColorR,float textColorG,float textColorB);
void GLGUICreateSelectionList(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int displayWidth, int displayHeight,int scrollUpIndex,int scrollDownIndex,
						  int sliderIndex, int padding, bool  drawBackground, bool drawHighlight, bool setColor,
						  float bgColorR,float bgColorG,float bgColorB,
						  float textColorR,float textColorG,float textColorB,
						  float highlightBgColorR,float highlightBgColorG,float highlightBgColorB,
						  float highlightTextColorR,float highlightTextColorG,float highlightTextColorB,
						  char *listText, bool submitButton=false);
void GLGUICreateButton(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition, bool submitButton,///
						  int width, int height, char *buttonText, char *defaultTexName,
						  char *mouseOverTexName, char *mouseDownTexName, bool setColor, float textColorR,
						  float textColorG, float textColorB);
void GLGUICreateDropDownList(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int displayWidth, int defaultHeight, int fullHeight, int openIndex,int scrollUpIndex,int scrollDownIndex,
						  int padding, bool  drawBackground, bool drawHighlight, bool setColor,
						  float bgColorR,float bgColorG,float bgColorB,
						  float textColorR,float textColorG,float textColorB,
						  float highlightBgColorR,float highlightBgColorG,float highlightBgColorB,
						  float highlightTextColorR,float highlightTextColorG,float highlightTextColorB,
						  char *listText, bool submitButton=false);
void GLGUICreateToggleButton(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int width, int height, char *buttonText, char *OffTexName,
						  char *OnTexName, bool setColor, float textColorR,float textColorG, float textColorB,
						  bool defaultCheckStatus);
void GLGUICreateTextDisplay(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  bool setColor,float textColorR,float textColorG,float textColorB,
						  char *text);
void GLGUICreateTextDisplay(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int padding, bool  drawBackground, bool setColor,
						  float bgColorR,float bgColorG,float bgColorB,
						  float textColorR,float textColorG,float textColorB,
						  char *text);
void GLGUICreateSlider(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int width,int height, int sliderPosition,bool isHorizontal,
						  const char *backTexName, const char *buttonTexName);//SLIDERRENDERER_MAXVALUE
void GLGUICreateImage(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int width,int height, const char *texName);
void GLGUICoordinateCalculator(int &left, int &top, int &right, int &bottom,glguiElementData_t *windowDescriptor);
int	 GLGUITextDispWidthEstCalculator(char *string, int maxTextWidth);
#endif