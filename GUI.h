#include "log.h"
#include "texture manager.h"
#define GUI_OBJECTTYPE_TABLE		0
#define GUI_OBJECTTYPE_TEXTFEILD	1
#define GUI_OBJECTTYPE_TEXTAREA		2
#define GUI_OBJECTTYPE_OPTIONLIST	3
#define GUI_OBJECTTYPE_IMAGE		4
#define GUI_OBJECTTYPE_TEXT			5
#define GUI_OBJECTTYPE_RENDEROBJECT	6
#define GUI_POSITION_CENTERED	-1

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
	int	width; // for text feild/area or option list width in chars
	int height; // for text area or option list height in lines
	int textureIndex; // -1 if no texture is to be applied (makes images into spacers)
	int mouseOverTexture; // -1 if none
	int mouseDownTexture; // -1 if none
	float colorRGB[3]; // color
	float selectedColorRGB[3]; // color when selected
	float mouseOverColorRGB[3]; // color when the cursor is over it
	int cellSpacing; // tables only
	int tabIndex; // make sure no repeates occur
	char *string; // centered in middle of object
	int selectedIndex; // only for option lists set to 0 if you have no default
	void (*render)(float elapsedTime,const GUIObjectState_t *state);// set to NULL if you have no rendering function
} GUIObjectData_t;
// table cells can hold only one object which can even be another table
class CGUIWindowGenerator
{
public:
	CGUIWindowGenerator(CLog **cLog, CTextureManager **cTexMan);
	~CGUIWindowGenerator();
	bool Initialize(GUIObjectData_t *windowDescription,int numberOfObjects,
		int positionX, int positionY,int screenWidth, int screenHeight,
		int textMaxHeight, int textMaxWidth, int font);
	void Render(float elapsedTime=0.0f);
	int	 UpdateInput(const char nextCharacter,int cursorX,int cursorY,bool mouseButton);
	void KillWindow();
protected:
	void SetUpCoordinates(int indexOfLastTable);
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
CGUIWindowGenerator::CGUIWindowGenerator(CLog **cLog,CTextureManager **cTexMan)
{
	m_cLog	  = *cLog;
	m_cTexMan = *cTexMan;
	m_objectList = NULL;
	m_objectCount = 0;
}
/**************************************************

  Class destructor.

**************************************************/
CGUIWindowGenerator::~CGUIWindowGenerator()
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
void CGUIWindowGenerator::KillWindow()
{
	if (NULL==m_objectList)
	{
		m_cLog->lprintf("CGUIWindowGenerator error: attempt to kill window with"
			" m_objectList already a NULL pointer.");
		return;
	}
	if (NULL==m_objectStateList)
	{
		m_cLog->lprintf(CGUIWindowGenerator error: null pointer in a kill window call.");
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
		m_cLog->lprintf("CGUIWindowGenerator error: KillWindow call failed during the"
			" freeing of allocated memory.");
	}
}
/**************************************************

  Initializes System.

**************************************************/
bool CGUIWindowGenerator::Initialize(GUIObjectData_t *windowDescription, int numberOfObjects,
								int positionX,int positionY,int screenWidth,int screenHeight,
								int textMaxHeight, int textMaxWidth, int font)
{
	try
	{
		if (NULL==windowDescription || numberOfObjects<2)
		{
			m_cLog->lprintf("CGUIWindowGenerator Received Invalid data.");
			return false;
		}
		m_objectList = new GUIObjectData_t[numberOfObjects];
		m_objectStateList = new GUIObjectStateList_t[numberOfObjects];
		if (NULL==m_objectList || NULL==m_objectStateList)
		{
			m_cLog->lprintf("CGUIWindowGenerator failed memory allocation of %i objects.",
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
					m_cLog->lprintf("CGUIWindowGenerator malloc of string index %i failed.",i);
					return false;
				}
				strcpy(m_objectList[i].string,windowDescription[i].string);
			}
			expectedTally+=i;
			tabIndexTally+=windowDescription[i].tabIndex;
		}
		m_objectCount		= numberOfObjects;
		m_windowPositionX	= positionX;
		m_windowPositionY	= positionY;
		m_screenWidth		= screenWidth;
		m_screenHeight		= screenHeight;
		m_textMaxHeight		= textMaxHeight;
		m_textMaxWidth		= textMaxWidth;
		m_fontListBase		= font;
		m_selectedTabIndex	= 0;
		if (tabIndexTally!=expectedTally)
		{
			m_cLog->lprintf("CGUIWindowGenerator error: specified tab indexes are incorrect.  "
				"Using default tab indexes.");
			for (int i=0; i<m_objectCount; i++)
				m_objectList[i].tabIndex=i;
		}
		if (m_objectList[0].GUIobjectType!=GUI_OBJECTTYPE_TABLE)
		{
			m_cLog->lprintf("CGUIWindowGenerator error: first object type is not a table.");
			return false;
		}
	}
	catch (...)
	{
		m_cLog->lprintf("CGUIWindowGenerator initialization failed!");
		throw;
	}
	// set drawing coordinates
	m_objectStateList[0].top=0;
	m_objectStateList[0].left=0;
	SetUpCoordinates(0);
	/*
	for (int i=0; i<m_objectCount; i++)
	{
		if (m_objectList[i].GUIobjectType==GUI_OBJECTTYPE_TABLE)
		{
			int tableIndex
			i++;
			switch (m_objectList[i].GUIobjectType)
			{
			case GUI_OBJECTTYPE_TEXTFEILD:
				m_objectList*/
/*
#define GUI_OBJECTTYPE_TABLE		0
#define GUI_OBJECTTYPE_TEXTFEILD	1
#define GUI_OBJECTTYPE_TEXTAREA		2
#define GUI_OBJECTTYPE_OPTIONLIST	3
#define GUI_OBJECTTYPE_IMAGE		4
#define GUI_OBJECTTYPE_TEXT			5
#define GUI_OBJECTTYPE_RENDEROBJECT	6*/

	///////////////////////////
	return true;
}
void CGUIWindowGenerator::SetUpCoordinates(int indexOfLastTable)
{
	if ((indexOfLastTable+1)>=m_objectCount)
		return;
	int xPos=m_objectList[indexOfLastTable].cellSpacing+
		m_objectStateList[indexOfLastTable].left;
	int yPos=m_objectList[indexOfLastTable].cellSpacing+
		m_objectStateList[indexOfLastTable].top;
	int cell=0;
	int cellHeight=0;
	int maxWidth=0;
	for (int i=(indexOfLastTable+1); i<m_objectCount; i++)
	{
		if (cell>=m_objectList[indexOfLastTable].width)
		{
			cell=0;
			if (xPos>maxWidth)
				maxWidth=xPos;
			xPos=m_objectList[indexOfLastTable].cellSpacing+
				m_objectStateList[indexOfLastTable].left;
			yPos+=cellHeight;
			cellHeight=0;
		}
		switch (m_objectList[i].GUIobjectType)
		{
		case GUI_OBJECTTYPE_TEXTFEILD:
			m_objectStateList[i].top=yPos;
			m_objectStateList[i].left=xPos;

			break;
		default:
			return;
		}
		cell++;
	}
}

/***************************************************

  Renders Window for a 2-D Orthographic Projection
  Setup.

***************************************************/
void CGUIWindowGenerator::Render(float elapsedTime)
{
	if (NULL!=m_objectList[0].render)
		*m_objectList
