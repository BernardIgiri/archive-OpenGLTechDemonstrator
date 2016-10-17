#ifndef __INCLUDED_SLIDERRENDERER__
#include "texture manager.h"
#include "log.h"

#define SLIDERRENDERER_MAXVALUE 10000
#define SLIDERRENDERER_BUTTONWIDTH 5

class CSliderRenderer
{
public:
	CSliderRenderer(int xPos, int yPos, int width, int height, bool isHorizontal,
								 const char *backTexName, const char *buttonTexName,
								 CLog *cLog, CTextureManager *cTexMan);
	~CSliderRenderer();
	int Update(int cursorX, int cursorY, bool mouseButton);
	void MoveTo(int xPos, int yPos);
	void SetPosition(int newPosition);
	void GetCoordinates(int &left, int &top, int &right, int &bottom);
	bool IsOver(int cursorX, int cursorY);
protected:
	bool m_isHorizontal,
		 m_backIsRGBA,
		 m_buttonIsRGBA;
	int m_maxX, m_minX, m_maxY, m_minY,
		m_width, m_height,
		m_position, m_maxPosition,
		m_backTexture, m_buttonTexture;
	CLog			*m_cLog;
	CTextureManager	*m_cTexMan;
};

void SliderRendererCoordinateCalculator(int &left, int &top, int &right, int &bottom,
									  int xPos, int yPos, int width, int height);
#endif