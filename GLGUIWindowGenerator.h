#ifndef __INCLUDED_GLGUIWINDOWGENERATOR__
#define __INCLUDED_GLGUIWINDOWGENERATOR__
#include "clist.h"
#include "GLGUIWindowRenderer.h"

#ifndef __INCLUDED_GLGUI_ALIGNMENTS__
#define __INCLUDED_GLGUI_ALIGNMENTS__
#define GLGUI_ALIGNCENTER	0
#define GLGUI_ALIGNRIGHT	1
#define GLGUI_ALIGNLEFT		2
#endif
#ifndef __INCLUDED_GLGUI_FLAGS__
#define __INCLUDED_GLGUI_FLAGS__
#define GLGUI_FLAGWIDE				1
#define GLGUI_FLAGTALL				2
#define GLGUI_FLAGNOBACKGROUND		4
#define GLGUI_FLAGSUBMITBUTTON		8
#define GLGUI_FLAGISCHECKED			16
#endif

#define GLGUIWINDOWGENERATOR_MAXROWS 100

typedef struct
{
	char *windowStyle;
	char *font;
	char *colorScheme;
	char *settings;
	char *settingsName;
} GLGUIWINDOWGENERATORfullSettings_t;

typedef struct
{
	int topBorderWidth;
	int leftBorderWidth;
	int rightBorderWidth;
	int bottomBorderWidth;
	int tileFactor;
	int texture;
	char *styleName;
} GLGUIWINDOWGENERATORwindowStyle_t;

typedef struct
{
	int textWidth;
	int textHeight;
	int fontListBase;
	char *fontName;
} GLGUIWINDOWGENERATORfont_t;

typedef struct
{
	float backR;
	float backG;
	float backB;
	float textR;
	float textG;
	float textB;
	float hBackR;
	float hBackG;
	float hBackB;
	float hTextR;
	float hTextG;
	float hTextB;

	float bTextR;
	float bTextG;
	float bTextB;
	char *schemeName;
} GLGUIWINDOWGENERATORcolorScheme_t;

typedef struct
{
	CTextureManager *cTexMan;

	int shortScrollBoxHeight;
	int tallScrollBoxHeight;
	int narrowScrollBoxWidth;
	int wideScrollBoxWidth;
	int scrollBoxPadding;
	int dropDownHeight;
	int dropDownPadding;
	int textFeildPadding;
	int openButtonWidth;
	int openButtonHeight;
	int scrollButtonWidth;
	int scrollButtonHeight;
	int topScrollButtonWidth;
	int topScrollButtonHeight;
	int standardButtonWidth;
	int standardButtonHeight;

	int openButtonMDefTex;
	int openButtonMOverTex;
	int openButtonMDownTex;

	int scrollUpButtonMDefTex;
	int scrollUpButtonMOverTex;
	int scrollUpButtonMDownTex;

	int scrollDownButtonMDefTex;
	int scrollDownButtonMOverTex;
	int scrollDownButtonMDownTex;

	int defaultButtonMDefTex;
	int defaultButtonMOverTex;
	int defaultButtonMDownTex;

	int checkBoxOnTex;
	int checkBoxOffTex;

	int sliderBackTexV;
	int sliderBackTexH;
	int sliderButtonTex;

	int alignment; //GLGUI_ALIGNx CENTER/LEFT/RIGHT
	int elmtSpacing;
} GLGUIWINDOWGENERATORsettings_t;

typedef struct
{
	int shortScrollBoxHeight;
	int tallScrollBoxHeight;
	int narrowScrollBoxWidth;
	int wideScrollBoxWidth;
	int scrollBoxPadding;
	int dropDownHeight;
	int dropDownPadding;
	int textFeildPadding;
	int openButtonWidth;
	int openButtonHeight;
	int scrollButtonWidth;
	int scrollButtonHeight;
	int topScrollButtonWidth;
	int topScrollButtonHeight;
	int standardButtonWidth;
	int standardButtonHeight;

	char *openButtonMDefTex;
	char *openButtonMOverTex;
	char *openButtonMDownTex;

	char *scrollUpButtonMDefTex;
	char *scrollUpButtonMOverTex;
	char *scrollUpButtonMDownTex;

	char *scrollDownButtonMDefTex;
	char *scrollDownButtonMOverTex;
	char *scrollDownButtonMDownTex;

	char *defaultButtonMDefTex;
	char *defaultButtonMOverTex;
	char *defaultButtonMDownTex;

	char *checkBoxOnTex;
	char *checkBoxOffTex;

	char *sliderBackTexV;
	char *sliderBackTexH;
	char *sliderButtonTex;

	int alignment;
	int elmtSpacing; 
} GLGUIWINDOWGENERATORSETUP;

typedef struct
{
	int start;
	int end;
	int width;
	int height;
	int alignment;
} GLGUIWINDOWGENERATORrow_t;

void GLGUIWINDOWGENERATORDestroyWindowStyle(void *data);
void GLGUIWINDOWGENERATORDestroyFont(void *data);
void GLGUIWINDOWGENERATORDestroyColorScheme(void *data);
void GLGUIWINDOWGENERATORDestroyFullSettings(void *data);

void GLGUIWINDOWGENERATORDestroyStringData(void *data);

int GLGUIWINDOWGENERATORCompareWindowStyle(const void *key1, const void *key2, int method);
int GLGUIWINDOWGENERATORCompareFont(const void *key1, const void *key2, int method);
int GLGUIWINDOWGENERATORCompareColorScheme(const void *key1, const void *key2, int method);
int GLGUIWINDOWGENERATORCompareFullSettings(const void *key1, const void *key2, int method);

void GLGUIWINDOWGENERATORInsertdata(void *data, const char *command,const char *dataString);

class CGLGUIWINDOWGENERATOR
{
public:
	CGLGUIWINDOWGENERATOR();
	~CGLGUIWINDOWGENERATOR();
	void Initialize(CLog *cLog, CTextureManager *cTexMan);
	void KillSystem();
	void RegisterWindowStyle(const char *styleName, int leftBorderWidth, int topBorderWidth, int rightBorderWidth,
		int bottomBorderWidth,int tileFactor,	const char *backTexName);
	void RegisterFontType(const char *fontName, int openGLFontListBase, int textWidth, int textHeight);
	void RegisterColorScheme(const char *schemeName,
		float backR,float backG,float backB,
		float textR,float textG,float textB,
		float highlightBackR,float highlightBackG,float highlightBackB,
		float highlightTextR,float highlightTextG,float highlightTextB,
		float buttonTextR,float buttonTextG,float buttonTextB);
	void RegisterFullSettings(const char *settingsName,const char *windowStyle,const char *font,
		const char *colorScheme,const char *settingsString);	
	void SetDefaults(GLGUIWINDOWGENERATORSETUP *setup);
	void ChangeCurrentSettings(const char *changes);
	void RestoreDefaults();

	bool SelectWindowStyle(const char *styleName);
	bool SelectFontType(const char *fontName);
	bool SelectColorScheme(const char *schemeName);
	bool SelectFullSettings(const char *settingsName);
	
	void SetAlignment(int alignment);
	void BeginRow();
	void EndRow();
	void NextColumn();

	void ForceDispMin(int width,int height);
	void ForceRowMin(int width,int height);

	void AddTextFeild(const char *elmtName, const char *initialText, int width, int maxInput);
	void AddTextDisplay(const char *elmtName, const char *text, DWORD flags=0);
	void AddScrollingText(const char *elmtName, const char *text, DWORD flags=0);
	void AddSelectList(const char *elmtName, const char *text, DWORD flags=0);
	void AddDropMenu(const char *elmtName, const char *text, DWORD flags=0);
	void AddStandardButton(const char *elmtName, const char *buttonText);
	void AddCustomButton(const char *elmtName, const char *buttonText,
		int width, int height,char *defaultTexName,
		char *mouseOverTexName, char *mouseDownTexName,
		float textColorR,float textColorG, float textColorB);
	void AddCheckBox(const char *elmtName, const char *text, DWORD flags=0);
	void AddSlider(const char *elmtName,int size, int position, bool isVertical=false);
	void AddImage(const char *elmtName,const char *texName, int width, int height);

	CGLGUIWindow *ReturnWindow(int xPos, int yPos);
	void ClearData();
	bool IsOk();
protected:
	int FindElmt(const char *elmtName);
	glguiElementData_t *AddElmt(const char *elmtName);
	void NextElmt();
	void NextElmt(int width, int height, int start);

	char *WordWrap(const char *text, DWORD flags);

	CLog *m_cLog;
	CTextureManager *m_cTexMan;
	GLGUIWINDOWGENERATORsettings_t m_defaultSettings;
	GLGUIWINDOWGENERATORsettings_t m_currentSettings;
	CList m_windowStyles;
	CList m_fontTypes;
	CList m_colorSchemes;
	CList m_fullSettings;
	CListElmt *m_curStyle;
	CListElmt *m_curFont;
	CListElmt *m_curScheme;
	CListElmt *m_curSet;

	CList m_stringList;
	CListElmt *m_curString;

	glguiElementData_t *m_winDesc;
	
	GLGUIWINDOWGENERATORrow_t m_rows[GLGUIWINDOWGENERATOR_MAXROWS];
	int m_winElmtCount;
	int m_curAlign;
	int m_curWidth;
	int m_curHeight;
	int m_curElmt;
	int m_curColStart;
	int m_curRow;
	int	m_curX;
	int m_curWindowHeight;
	bool m_isRow;

	char *m_noName;

	bool m_isOk;
};

#endif