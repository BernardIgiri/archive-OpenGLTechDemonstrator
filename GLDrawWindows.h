#include "log.h"
#include "texture manager.h"
#define GUI_OBJECTTYPE_TEXTFEILD	1
#define GUI_OBJECTTYPE_OPTIONLIST	2
#define GUI_OBJECTTYPE_IMAGE		3
#define GUI_OBJECTTYPE_RENDEROBJECT	4

typedef struct
{
	bool mouseOver; // true if mouse is on top of it
	bool mouseDown; // true if it mouse button is down while on top of it
	bool selected; // true if selected
	int top; // y coordinate of top
	int left; // x coordinate of left
	int right; // x coordinate of right
	int bottom; // y coordinate of bottom
	int index; // index of object within window
} GUIObjectState_t;

typedef struct
{
	int GUIobjectType; // GUI_OBJECTTYPE_x TABLE/TEXTFEILD/TEXTAREA/OPTIONLIST/IMAGE
	int xOffset; // xOffset from left of window
	int yOffset; // yOffset from top of window
	int	width; // for text feild/area or option list width in chars
	int height; // for text area or option list height in lines
	int textureIndex; // -1 if no texture is to be applied (makes images into spacers)
	int mouseOverTexture; // -1 if none
	int mouseDownTexture; // -1 if none
	float colorRGB[3]; // color
	float selectedColorRGB[3]; // color when selected
	float mouseOverColorRGB[3]; // color when the cursor is over it
	int tabIndex; // make sure no repeates occur
	char *string; // text to be centered in middle of object, NULL if none
	int selectedIndex; // only for option lists set to 0 if you have no default
	void (*render)(float elapsedTime,const GUIObjectState_t *state);// set to NULL if you have no rendering function
} GUIObjectData_t;
// table cells can hold only one object which can even be another table
class CGLDrawWindows
{
public:
	CGLDrawWindows(CLog **cLog, CTextureManager **cTexMan);
	~CGLDrawWindows();
	bool Initialize(GUIObjectData_t *windowDescription,int numberOfObjects,
		int textMaxHeight, int textMaxWidth, int font);
	void Render(float elapsedTime=0.0f,int xPos, int yPos);
	int	 UpdateInput(const char nextCharacter,int cursorX,int cursorY,bool mouseButton);
	void KillWindow();
protected:
	CLog			*m_cLog;
	CTextureManager	*m_cTexMan;
	GUIObjectData_t *m_objectList;
	GUIObjectState_t*m_objectStateList;
	int				 m_objectCount;
	int				 m_windowPositionX,
					 m_windowPositionY,
					 m_screenWidth,
					 m_screenHeight,
					 m_textMaxHeight,
					 m_textMaxWidth,
					 m_fontListBase,
					 m_selectedTabIndex;
};
/**************************************************

  Class constructor.

**************************************************/
CGLDrawWindows::CGLDrawWindows(CLog **cLog,CTextureManager **cTexMan)
{
	m_cLog	  = *cLog;
	m_cTexMan = *cTexMan;
	m_objectList = NULL;
	m_objectCount = 0;
}
/**************************************************

  Class destructor.

**************************************************/
CGLDrawWindows::~CGLDrawWindows()
{
	if (m_objectList!=NULL)
		KillWindow();
	m_cLog	  = NULL
	m_cTexMan = NULL;
	m_objectList = NULL;
	m_objectStateList = NULL;
}
/**************************************************

  Shuts system down freeing allocated memory.

**************************************************/
void CGLDrawWindows::KillWindow()
{
	if (NULL==m_objectList)
	{
		m_cLog->lprintf("CGLDrawWindows error: attempt to kill window with"
			" m_objectList already a NULL pointer.");
		return;
	}
	if (NULL==m_objectStateList)
	{
		m_cLog->lprintf(CGLDrawWindows error: null pointer in a kill window call.");
			return;
	}
	try
	{
		delete [] m_objectStateList;
		for (int i=0; i<m_objectCount; i++)
			free(m_objectList[i].string);
		delete [] m_objectList;
		m_objectStateList=NULL;
		m_objectList=NULL;
	}
	catch (...)
	{
		m_cLog->lprintf("CGLDrawWindows error: KillWindow call failed during the"
			" freeing of allocated memory.");
	}
}
/**************************************************

  Initializes System.

**************************************************/
bool CGLDrawWindows::Initialize(GUIObjectData_t *windowDescription, int numberOfObjects,
								int positionX,int positionY,int screenWidth,int screenHeight,
								int textMaxHeight, int textMaxWidth, int font)
{
	try
	{
		if (NULL==windowDescription || numberOfObjects<2)
		{
			m_cLog->lprintf("CGLDrawWindows Received Invalid data.");
			return false;
		}
		m_objectList = new GUIObjectData_t[numberOfObjects];
		m_objectStateList = new GUIObjectStateList_t[numberOfObjects];
		if (NULL==m_objectList || NULL==m_objectStateList)
		{
			m_cLog->lprintf("CGLDrawWindows failed memory allocation of %i objects.",
				numberOfObjects);
			return false;
		}
		memcpy(m_objectList,windowDescription,numberOfObjects*sizeof(GUIObjectData_t));
		int tabIndexTally=0,expectedTally=0;
		for (int i=0; i<numberOfObjects;i++)
		{
			m_objectList[i].string = NULL;
			if (NULL!=windowDescription[i].string) // if there is no string don't allocate for it
			{
				m_objectList[i].string = (char*)malloc(strlen(windowDescription[i].string)+1);
				if (NULL==m_objectList[i].string)
				{
					m_cLog->lprintf("CGLDrawWindows malloc of string index %i failed.",i);
					return false;
				}
				strcpy(m_objectList[i].string,windowDescription[i].string);
			}
			expectedTally+=i;
			tabIndexTally+=windowDescription[i].tabIndex;
		}
		m_objectCount		= numberOfObjects;/*
		m_windowPositionX	= positionX;
		m_windowPositionY	= positionY;
		m_screenWidth		= screenWidth;
		m_screenHeight		= screenHeight;*/
		m_textMaxHeight		= textMaxHeight;
		m_textMaxWidth		= textMaxWidth;
		m_fontListBase		= font;
		m_selectedTabIndex	= 0;
		if (tabIndexTally!=expectedTally)
		{
			m_cLog->lprintf("CGLDrawWindows error: specified tab indexes are incorrect.  "
				"Using default tab indexes.");
			for (int i=0; i<m_objectCount; i++)
				m_objectList[i].tabIndex=i;
		}
		/*if (m_objectList[0].GUIobjectType!=GUI_OBJECTTYPE_TABLE)
		{
			m_cLog->lprintf("CGLDrawWindows error: first object type is not a table.");
			return false;
		}*/
	}
	catch (...)
	{
		m_cLog->lprintf("CGLDrawWindows initialization failed!");
		throw;
	}
	return true;
}


/***************************************************

  Renders Window for a 2-D Orthographic Projection
  Setup.

***************************************************/
void CGLDrawWindows::Render(float elapsedTime,int xPos, int yPos)
{
	for (int i=0;i<m_objectCount;i++)
	{
		m_objectStateList[i].top=yPos+m_objectList[i].yOffset;
		m_objectStateList[i].left=xPos+m_objectList[i].xOffset;
		m_objectStateList[i].right=m_objectStateList[i].left+m_objectList[i].width-1;
		m_objectStateList[i].bottom=m_objectStateList[i].top+m_objectList[i].height-1;
		if (NULL!=m_objectList[i].render)
			*m_objectList[i].render(elapsedTime,&m_objectStateList[i]);
		else
		{
			switch (m_objectList[i].GUIobjectType)
			{
			case GUI_OBJECTTYPE_TEXTFEILD:
#define GUI_OBJECTTYPE_TEXTFEILD	1
#define GUI_OBJECTTYPE_OPTIONLIST	2
#define GUI_OBJECTTYPE_IMAGE		3
#define GUI_OBJECTTYPE_RENDEROBJECT	4
