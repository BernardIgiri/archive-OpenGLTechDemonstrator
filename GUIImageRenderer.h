#ifndef __INCLUDED_GUIIMAGERENDERER__
#define __INCLUDED_GUIIMAGERENDERER__
#include "log.h"
#include "texture manager.h"

class CGUIImageRenderer
{
public:
	CGUIImageRenderer(const char *texName,int xPos,int yPos,int width,int height,CLog *cLog, CTextureManager *cTexMan);
	~CGUIImageRenderer();
	void Render();
	bool IsMouseOver(int cursorX, int cursorY);
	void ChangeImage(const char *texName);
	void GetCoordinates(int &left,int &top,int &right,int &bottom);
	void MoveTo(int x,int y);
protected:
	int m_minX,
		m_maxX,
		m_minY,
		m_maxY,
		m_texture;
	bool m_isAlpha;
	bool m_render;
	CLog *m_cLog;
	CTextureManager *m_cTexMan;
};
void GUIImageRendererCoordinateCalculater(int &dLeft, int &dTop, int &dRight, int &dBottom,
										  int xPos,int yPos,int width,int height);
#endif
