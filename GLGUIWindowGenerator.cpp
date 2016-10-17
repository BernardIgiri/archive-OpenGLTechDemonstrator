#include "GLGUIWindowGenerator.h"
#include "StringUtilities.h"
/**********************************************************************************

  Starts an instance of CGLGUIWINDOWGENERATOR initializing all the
  variables.

**********************************************************************************/
CGLGUIWINDOWGENERATOR::CGLGUIWINDOWGENERATOR()
{
	m_cLog=NULL;
	m_cTexMan=NULL;
	m_curStyle=NULL;
	m_curFont=NULL;
	m_curScheme=NULL;
	m_curSet=NULL;
	m_curString=NULL;
	m_winDesc=NULL;	
	m_winElmtCount=0;
	m_curAlign=0;
	m_curWidth=0;
	m_curHeight=0;
	m_curElmt=0;
	m_curColStart=0;
	m_curRow=0;
	m_curX=0;
	m_curWindowHeight=0;
	m_noName=NULL;
	m_isOk=false;
	m_isRow=false;
	for (int i=0; i<GLGUIWINDOWGENERATOR_MAXROWS; i++)
	{
		m_rows[i].end=0;
		m_rows[i].height=0;
		m_rows[i].start=0;
		m_rows[i].width=0;
	}
}

/**********************************************************************************

  Destroys instance of CGLGUIWINDOWGENERATOR ensuring that KillSystem()
  has been called.

**********************************************************************************/
CGLGUIWINDOWGENERATOR::~CGLGUIWINDOWGENERATOR()
{
	if (clist_size(&m_windowStyles)||
		clist_size(&m_fontTypes)||
		clist_size(&m_colorSchemes)||
		clist_size(&m_fullSettings)||
		clist_size(&m_stringList))
		KillSystem();
}
/***********************************************************************************

  Kills Window Generator and frees all allocated ram.

***********************************************************************************/
void CGLGUIWINDOWGENERATOR::KillSystem()
{
	try
	{
		ClearData();
		clist_destroy(&m_windowStyles);
		clist_destroy(&m_fontTypes);
		clist_destroy(&m_colorSchemes);
		clist_destroy(&m_fullSettings);
		clist_destroy(&m_stringList);
		m_curStyle=NULL;
		m_curFont=NULL;
		m_curScheme=NULL;
		if (m_noName!=NULL)
			free(m_noName);
		m_noName=NULL;
		m_isOk=false;
	}
	catch (...)
	{
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: Error During KillSystem() call.");
		m_isOk=false;
		throw;
	}
}
/**********************************************************************************

  Initializes Window Generator with error logger and TextureManager.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::Initialize(CLog *cLog, CTextureManager *cTexMan)
{
	try
	{
		static bool isInitialized=false;
		if (isInitialized||m_winDesc!=NULL||m_noName!=NULL)
			KillSystem();
		if (cLog==NULL||cTexMan==NULL)
		{
			m_isOk=false;
			return;
		}
		m_cLog=cLog;
		m_cTexMan=cTexMan;
		m_noName=(char *)malloc(strlen("NoName")+1);
		strcpy(m_noName,"NoName");
		clist_init(&m_windowStyles,
			GLGUIWINDOWGENERATORDestroyWindowStyle,
			GLGUIWINDOWGENERATORCompareWindowStyle);
		clist_init(&m_fontTypes,
			GLGUIWINDOWGENERATORDestroyFont,
			GLGUIWINDOWGENERATORCompareFont);
		clist_init(&m_colorSchemes,
			GLGUIWINDOWGENERATORDestroyColorScheme,
			GLGUIWINDOWGENERATORCompareColorScheme);
		clist_init(&m_fullSettings,
			GLGUIWINDOWGENERATORDestroyFullSettings,
			GLGUIWINDOWGENERATORCompareFullSettings);
		clist_init(&m_stringList,
			GLGUIWINDOWGENERATORDestroyStringData,
			NULL);
		m_curString=clist_head(&m_stringList);
		m_curStyle=clist_head(&m_windowStyles);
		m_curFont=clist_head(&m_fontTypes);
		m_curScheme=clist_head(&m_colorSchemes);
		m_curSet=clist_head(&m_fullSettings);
		m_winDesc=NULL;
		m_winElmtCount=0;
		m_curAlign=0;
		m_curWidth=0;
		m_curHeight=0;
		m_curElmt=0;
		m_curColStart=0;
		m_curRow=0;
		m_curX=0;
		m_curWindowHeight=0;
		m_isRow=false;
		m_isOk=true;
		isInitialized=true;
		m_currentSettings.cTexMan=m_defaultSettings.cTexMan=cTexMan;
		for (int i=0; i<GLGUIWINDOWGENERATOR_MAXROWS; i++)
		{
			m_rows[i].end=0;
			m_rows[i].height=0;
			m_rows[i].start=0;
			m_rows[i].width=0;
		}
	}
	catch (...)
	{
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to Initialize.");
		m_isOk=false;
		throw;
	}
}
/***********************************************************************************

  Destroys data stored in the string list.

***********************************************************************************/
void GLGUIWINDOWGENERATORDestroyStringData(void *data)
{
	if (data!=NULL)
		free(data);
}
/**********************************************************************************

  Adds a new Window Style to the Window Style List.

  NOTE: Window textures are assumed to be 128x128 make border width
  adjustments accordingly. (When the texture are strecthed readjustment
  calculations must be made)

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::RegisterWindowStyle(const char *styleName,
												int leftBorderWidth, int topBorderWidth,
												int rightBorderWidth, int bottomBorderWidth,
												int tileFactor, const char *backTexName)
{
	if (!m_isOk)
		return;
	if (styleName==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: window style name is NULL.");
		return;
	}
	if (backTexName==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: window style %s, backTexName is NULL.",styleName);
		return;
	}
	GLGUIWINDOWGENERATORwindowStyle_t *newStyleData = NULL;
	newStyleData = new GLGUIWINDOWGENERATORwindowStyle_t;
	if (newStyleData==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for window style %s",styleName);
		return;
	}
	newStyleData->styleName=NULL;
	newStyleData->styleName=(char *)malloc(strlen(styleName)+1);
	if (newStyleData->styleName==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for window style name of %s",styleName);
		delete newStyleData;
		return;
	}
	strcpy(newStyleData->styleName,styleName);
	newStyleData->topBorderWidth=topBorderWidth;
	newStyleData->leftBorderWidth=leftBorderWidth;
	newStyleData->rightBorderWidth=rightBorderWidth;
	newStyleData->bottomBorderWidth=bottomBorderWidth;
	newStyleData->tileFactor=tileFactor;
	newStyleData->texture=m_cTexMan->FindTextureIndex((char *)backTexName);
	if (newStyleData->texture<0)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: texture name %s not found for use in window style %s.",
			backTexName,styleName);
		delete newStyleData;
		return;
	}
	if (clist_ins_next(&m_windowStyles,m_curStyle,newStyleData)!=0)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: insertion of window style %s into window style list failed.",
			styleName);
		delete newStyleData;
	}
	if (clist_size(&m_windowStyles)==1)
		m_curStyle=clist_head(&m_windowStyles);
}
/**********************************************************************************

  Destroys a window style in the window style list.

**********************************************************************************/
void GLGUIWINDOWGENERATORDestroyWindowStyle(void *data)
{
	if (data==NULL)
		return;
	GLGUIWINDOWGENERATORwindowStyle_t *styleData=(GLGUIWINDOWGENERATORwindowStyle_t *)data;
	free(styleData->styleName);
	delete styleData;
}
/**********************************************************************************

  Compares a window style in the window style list.

**********************************************************************************/
int GLGUIWINDOWGENERATORCompareWindowStyle(const void *key1, const void *key2, int method)
{
	GLGUIWINDOWGENERATORwindowStyle_t *k1=(GLGUIWINDOWGENERATORwindowStyle_t *)key1;
	GLGUIWINDOWGENERATORwindowStyle_t *k2=(GLGUIWINDOWGENERATORwindowStyle_t *)key2;
	int r=strcmp(k1->styleName,k2->styleName);
	if (r<NULL)
		return -1;
	else if (r>NULL)
		return 1;
	else
		return NULL;
	return NULL;
}
/**********************************************************************************

  Adds a new Font to the Font List.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::RegisterFontType(const char *fontName,int openGLFontListBase,
											 int textWidth,int textHeight)
{
	if (!m_isOk)
		return;
	if (fontName==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: fontName is NULL.");
		return;
	}
	GLGUIWINDOWGENERATORfont_t *newFontData = NULL;
	newFontData = new GLGUIWINDOWGENERATORfont_t;
	if (newFontData==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for font %s",fontName);
		return;
	}
	newFontData->fontName=NULL;
	newFontData->fontName=(char *)malloc(strlen(fontName)+1);
	if (newFontData->fontName==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for font name of %s",fontName);
		delete newFontData;
		return;
	}
	strcpy(newFontData->fontName,fontName);
	newFontData->fontListBase=openGLFontListBase;
	newFontData->textWidth=textWidth;
	newFontData->textHeight=textHeight;
	if (clist_ins_next(&m_fontTypes,m_curFont,newFontData)!=0)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: insertion of font %s into font list failed.",fontName);
		delete newFontData;
	}
	if (clist_size(&m_fontTypes)==1)
		m_curFont=clist_head(&m_fontTypes);
}
/**********************************************************************************

  Destroys a font in the font list.

**********************************************************************************/
void GLGUIWINDOWGENERATORDestroyFont(void *data)
{
	if (data==NULL)
		return;
	GLGUIWINDOWGENERATORfont_t *fontData=(GLGUIWINDOWGENERATORfont_t *)data;
	free(fontData->fontName);	
	delete fontData;
}
/**********************************************************************************

  Compares a window style in the window style list.

**********************************************************************************/
int GLGUIWINDOWGENERATORCompareFont(const void *key1, const void *key2, int method)
{
	GLGUIWINDOWGENERATORfont_t *k1=(GLGUIWINDOWGENERATORfont_t *)key1;
	GLGUIWINDOWGENERATORfont_t *k2=(GLGUIWINDOWGENERATORfont_t *)key2;
	int r=strcmp(k1->fontName,k2->fontName);
	if (r<NULL)
		return -1;
	else if (r>NULL)
		return 1;
	else
		return NULL;
	return NULL;
}
/**********************************************************************************

  Adds a new Color Scheme to the Color Scheme List.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::RegisterColorScheme(const char *schemeName,
												float backR,float backG,float backB,
												float textR,float textG,float textB,
												float highlightBackR,float highlightBackG,float highlightBackB,
												float highlightTextR,float highlightTextG,float highlightTextB,
												float buttonTextR,float buttonTextG,float buttonTextB)
{
	if (!m_isOk)
		return;
	if (schemeName==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: schemeName is NULL.");
		return;
	}
	GLGUIWINDOWGENERATORcolorScheme_t *newColorSchemeData = NULL;
	newColorSchemeData = new GLGUIWINDOWGENERATORcolorScheme_t;
	if (newColorSchemeData==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for color scheme %s",schemeName);
		return;
	}
	newColorSchemeData->schemeName=NULL;
	newColorSchemeData->schemeName=(char *)malloc(strlen(schemeName)+1);
	if (newColorSchemeData->schemeName==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for the scheme name of %s",schemeName);
		delete newColorSchemeData;
		return;
	}
	strcpy(newColorSchemeData->schemeName,schemeName);
	newColorSchemeData->backB=backB;
	newColorSchemeData->backG=backG;
	newColorSchemeData->backR=backR;
	newColorSchemeData->bTextB=buttonTextB;
	newColorSchemeData->bTextG=buttonTextG;
	newColorSchemeData->bTextR=buttonTextR;
	newColorSchemeData->hBackB=highlightBackB;
	newColorSchemeData->hBackG=highlightBackG;
	newColorSchemeData->hBackR=highlightBackR;
	newColorSchemeData->hTextB=highlightTextB;
	newColorSchemeData->hTextG=highlightTextG;
	newColorSchemeData->hTextR=highlightTextR;
	newColorSchemeData->textB=textB;
	newColorSchemeData->textG=textG;
	newColorSchemeData->textR=textR;
	if (clist_ins_next(&m_colorSchemes,m_curScheme,newColorSchemeData)!=0)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: insertion of color scheme %s into color scheme list failed.",
			schemeName);
		delete newColorSchemeData;
	}
	if (clist_size(&m_colorSchemes)==1)
		m_curScheme=clist_head(&m_colorSchemes);
}
/**********************************************************************************

  Destroys a color scheme in the color scheme list.

**********************************************************************************/
void GLGUIWINDOWGENERATORDestroyColorScheme(void *data)
{
	if (data==NULL)
		return;
	GLGUIWINDOWGENERATORcolorScheme_t *schemeData=(GLGUIWINDOWGENERATORcolorScheme_t *)data;
	free(schemeData->schemeName);	
	delete schemeData;
}
/**********************************************************************************

  Compares a window style in the window style list.

**********************************************************************************/
int GLGUIWINDOWGENERATORCompareColorScheme(const void *key1, const void *key2, int method)
{
	GLGUIWINDOWGENERATORcolorScheme_t *k1=(GLGUIWINDOWGENERATORcolorScheme_t *)key1;
	GLGUIWINDOWGENERATORcolorScheme_t *k2=(GLGUIWINDOWGENERATORcolorScheme_t *)key2;
	int r=strcmp(k1->schemeName,k2->schemeName);
	if (r<NULL)
		return -1;
	else if (r>NULL)
		return 1;
	else
		return NULL;
	return NULL;
}


/**********************************************************************************

  Adds new set of settings to The Full Settings list.
  Set to NULL to varibles that you do not want to have effect.
  NOTE: On selection RestoreDefaults() is called.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::RegisterFullSettings(const char *settingsName,
												 const char *windowStyle,
												 const char *font,
												 const char *colorScheme,
												 const char *settingsString)
{
	if (!m_isOk)
		return;
	if (settingsName==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: settingsName is NULL.");
		return;
	}
	GLGUIWINDOWGENERATORfullSettings_t *newSetData = NULL;
	newSetData = new GLGUIWINDOWGENERATORfullSettings_t;
	if (newSetData==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for settings %s",settingsName);
		return;
	}
	newSetData->settingsName=NULL;
	newSetData->settingsName=(char *)malloc(strlen(settingsName)+1);
	if (newSetData->settingsName==NULL)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for the settings name of %s",settingsName);
		delete newSetData;
		return;
	}
	strcpy(newSetData->settingsName,settingsName);
	
	newSetData->colorScheme=NULL;
	if (colorScheme!=NULL)
	{
		newSetData->colorScheme=(char *)malloc(strlen(colorScheme)+1);
		if (newSetData->colorScheme!=NULL)
			strcpy(newSetData->colorScheme,colorScheme);
		else
			m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for full settings data.");
	}
	newSetData->font=NULL;
	if (font!=NULL)
	{
		newSetData->font=(char *)malloc(strlen(font)+1);
		if (newSetData->font!=NULL)
			strcpy(newSetData->font,font);
		else
			m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for full settings data.");
	}
	newSetData->settings=NULL;
	if (settingsString!=NULL)
	{
		newSetData->settings=(char *)malloc(strlen(settingsString)+1);
		if (newSetData->colorScheme!=NULL)
			strcpy(newSetData->settings,settingsString);
		else
			m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for full settings data.");
	}
	newSetData->windowStyle=NULL;
	if (windowStyle!=NULL)
	{
		newSetData->windowStyle=(char *)malloc(strlen(windowStyle)+1);
		if (newSetData->windowStyle!=NULL)
			strcpy(newSetData->windowStyle,windowStyle);
		else
			m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for full settings data.");
	}

	if (clist_ins_next(&m_fullSettings,m_curSet,newSetData)!=0)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: insertion of settings %s into full settings list failed.",
			settingsName);
		delete newSetData;
	}
	if (clist_size(&m_fullSettings)==1)
		m_curSet=clist_head(&m_fullSettings);
}
/**********************************************************************************

  Destroys a set of setting in the full settings list.

**********************************************************************************/
void GLGUIWINDOWGENERATORDestroyFullSettings(void *data)
{
	if (data==NULL)
		return;
	GLGUIWINDOWGENERATORfullSettings_t *setData=(GLGUIWINDOWGENERATORfullSettings_t *)data;
	if (setData->colorScheme!=NULL)
		free(setData->colorScheme);
	if (setData->font!=NULL)
		free(setData->font);
	if (setData->settings!=NULL)
		free(setData->settings);
	if (setData->settingsName!=NULL)
		free(setData->settingsName);
	if (setData->windowStyle!=NULL)
		free(setData->windowStyle);
	delete setData;
	setData=NULL;
}
/**********************************************************************************

  Compares a set of setting in the full settings list.

**********************************************************************************/
int GLGUIWINDOWGENERATORCompareFullSettings(const void *key1, const void *key2, int method)
{
	GLGUIWINDOWGENERATORfullSettings_t *k1=(GLGUIWINDOWGENERATORfullSettings_t *)key1;
	GLGUIWINDOWGENERATORfullSettings_t *k2=(GLGUIWINDOWGENERATORfullSettings_t *)key2;
	int r=strcmp(k1->settingsName,k2->settingsName);
	if (r<NULL)
		return -1;
	else if (r>NULL)
		return 1;
	else
		return NULL;
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////////////////
/**********************************************************************************

  Sets the initial defaults for window elements defined by system.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::SetDefaults(GLGUIWINDOWGENERATORSETUP *setup)
{
	if (!m_isOk)
		return;
	if ((setup->checkBoxOffTex==NULL)||
		(setup->checkBoxOnTex==NULL)||
		(setup->defaultButtonMDefTex==NULL)||
		(setup->defaultButtonMDownTex==NULL)||
		(setup->defaultButtonMOverTex==NULL)||
		(setup->openButtonMDefTex==NULL)||
		(setup->openButtonMDownTex==NULL)||
		(setup->openButtonMOverTex==NULL)||
		(setup->scrollDownButtonMDefTex==NULL)||
		(setup->scrollDownButtonMDownTex==NULL)||
		(setup->scrollDownButtonMOverTex==NULL)||
		(setup->scrollUpButtonMDefTex==NULL)||
		(setup->scrollUpButtonMDownTex==NULL)||
		(setup->scrollUpButtonMOverTex==NULL)||
		(setup->sliderBackTexV==NULL)||
		(setup->sliderBackTexH==NULL)||
		(setup->sliderButtonTex==NULL))
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: one of the specified default texture names is NULL.");
		return;
	}
	if (((m_defaultSettings.checkBoxOffTex=m_cTexMan->FindTextureIndex(setup->checkBoxOffTex))<0)||
		((m_defaultSettings.checkBoxOnTex=m_cTexMan->FindTextureIndex(setup->checkBoxOnTex))<0)||
		((m_defaultSettings.defaultButtonMDefTex=m_cTexMan->FindTextureIndex(setup->defaultButtonMDefTex))<0)||
		((m_defaultSettings.defaultButtonMDownTex=m_cTexMan->FindTextureIndex(setup->defaultButtonMDownTex))<0)||
		((m_defaultSettings.defaultButtonMOverTex=m_cTexMan->FindTextureIndex(setup->defaultButtonMOverTex))<0)||
		((m_defaultSettings.openButtonMDefTex=m_cTexMan->FindTextureIndex(setup->openButtonMDefTex))<0)||
		((m_defaultSettings.openButtonMDownTex=m_cTexMan->FindTextureIndex(setup->openButtonMDownTex))<0)||
		((m_defaultSettings.openButtonMOverTex=m_cTexMan->FindTextureIndex(setup->openButtonMOverTex))<0)||
		((m_defaultSettings.scrollDownButtonMDefTex=m_cTexMan->FindTextureIndex(setup->scrollDownButtonMDefTex))<0)||
		((m_defaultSettings.scrollDownButtonMDownTex=m_cTexMan->FindTextureIndex(setup->scrollDownButtonMDownTex))<0)||
		((m_defaultSettings.scrollDownButtonMOverTex=m_cTexMan->FindTextureIndex(setup->scrollDownButtonMOverTex))<0)||
		((m_defaultSettings.scrollUpButtonMDefTex=m_cTexMan->FindTextureIndex(setup->scrollUpButtonMDefTex))<0)||
		((m_defaultSettings.scrollUpButtonMDownTex=m_cTexMan->FindTextureIndex(setup->scrollUpButtonMDownTex))<0)||
		((m_defaultSettings.scrollUpButtonMOverTex=m_cTexMan->FindTextureIndex(setup->scrollUpButtonMOverTex))<0)||
		((m_defaultSettings.sliderBackTexV=m_cTexMan->FindTextureIndex(setup->sliderBackTexV))<0)||
		((m_defaultSettings.sliderBackTexH=m_cTexMan->FindTextureIndex(setup->sliderBackTexH))<0)||
		((m_defaultSettings.sliderButtonTex=m_cTexMan->FindTextureIndex(setup->sliderButtonTex))<0))
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: one of the specified default textures could not be found.");
		return;
	}
	m_defaultSettings.alignment				=setup->alignment;
	m_defaultSettings.dropDownHeight		=setup->dropDownHeight;
	m_defaultSettings.dropDownPadding		=setup->dropDownPadding;
	m_defaultSettings.elmtSpacing			=setup->elmtSpacing;
	m_defaultSettings.narrowScrollBoxWidth	=setup->narrowScrollBoxWidth;
	m_defaultSettings.openButtonHeight		=setup->openButtonHeight;
	m_defaultSettings.openButtonWidth		=setup->openButtonWidth;
	m_defaultSettings.scrollBoxPadding		=setup->scrollBoxPadding;
	m_defaultSettings.scrollButtonHeight	=setup->scrollButtonHeight;
	m_defaultSettings.scrollButtonWidth		=setup->scrollButtonWidth;
	m_defaultSettings.shortScrollBoxHeight	=setup->shortScrollBoxHeight;
	m_defaultSettings.standardButtonHeight	=setup->standardButtonHeight;
	m_defaultSettings.standardButtonWidth	=setup->standardButtonWidth;
	m_defaultSettings.tallScrollBoxHeight	=setup->tallScrollBoxHeight;
	m_defaultSettings.textFeildPadding		=setup->textFeildPadding;
	m_defaultSettings.topScrollButtonHeight	=setup->topScrollButtonHeight;
	m_defaultSettings.topScrollButtonWidth	=setup->topScrollButtonWidth;
	m_defaultSettings.wideScrollBoxWidth	=setup->wideScrollBoxWidth;
	memcpy(&m_currentSettings,&m_defaultSettings,sizeof(GLGUIWINDOWGENERATORsettings_t));
	m_curAlign=m_defaultSettings.alignment;
}
/**********************************************************************************

  Used to read default settings change requests from a string
  and apply them to the default settings structure.

**********************************************************************************/
void GLGUIWINDOWGENERATORInsertdata(void *data, const char *command,const char *dataString)
{
	if (data==NULL||command==NULL||dataString==NULL)
		return;
	GLGUIWINDOWGENERATORsettings_t *settings=(GLGUIWINDOWGENERATORsettings_t *)data;
	if (strcmp(command,"CHECKBOXOFFTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->checkBoxOffTex=index;
	}
	else if (strcmp(command,"CHECKBOXONTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->checkBoxOnTex=index;
	}	
	else if (strcmp(command,"DEFAULTBUTTONMDEFTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->defaultButtonMDefTex=index;
	}	
	else if (strcmp(command,"DEFAULTBUTTONMDOWNTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->defaultButtonMDownTex=index;
	}
	else if (strcmp(command,"DEFAULTBUTTONMOVERTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->defaultButtonMOverTex=index;
	}
	else if (strcmp(command,"OPENBUTTONMDEFTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->openButtonMDefTex=index;
	}
	else if (strcmp(command,"OPENBUTTONMDOWNTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->openButtonMDownTex=index;
	}
	else if (strcmp(command,"OPENBUTTONMOVERTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->openButtonMOverTex=index;
	}
	else if (strcmp(command,"SCROLLDOWNBUTTONMDEFTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->scrollDownButtonMDefTex=index;
	}
	else if (strcmp(command,"SCROLLDOWNBUTTONMDOWNTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->scrollDownButtonMDownTex=index;
	}
	else if (strcmp(command,"SCROLLDOWNBUTTONMOVERTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->scrollDownButtonMOverTex=index;
	}
	else if (strcmp(command,"SCROLLUPBUTTONMDEFTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->scrollUpButtonMDefTex=index;
	}
	else if (strcmp(command,"SCROLLUPBUTTONMDOWNTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->scrollUpButtonMDownTex=index;
	}
	else if (strcmp(command,"SCROLLUPBUTTONMOVERTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->scrollUpButtonMOverTex=index;
	}
	else if (strcmp(command,"SLIDERBACKTEXV")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->sliderBackTexV=index;
	}
	else if (strcmp(command,"SLIDERBACKTEXH")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->sliderBackTexH=index;
	}
	else if (strcmp(command,"SLIDERBUTTONTEX")==NULL)
	{
		int index=settings->cTexMan->FindTextureIndex((char *)dataString);
		if (index>=0)
			settings->sliderButtonTex=index;
	}

	
	else if (strcmp(command,"ALIGNMENT")==NULL)
	{
		if (stricmp(dataString,"center")==NULL)
			settings->alignment=GLGUI_ALIGNCENTER;
		else if (stricmp(dataString,"right")==NULL)
			settings->alignment=GLGUI_ALIGNRIGHT;
		else
			settings->alignment=GLGUI_ALIGNLEFT;
	}
	else if (strcmp(command,"DROPDOWNHEIGHT")==NULL)
	{
		sscanf(dataString,"%i",&settings->dropDownHeight);
	}
	else if (strcmp(command,"DROPDOWNPADDING")==NULL)
	{
		sscanf(dataString,"%i",&settings->dropDownPadding);
	}
	else if (strcmp(command,"ELMTSPACING")==NULL)
	{
		sscanf(dataString,"%i",&settings->elmtSpacing);
	}
	else if (strcmp(command,"NARROWSCROLLBOXWIDTH")==NULL)
	{
		sscanf(dataString,"%i",&settings->narrowScrollBoxWidth);
	}
	else if (strcmp(command,"OPENBUTTONHEIGHT")==NULL)
	{
		sscanf(dataString,"%i",&settings->openButtonHeight);
	}
	else if (strcmp(command,"OPENBUTTONWIDTH")==NULL)
	{
		sscanf(dataString,"%i",&settings->openButtonWidth);
	}
	else if (strcmp(command,"SCROLLBOXPADDING")==NULL)
	{
		sscanf(dataString,"%i",&settings->scrollBoxPadding);
	}
	else if (strcmp(command,"SCROLLBUTTONHEIGHT")==NULL)
	{
		sscanf(dataString,"%i",&settings->scrollButtonHeight);
	}
	else if (strcmp(command,"SCROLLBUTTONWIDTH")==NULL)
	{
		sscanf(dataString,"%i",&settings->dropDownHeight);
	}
	else if (strcmp(command,"SHORTSCROLLBOXHEIGHT")==NULL)
	{
		sscanf(dataString,"%i",&settings->shortScrollBoxHeight);
	}
	else if (strcmp(command,"STANDARDBUTTONHEIGHT")==NULL)
	{
		sscanf(dataString,"%i",&settings->standardButtonHeight);
	}
	else if (strcmp(command,"STANDARDBUTTONWIDTH")==NULL)
	{
		sscanf(dataString,"%i",&settings->standardButtonWidth);
	}
	else if (strcmp(command,"TALLSCROLLBOXHEIGHT")==NULL)
	{
		sscanf(dataString,"%i",&settings->tallScrollBoxHeight);
	}
	else if (strcmp(command,"TEXTFEILDPADDING")==NULL)
	{
		sscanf(dataString,"%i",&settings->textFeildPadding);
	}
	else if (strcmp(command,"TOPSCROLLBUTTONHEIGHT")==NULL)
	{
		sscanf(dataString,"%i",&settings->topScrollButtonHeight);
	}
	else if (strcmp(command,"TOPSCROLLBUTTONWIDTH")==NULL)
	{
		sscanf(dataString,"%i",&settings->topScrollButtonWidth);
	}
	else if (strcmp(command,"WIDESCROLLBOXWIDTH")==NULL)
	{
		sscanf(dataString,"%i",&settings->wideScrollBoxWidth);
	}
}
/**********************************************************************************

  Changes current settings for window generation.
  Initialized defaults may be restored by calling RestoreDefaults()

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::ChangeCurrentSettings(const char *changes)
{
	if (changes!=NULL&&m_isOk)
	{
		StrUtil_ReadCommandString(changes,&m_currentSettings,GLGUIWINDOWGENERATORInsertdata);
		m_curAlign=m_currentSettings.alignment;
	}
}
/**********************************************************************************

  Resets Window Generator to the Initialized Default Settings.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::RestoreDefaults()
{
	if (m_isOk)
	{
		memcpy(&m_currentSettings,&m_defaultSettings,sizeof(GLGUIWINDOWGENERATORsettings_t));
		m_curAlign=m_currentSettings.alignment;
		m_curStyle=clist_head(&m_windowStyles);
		m_curFont=clist_head(&m_fontTypes);
		m_curScheme=clist_head(&m_colorSchemes);
		if (clist_size(&m_fullSettings)>0)
			m_curSet=clist_head(&m_fullSettings);
	}
}



/**********************************************************************************

  Selects a window style from the window style list. Returns false on failure.

**********************************************************************************/
bool CGLGUIWINDOWGENERATOR::SelectWindowStyle(const char *styleName)
{
	if (!m_isOk||styleName==NULL)
		return false;
	if (m_curStyle!=NULL)
	{
		if (strcmp(styleName,((GLGUIWINDOWGENERATORwindowStyle_t *)clist_data(m_curStyle))->styleName)==NULL)
			return true;
	}
	GLGUIWINDOWGENERATORwindowStyle_t searchData;
	searchData.styleName=(char *)styleName;
	return (clist_search(&m_windowStyles,&searchData,0,&m_curStyle)==1);
}
/**********************************************************************************

  Selects a font from the font list. Returns false on failure.

**********************************************************************************/
bool CGLGUIWINDOWGENERATOR::SelectFontType(const char *fontName)
{
	if (!m_isOk||fontName==NULL)
		return false;
	if (m_curFont!=NULL)
	{
		if (strcmp(fontName,((GLGUIWINDOWGENERATORfont_t *)clist_data(m_curFont))->fontName)==NULL)
			return true;
	}
	GLGUIWINDOWGENERATORfont_t searchData;
	searchData.fontName=(char *)fontName;
	return (clist_search(&m_fontTypes,&searchData,0,&m_curFont)==1);
}
/**********************************************************************************

  Selects a color scheme from the color scheme list. Returns false on failure.

**********************************************************************************/
bool CGLGUIWINDOWGENERATOR::SelectColorScheme(const char *schemeName)
{
	if (!m_isOk||schemeName==NULL)
		return false;
	if (m_curScheme!=NULL)
	{
		if (strcmp(schemeName,((GLGUIWINDOWGENERATORcolorScheme_t *)clist_data(m_curScheme))->schemeName)==NULL)
			return true;
	}
	GLGUIWINDOWGENERATORcolorScheme_t searchData;
	searchData.schemeName=(char *)schemeName;
	return (clist_search(&m_colorSchemes,&searchData,0,&m_curScheme)==1);
}
/**********************************************************************************

  Selects a set of setttings from the fullSettings list.
  Returns false on failure.

**********************************************************************************/
bool CGLGUIWINDOWGENERATOR::SelectFullSettings(const char *settingsName)
{
	if (!m_isOk||settingsName==NULL)
		return false;
	bool success=false;
	if (m_curSet!=NULL)
	{
		if (strcmp(settingsName,((GLGUIWINDOWGENERATORfullSettings_t *)clist_data(m_curSet))->settingsName)==NULL)
			success=true;
	}
	if (!success&&m_curSet!=NULL)
	{
		GLGUIWINDOWGENERATORfullSettings_t searchData;
		searchData.settingsName=(char *)settingsName;
		success = (clist_search(&m_fullSettings,&searchData,0,&m_curSet)==1);
	}
	if (success)
	{
		memcpy(&m_currentSettings,&m_defaultSettings,sizeof(GLGUIWINDOWGENERATORsettings_t));
		m_curAlign=m_currentSettings.alignment;

		GLGUIWINDOWGENERATORfullSettings_t * pCurSet = (GLGUIWINDOWGENERATORfullSettings_t *)clist_data(m_curSet);		
		if (clist_size(&m_fullSettings)>0)
			m_curSet=clist_head(&m_fullSettings);
		if (pCurSet==NULL)
			return false;
		if (pCurSet->colorScheme!=NULL)
			success = success && SelectColorScheme(pCurSet->colorScheme);
		else
			m_curScheme=clist_head(&m_colorSchemes);
		if (pCurSet->font!=NULL)
			success = success && SelectFontType(pCurSet->font);	
		else
			m_curFont=clist_head(&m_fontTypes);
		if (pCurSet->windowStyle!=NULL)
			success = success && SelectWindowStyle(pCurSet->windowStyle);
		else
			m_curStyle=clist_head(&m_windowStyles);
		if (pCurSet->settings!=NULL)
			ChangeCurrentSettings(pCurSet->settings);
	}
	return success;
}



/**********************************************************************************

  Sets the current alingment to new value.
  Call RestoreDefaults() to revert to initial settings.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::SetAlignment(int alignment)
{
	if (!m_isOk)
		return;
	m_currentSettings.alignment=m_curAlign=alignment;
}
/**********************************************************************************

  Starts a new horizontal row. (Only needs to be called if multiple items
  occur on the same row)

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::BeginRow()
{
	if (!m_isOk)
		return;
	m_rows[m_curRow].start=m_curElmt;
	m_rows[m_curRow].height=0;
	m_rows[m_curRow].width=0;
	m_rows[m_curRow].alignment=m_curAlign;
	m_isRow=true;
}
/**********************************************************************************

  Ends the current horizontal row. ( Call at the end of every row started by
  BeginRow() )

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::EndRow()
{
	if (!m_isOk)
		return;
	m_isRow=false;
	m_rows[m_curRow].alignment=m_curAlign;
	m_rows[m_curRow].end=m_curElmt-1;
	m_curHeight+=m_rows[m_curRow].height+m_defaultSettings.elmtSpacing;
	if (m_rows[m_curRow].width>m_curWidth)
		m_curWidth=m_rows[m_curRow].width;
	m_curRow++;
	if (GLGUIWINDOWGENERATOR_MAXROWS<=m_curRow)
	{
		m_isOk=false;
		m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: window generation required more than %i rows.",
			GLGUIWINDOWGENERATOR_MAXROWS);
	}
}
/**********************************************************************************

  Starts a new column to the right of the last column.
  Elements added after a call to this will be added in the normal order except
  to the right of the elements in the last column.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::NextColumn()
{
	if (!m_isOk)
		return;
	GLGUIWINDOWGENERATORwindowStyle_t *pCurStyle=(GLGUIWINDOWGENERATORwindowStyle_t *)clist_data(m_curStyle);
	int rowCount=m_curRow;
	int columnHeight=0;
	for (int i=0; i<rowCount; i++)
	{
		columnHeight+=m_rows[i].height+m_currentSettings.elmtSpacing;
		int adjustFactor=0;
		switch (m_rows[i].alignment)
		{
		case GLGUI_ALIGNRIGHT:
			adjustFactor=m_curWidth-m_rows[i].width;
			break;
		case GLGUI_ALIGNLEFT:
			adjustFactor=0;
			break;
		default:
			adjustFactor=m_curWidth/2-m_rows[i].width/2;
			break;
		}
		adjustFactor+=m_curX+pCurStyle->leftBorderWidth;
		glguiElementData_t *pCurElmt=NULL;
		for (int n=m_rows[i].start; n<(m_rows[i].end+1); n++)
		{
			pCurElmt=&m_winDesc[n];
			pCurElmt->posX+=adjustFactor;
			pCurElmt->posY+=pCurStyle->bottomBorderWidth;
		}
	}
	if (m_curWindowHeight<columnHeight)
		m_curWindowHeight=columnHeight;
	m_curX+=m_curWidth+1;
	m_curWidth=0;
	m_curHeight=0;
	m_curRow=0;
	for (i=0; i<GLGUIWINDOWGENERATOR_MAXROWS; i++)
	{
		m_rows[i].end=0;
		m_rows[i].height=0;
		m_rows[i].start=0;
		m_rows[i].width=0;
	}
}
/**********************************************************************************

  Adds a single element to the window description array.
  On error logs report and returns NULL;

**********************************************************************************/
glguiElementData_t *CGLGUIWINDOWGENERATOR::AddElmt(const char *elmtName)
{
	if (m_winDesc==NULL)
	{
		m_winDesc=(glguiElementData_t *)malloc(sizeof(glguiElementData_t)*2);
		if (m_winDesc==NULL)
		{
			m_isOk=false;
			m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to allocate ram for element %s.",elmtName);
			return NULL;
		}
		m_winElmtCount=1;
	}
	else if (m_winElmtCount==1)
	{
		m_winElmtCount=2;
	}
	else
	{
		m_winElmtCount++;
		glguiElementData_t *newPtr=NULL;
		newPtr=(glguiElementData_t *)realloc(m_winDesc,sizeof(glguiElementData_t)*m_winElmtCount);
		if (newPtr==NULL)
		{
			m_isOk=false;
			m_cLog->lprintf("CGLGUIWINDOWGENERATOR error: failed to reallocate new ram for element #%i %s",m_curElmt,elmtName);
		}
		m_winDesc=newPtr;
	}
	glguiElementData_t *pCurElmt=NULL;
	/*if (m_winElmtCount<2)
		pCurElmt=m_winDesc;
	else*/
		pCurElmt=&m_winDesc[m_curElmt];
	return pCurElmt;
}
/**********************************************************************************

  Moves on to the next Element and updates the coordinates for the next element.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::NextElmt()
{
	glguiElementData_t *pCurElmt=NULL;
	pCurElmt=&m_winDesc[m_curElmt];
	int left, top, right, bottom;
	GLGUICoordinateCalculator(left,top,right,bottom,pCurElmt);	
	int height = (top-bottom+1), width = right-left+1;
	pCurElmt->posX=m_rows[m_curRow].width;
	m_rows[m_curRow].width+=width;
	m_rows[m_curRow].alignment=m_curAlign;
	if (height>m_rows[m_curRow].height)
		m_rows[m_curRow].height=height;
	if (!m_isRow)
	{
		m_rows[m_curRow].end=m_rows[m_curRow].start=m_curElmt;
		m_curHeight+=m_rows[m_curRow].height+m_currentSettings.elmtSpacing;
		if (m_rows[m_curRow].width>m_curWidth)
			m_curWidth=m_rows[m_curRow].width;
		m_curRow++;
	}
	m_curElmt++;
}
void CGLGUIWINDOWGENERATOR::NextElmt(int width,int height,int start)
{
	m_rows[m_curRow].width+=width;
	m_rows[m_curRow].alignment=m_curAlign;
	if (height>m_rows[m_curRow].height)
		m_rows[m_curRow].height=height;
	if (!m_isRow)
	{
		m_rows[m_curRow].start=start;
		m_rows[m_curRow].end=m_curElmt;
		m_curHeight+=m_rows[m_curRow].height+m_currentSettings.elmtSpacing;
		if (m_rows[m_curRow].width>m_curWidth)
			m_curWidth=m_rows[m_curRow].width;
		m_curRow++;
	}
	m_curElmt++;
}

/**********************************************************************************

  Forces the window to meet these minimal dimensions.
  NOTE: Setting a positive minimal height should only be done after all
  of the elements in the window have already been inserted.  Doing other wise
  will cause the following elements to stack on top of the added height.
  Width only works on current column.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::ForceDispMin(int width,int height)
{
	if (width>m_curWidth)
		m_curWidth=width;
	if (height>m_curWindowHeight)
		m_curWindowHeight=height;
}
/**********************************************************************************

  Forces the row within the window to meet these minimal dimensions.
  NOTE: Width applied to entire window column.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::ForceRowMin(int width,int height)
{
	if (m_rows[m_curRow].height<height)
		m_rows[m_curRow].height=height;
	if (width>m_curWidth)
		m_curWidth=width;
}
/**********************************************************************************

  Adds a single text feild to the window being generated.

  NOTE: Does not maintain pointers to string data, memory management of 
  const char *elmtName and other string data is the responsibility of the
  user.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::AddTextFeild(const char *elmtName, const char *initialText,
										 int width, int maxInput)
{
	if (!m_isOk||elmtName==NULL||
		m_curFont==NULL||
		m_curStyle==NULL||
		m_curScheme==NULL)
		return;
	glguiElementData_t *pCurElmt=AddElmt(elmtName);
	if (pCurElmt==NULL)
		return;
	GLGUIWINDOWGENERATORfont_t *pCurFont=(GLGUIWINDOWGENERATORfont_t *)clist_data(m_curFont);
	GLGUIWINDOWGENERATORcolorScheme_t *pCurScheme=(GLGUIWINDOWGENERATORcolorScheme_t *)clist_data(m_curScheme);
	GLGUICreateTextFeild(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		0,m_curHeight,width,maxInput,m_currentSettings.textFeildPadding,(char *)initialText,true,
		true,pCurScheme->backR,pCurScheme->backG,pCurScheme->backB,
		pCurScheme->textR,pCurScheme->textG,pCurScheme->textB);
	NextElmt();
}
/**********************************************************************************

  Adds a single text feild to the window being generated.

  NOTE: Does not maintain pointers to string data, memory management of 
  const char *elmtName and other string data is the responsibility of the
  user.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::AddTextDisplay(const char *elmtName, const char *text, DWORD flags)
{
	if (!m_isOk||elmtName==NULL||
		m_curFont==NULL||
		m_curStyle==NULL||
		m_curScheme==NULL)
		return;
	glguiElementData_t *pCurElmt=AddElmt(elmtName);
	if (pCurElmt==NULL)
		return;
	GLGUIWINDOWGENERATORfont_t *pCurFont=(GLGUIWINDOWGENERATORfont_t *)clist_data(m_curFont);
	GLGUIWINDOWGENERATORcolorScheme_t *pCurScheme=(GLGUIWINDOWGENERATORcolorScheme_t *)clist_data(m_curScheme);
	GLGUICreateTextDisplay(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		0,m_curHeight,m_currentSettings.textFeildPadding,!(flags & GLGUI_FLAGNOBACKGROUND),true,
		pCurScheme->backR,pCurScheme->backG,pCurScheme->backB,
		pCurScheme->textR,pCurScheme->textG,pCurScheme->textB,
		WordWrap(text,flags));
	NextElmt();
}
/**********************************************************************************

  Adds a single scrolling text box to the window being generated.

  NOTE: Does not maintain pointers to string data, memory management of 
  const char *elmtName and other string data is the responsibility of the
  user.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::AddScrollingText(const char *elmtName, const char *text, DWORD flags)
{
	if (!m_isOk||elmtName==NULL||
		m_curFont==NULL||
		m_curStyle==NULL||
		m_curScheme==NULL)
		return;
	glguiElementData_t *pCurElmt=AddElmt(elmtName);
	if (pCurElmt==NULL)
		return;
	int start=m_curElmt;
	GLGUIWINDOWGENERATORfont_t *pCurFont=(GLGUIWINDOWGENERATORfont_t *)clist_data(m_curFont);
	GLGUIWINDOWGENERATORcolorScheme_t *pCurScheme=(GLGUIWINDOWGENERATORcolorScheme_t *)clist_data(m_curScheme);
	int sentWidth=((flags & GLGUI_FLAGWIDE) ? m_currentSettings.wideScrollBoxWidth :
					m_currentSettings.narrowScrollBoxWidth);
	int sentHeight=((flags & GLGUI_FLAGTALL) ? m_currentSettings.tallScrollBoxHeight :
					m_currentSettings.shortScrollBoxHeight);
	GLGUICreateSelectionList(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		0,m_curHeight,sentWidth,sentHeight,m_curElmt+1,m_curElmt+2,m_curElmt+3,
		m_currentSettings.scrollBoxPadding,!(flags & GLGUI_FLAGNOBACKGROUND),false,true,
		pCurScheme->backR,pCurScheme->backG,pCurScheme->backB,
		pCurScheme->textR,pCurScheme->textG,pCurScheme->textB,
		pCurScheme->backR,pCurScheme->backG,pCurScheme->backB,
		pCurScheme->textR,pCurScheme->textG,pCurScheme->textB,
		WordWrap(text,flags),((flags & GLGUI_FLAGSUBMITBUTTON)!=0));
	pCurElmt->posX=m_rows[m_curRow].width;
	int left, top, right, bottom;
	GLGUICoordinateCalculator(left,top,right,bottom,pCurElmt);	
	int height = (top-bottom+1),
		width = (right-left+1)+m_currentSettings.scrollButtonWidth;
	m_curElmt++;
	pCurElmt=AddElmt(m_noName);
	GLGUICreateButton(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		right,top-(m_currentSettings.scrollButtonHeight-1),
		false,m_currentSettings.scrollButtonWidth,
		m_currentSettings.scrollButtonHeight,NULL,
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollUpButtonMDefTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollUpButtonMOverTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollUpButtonMDownTex),
		true,pCurScheme->textR,pCurScheme->textG,pCurScheme->textB);
	m_curElmt++;
	pCurElmt=AddElmt(m_noName);
	GLGUICreateButton(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		right,bottom,false,m_currentSettings.scrollButtonWidth,
		m_currentSettings.scrollButtonHeight,NULL,
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollDownButtonMDefTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollDownButtonMOverTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollDownButtonMDownTex),
		true,pCurScheme->textR,pCurScheme->textG,pCurScheme->textB);
	m_curElmt++;
	pCurElmt=AddElmt(m_noName);
	GLGUICreateSlider(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		right,bottom+m_currentSettings.scrollButtonHeight-1,m_currentSettings.scrollButtonWidth,
		height-m_currentSettings.scrollButtonHeight*2+2,SLIDERRENDERER_MAXVALUE,false,
		(char *)m_cTexMan->FindTextureName(m_currentSettings.sliderBackTexV),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.sliderButtonTex));
	NextElmt(width,height,start);
}
/**********************************************************************************

  Adds a single select list to the window being generated.

  NOTE: Does not maintain pointers to string data, memory management of 
  const char *elmtName and other string data is the responsibility of the
  user.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::AddSelectList(const char *elmtName,const char *text,DWORD flags)
{
	if (!m_isOk||elmtName==NULL||
		m_curFont==NULL||
		m_curStyle==NULL||
		m_curScheme==NULL)
		return;
	glguiElementData_t *pCurElmt=AddElmt(elmtName);
	if (pCurElmt==NULL)
		return;
	int start=m_curElmt;
	GLGUIWINDOWGENERATORfont_t *pCurFont=(GLGUIWINDOWGENERATORfont_t *)clist_data(m_curFont);
	GLGUIWINDOWGENERATORcolorScheme_t *pCurScheme=(GLGUIWINDOWGENERATORcolorScheme_t *)clist_data(m_curScheme);
	int sentWidth=((flags & GLGUI_FLAGWIDE) ? m_currentSettings.wideScrollBoxWidth :
					m_currentSettings.narrowScrollBoxWidth);
	int sentHeight=((flags & GLGUI_FLAGTALL) ? m_currentSettings.tallScrollBoxHeight :
					m_currentSettings.shortScrollBoxHeight);
	GLGUICreateSelectionList(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		0,m_curHeight,sentWidth,sentHeight,m_curElmt+1,m_curElmt+2,m_curElmt+3,
		m_currentSettings.scrollBoxPadding,!(flags & GLGUI_FLAGNOBACKGROUND),true,true,
		pCurScheme->backR,pCurScheme->backG,pCurScheme->backB,
		pCurScheme->textR,pCurScheme->textG,pCurScheme->textB,
		pCurScheme->hBackR,pCurScheme->hBackG,pCurScheme->hBackB,
		pCurScheme->hTextR,pCurScheme->hTextG,pCurScheme->hTextB,
		(char *)text,((flags & GLGUI_FLAGSUBMITBUTTON)!=0));
	pCurElmt->posX=m_rows[m_curRow].width;
	int left, top, right, bottom;
	GLGUICoordinateCalculator(left,top,right,bottom,pCurElmt);	
	int height = (top-bottom+1),
		width = (right-left+1)+m_currentSettings.scrollButtonWidth;
	m_curElmt++;
	pCurElmt=AddElmt(m_noName);
	GLGUICreateButton(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		right,top-(m_currentSettings.scrollButtonHeight-1),
		false,m_currentSettings.scrollButtonWidth,
		m_currentSettings.scrollButtonHeight,NULL,
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollUpButtonMDefTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollUpButtonMOverTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollUpButtonMDownTex),
		true,pCurScheme->textR,pCurScheme->textG,pCurScheme->textB);
	m_curElmt++;
	pCurElmt=AddElmt(m_noName);
	GLGUICreateButton(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		right,bottom,false,m_currentSettings.scrollButtonWidth,
		m_currentSettings.scrollButtonHeight,NULL,
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollDownButtonMDefTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollDownButtonMOverTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollDownButtonMDownTex),
		true,pCurScheme->textR,pCurScheme->textG,pCurScheme->textB);
	m_curElmt++;
	pCurElmt=AddElmt(m_noName);
	GLGUICreateSlider(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		right,bottom+m_currentSettings.scrollButtonHeight-1,m_currentSettings.scrollButtonWidth,
		height-m_currentSettings.scrollButtonHeight*2+2,SLIDERRENDERER_MAXVALUE,false,
		(char *)m_cTexMan->FindTextureName(m_currentSettings.sliderBackTexV),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.sliderButtonTex));
	NextElmt(width,height,start);
}
/**********************************************************************************

  Adds a drop menu to the window being generated.

  NOTE: Does not maintain pointers to string data, memory management of 
  const char *elmtName and other string data is the responsibility of the
  user.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::AddDropMenu(const char *elmtName,const char *text, DWORD flags)
{
	if (!m_isOk||elmtName==NULL||
		m_curFont==NULL||
		m_curStyle==NULL||
		m_curScheme==NULL)
		return;
	glguiElementData_t *pCurElmt=AddElmt(elmtName);
	if (pCurElmt==NULL)
		return;
	int start=m_curElmt;
	GLGUIWINDOWGENERATORfont_t *pCurFont=(GLGUIWINDOWGENERATORfont_t *)clist_data(m_curFont);
	GLGUIWINDOWGENERATORcolorScheme_t *pCurScheme=(GLGUIWINDOWGENERATORcolorScheme_t *)clist_data(m_curScheme);
	int sentWidth=((flags & GLGUI_FLAGWIDE) ? m_currentSettings.wideScrollBoxWidth :
					m_currentSettings.narrowScrollBoxWidth);
	int sentHeight=m_currentSettings.dropDownHeight;
	GLGUICreateDropDownList(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		0,m_curHeight,sentWidth,1,sentHeight,m_curElmt+1,m_curElmt+2,m_curElmt+3,
		m_currentSettings.dropDownPadding,!(flags & GLGUI_FLAGNOBACKGROUND),true,true,
		pCurScheme->backR,pCurScheme->backG,pCurScheme->backB,
		pCurScheme->textR,pCurScheme->textG,pCurScheme->textB,
		pCurScheme->hBackR,pCurScheme->hBackG,pCurScheme->hBackB,
		pCurScheme->hTextR,pCurScheme->hTextG,pCurScheme->hTextB,
		(char *)text,((flags & GLGUI_FLAGSUBMITBUTTON)!=0));
	pCurElmt->posX=m_rows[m_curRow].width;
	int left, top, right, bottom;
	GLGUICoordinateCalculator(left,top,right,bottom,pCurElmt);	
	int height = (top-bottom+1),
		width = (right-left+1)+m_currentSettings.scrollButtonWidth+m_currentSettings.openButtonWidth;
	m_curElmt++;
	pCurElmt=AddElmt(m_noName);
	GLGUICreateButton(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		right,top-(m_currentSettings.openButtonHeight-1),
		false,m_currentSettings.openButtonWidth,
		m_currentSettings.openButtonHeight,NULL,
		(char *)m_cTexMan->FindTextureName(m_currentSettings.openButtonMDefTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.openButtonMOverTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.openButtonMDownTex),
		true,pCurScheme->textR,pCurScheme->textG,pCurScheme->textB);
	GLGUICoordinateCalculator(left,top,right,bottom,pCurElmt);
	m_curElmt++;
	pCurElmt=AddElmt(m_noName);
	GLGUICreateButton(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		right,top-(m_currentSettings.scrollButtonHeight-1),
		false,m_currentSettings.scrollButtonWidth,
		m_currentSettings.scrollButtonHeight,NULL,
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollUpButtonMDefTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollUpButtonMOverTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollUpButtonMDownTex),
		true,pCurScheme->textR,pCurScheme->textG,pCurScheme->textB);
	m_curElmt++;
	pCurElmt=AddElmt(m_noName);
	GLGUICreateButton(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		right,bottom,false,m_currentSettings.scrollButtonWidth,
		m_currentSettings.scrollButtonHeight,NULL,
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollDownButtonMDefTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollDownButtonMOverTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.scrollDownButtonMDownTex),
		true,pCurScheme->textR,pCurScheme->textG,pCurScheme->textB);
	NextElmt(width,height,start);
}
/**********************************************************************************

  Adds a single standard button to the window being generated.

  NOTE: Does not maintain pointers to string data, memory management of 
  const char *elmtName and other string data is the responsibility of the
  user.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::AddStandardButton(const char *elmtName,const char *buttonText)
{
	if (!m_isOk||elmtName==NULL||
		m_curFont==NULL||
		m_curStyle==NULL||
		m_curScheme==NULL)
		return;
	glguiElementData_t *pCurElmt=AddElmt(elmtName);
	if (pCurElmt==NULL)
		return;
	GLGUIWINDOWGENERATORfont_t *pCurFont=(GLGUIWINDOWGENERATORfont_t *)clist_data(m_curFont);
	GLGUIWINDOWGENERATORcolorScheme_t *pCurScheme=(GLGUIWINDOWGENERATORcolorScheme_t *)clist_data(m_curScheme);
	GLGUICreateButton(pCurElmt,(char *)elmtName,pCurFont->fontListBase,
		GLGUITextDispWidthEstCalculator((char *)buttonText,pCurFont->textWidth),
		pCurFont->textHeight,
		0,m_curHeight,true,m_currentSettings.standardButtonWidth,
		m_currentSettings.standardButtonHeight,(char *)buttonText,
		(char *)m_cTexMan->FindTextureName(m_currentSettings.defaultButtonMDefTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.defaultButtonMOverTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.defaultButtonMDownTex),true,
		pCurScheme->bTextR,pCurScheme->bTextG,pCurScheme->bTextB);
	NextElmt();
}
/**********************************************************************************

  Adds a single standard button to the window being generated.

  NOTE: Does not maintain pointers to string data, memory management of 
  const char *elmtName and other string data is the responsibility of the
  user.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::AddCustomButton(const char *elmtName,const char *buttonText,
											int width,int height,char *defaultTexName,
											char *mouseOverTexName,char *mouseDownTexName,
											float textColorR,float textColorG,float textColorB)
{
	if (!m_isOk||elmtName==NULL||
		m_curFont==NULL||
		m_curStyle==NULL||
		m_curScheme==NULL)
		return;
	glguiElementData_t *pCurElmt=AddElmt(elmtName);
	if (pCurElmt==NULL)
		return;
	GLGUIWINDOWGENERATORfont_t *pCurFont=(GLGUIWINDOWGENERATORfont_t *)clist_data(m_curFont);
	GLGUIWINDOWGENERATORcolorScheme_t *pCurScheme=(GLGUIWINDOWGENERATORcolorScheme_t *)clist_data(m_curScheme);
	GLGUICreateButton(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,GLGUITextDispWidthEstCalculator((char *)buttonText,pCurFont->textWidth),
		pCurFont->textHeight,
		0,m_curHeight,true,width,height,(char *)buttonText,
		defaultTexName,mouseOverTexName,mouseDownTexName,true,
		textColorR,textColorG,textColorB);
	NextElmt();
}
/**********************************************************************************

  Adds a check box to the window being generated.

  NOTE: Does not maintain pointers to string data, memory management of 
  const char *elmtName and other string data is the responsibility of the
  user.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::AddCheckBox(const char *elmtName,const char *text, DWORD flags)
{
	if (!m_isOk||elmtName==NULL||
		m_curFont==NULL||
		m_curStyle==NULL||
		m_curScheme==NULL)
		return;
	glguiElementData_t *pCurElmt=AddElmt(m_noName);
	if (pCurElmt==NULL)
		return;
	int start=m_curElmt;
	GLGUIWINDOWGENERATORfont_t *pCurFont=(GLGUIWINDOWGENERATORfont_t *)clist_data(m_curFont);
	GLGUIWINDOWGENERATORcolorScheme_t *pCurScheme=(GLGUIWINDOWGENERATORcolorScheme_t *)clist_data(m_curScheme);
	GLGUICreateTextDisplay(pCurElmt,m_noName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		0,m_curHeight,m_currentSettings.textFeildPadding,!(flags & GLGUI_FLAGNOBACKGROUND),true,
		pCurScheme->backR,pCurScheme->backG,pCurScheme->backB,
		pCurScheme->textR,pCurScheme->textG,pCurScheme->textB,
		WordWrap(text,flags));
	pCurElmt->posX=m_rows[m_curRow].width+m_currentSettings.scrollButtonWidth;
	int left, top, right, bottom;
	GLGUICoordinateCalculator(left,top,right,bottom,pCurElmt);	
	int height = (top-bottom+1),
		width = (right-left+1)+m_currentSettings.scrollButtonWidth;
	m_curElmt++;
	pCurElmt=AddElmt(elmtName);
	GLGUICreateToggleButton(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		left-(m_currentSettings.scrollButtonWidth-1),top-(m_currentSettings.scrollButtonHeight-1),
		m_currentSettings.scrollButtonWidth,
		m_currentSettings.scrollButtonHeight,NULL,
		(char *)m_cTexMan->FindTextureName(m_currentSettings.checkBoxOffTex),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.checkBoxOnTex),
		true,pCurScheme->textR,pCurScheme->textG,pCurScheme->textB,
		((flags & GLGUI_FLAGISCHECKED)!=0));
	NextElmt(width,height,start);
}
/**********************************************************************************

  Adds a single slider to the window being generated.
  Size is the length of the slider in pixels.
  Position is its initial position within that size limit.

  NOTE: Does not maintain pointers to string data, memory management of 
  const char *elmtName and other string data is the responsibility of the
  user.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::AddSlider(const char *elmtName,int size, int position, bool isVertical)
{
	if (!m_isOk||elmtName==NULL||
		m_curFont==NULL||
		m_curStyle==NULL||
		m_curScheme==NULL)
		return;
	glguiElementData_t *pCurElmt=AddElmt(elmtName);
	if (pCurElmt==NULL)
		return;
	int width = (isVertical ? m_currentSettings.scrollButtonWidth : size);
	int height = (isVertical ? size : m_currentSettings.scrollButtonHeight);
	GLGUIWINDOWGENERATORfont_t *pCurFont=(GLGUIWINDOWGENERATORfont_t *)clist_data(m_curFont);
	GLGUIWINDOWGENERATORcolorScheme_t *pCurScheme=(GLGUIWINDOWGENERATORcolorScheme_t *)clist_data(m_curScheme);
	GLGUICreateSlider(pCurElmt,(char *)elmtName,
		pCurFont->fontListBase,pCurFont->textWidth,pCurFont->textHeight,
		0,m_curHeight,width,height,(position*SLIDERRENDERER_MAXVALUE)/size,!isVertical,
		(char *)m_cTexMan->FindTextureName((isVertical ? m_currentSettings.sliderBackTexV : m_currentSettings.sliderBackTexH)),
		(char *)m_cTexMan->FindTextureName(m_currentSettings.sliderButtonTex));
	NextElmt();
}
/**********************************************************************************

  Adds a image to the window being generated.

  Set texName to NULL for a blank space.

  NOTE: Does not maintain pointers to string data, memory management of 
  const char *elmtName and other string data is the responsibility of the
  user.

**********************************************************************************/
void CGLGUIWINDOWGENERATOR::AddImage(const char *elmtName,const char *texName,int width,int height)
{
	if (!m_isOk||elmtName==NULL||
		m_curFont==NULL||
		m_curStyle==NULL||
		m_curScheme==NULL)
		return;
	glguiElementData_t *pCurElmt=AddElmt(elmtName);
	if (pCurElmt==NULL)
		return;
	GLGUIWINDOWGENERATORfont_t *pCurFont=(GLGUIWINDOWGENERATORfont_t *)clist_data(m_curFont);
	GLGUIWINDOWGENERATORcolorScheme_t *pCurScheme=(GLGUIWINDOWGENERATORcolorScheme_t *)clist_data(m_curScheme);
	GLGUICreateImage(pCurElmt,(char *)elmtName,pCurFont->fontListBase,
		pCurFont->textWidth,pCurFont->textHeight,
		0,m_curHeight,width,height,texName);
	NextElmt();
}


/***********************************************************************************

  Creates a new window with generated description and then clears all window
  data.  To destroy window call use delete
  IE: delete myWindow;

  Returns NULL on failure.

***********************************************************************************/
CGLGUIWindow *CGLGUIWINDOWGENERATOR::ReturnWindow(int xPos, int yPos)
{
	if (!m_isOk)
		return NULL;
	if (m_winDesc==NULL)
		return NULL;
	NextColumn();
	GLGUIWINDOWGENERATORwindowStyle_t *pCurStyle=(GLGUIWINDOWGENERATORwindowStyle_t *)clist_data(m_curStyle);
	int width = m_curX;
	int height = m_curWindowHeight;
	int selected=-1;
	if (m_winElmtCount>1)
	{ // searches for the first acceptable element to select and selects it.
		for (int i=0; i<m_winElmtCount; i++)
			if (m_winDesc[i].elementType==GLGUI_ELEMENT_TEXTFEILD)
			{
				selected=i;
				break;
			}
		if (selected==-1)
			for (i=0; i<m_winElmtCount; i++)
				if (m_winDesc[i].elementType==GLGUI_ELEMENT_SELECTLIST)
				{
					selected=i;
					break;
				}
	}
	if (selected==-1)
		selected=0;

	int lBorder = (pCurStyle->leftBorderWidth*width)/128;
	int bBorder = (pCurStyle->bottomBorderWidth*height)/128;
	int tBorder = (pCurStyle->topBorderWidth*height)/128;
	int rBorder = (pCurStyle->rightBorderWidth*width)/128;
	for (int i=0; i<m_winElmtCount; i++)
	{
		m_winDesc[i].posX+=lBorder-(pCurStyle->leftBorderWidth);
		m_winDesc[i].posY+=bBorder-pCurStyle->bottomBorderWidth;
	}
	width=m_curX+lBorder+rBorder;
	height=m_curWindowHeight+bBorder+tBorder;/**/
	CGLGUIWindow *newWindow=NULL;
	newWindow=new CGLGUIWindow(m_cLog,m_cTexMan,xPos,yPos,width,height,
		(char *)m_cTexMan->FindTextureName(pCurStyle->texture),pCurStyle->tileFactor,m_winDesc,m_winElmtCount,
		selected);
	ClearData();
	return newWindow;
}
/***********************************************************************************

  Clears all window generation data.

***********************************************************************************/
void CGLGUIWINDOWGENERATOR::ClearData()
{
	if (m_winDesc!=NULL)
	{
		free(m_winDesc);
		m_winDesc=NULL;
	}
	m_winElmtCount=0;
	m_curWidth=0;
	m_curHeight=0;
	m_curElmt=0;
	m_curColStart=0;
	m_curRow=0;
	m_curX=0;
	m_curWindowHeight=0;
	m_isRow=false;
	for (int i=0; i<GLGUIWINDOWGENERATOR_MAXROWS; i++)
	{
		m_rows[i].end=0;
		m_rows[i].height=0;
		m_rows[i].start=0;
		m_rows[i].width=0;
	}
	clist_destroy(&m_stringList);
}
/***********************************************************************************

  Performs word wrap and returns result.

***********************************************************************************/
char *CGLGUIWINDOWGENERATOR::WordWrap(const char *text,DWORD flags)
{
	int start=m_curElmt;
	char *newString=NULL;
	{
		m_stringList;		
		int *indexes=NULL;
		StrUtil_WordWrapS(STRUTIL_DEFBREAKCHARS,text,((flags & GLGUI_FLAGWIDE) ? m_currentSettings.wideScrollBoxWidth :
							m_currentSettings.narrowScrollBoxWidth)-4,&newString,indexes);
		m_curString=clist_head(&m_stringList);
		clist_ins_next(&m_stringList,m_curString,newString);
		if (indexes!=NULL)
			free(indexes);
	}
	return newString;
}
/***********************************************************************************

  Returns false if the system has had an error along the way.

***********************************************************************************/
bool CGLGUIWINDOWGENERATOR::IsOk()
{
	return m_isOk;
}