#include "GLGUIWindowRenderer.h"
#include <stdlib.h>
#include <string.h>
/***********************************************************************

  Creates Window from description.

***********************************************************************/
CGLGUIWindow::CGLGUIWindow(CLog *cLog, CTextureManager *cTexMan, int left,int bottom, int width,
						   int height,char *backgroundTexture,int backgroundTileFactor,
						   glguiElementData_t *windowDescription,int numberOfElements,int initialSelect)
{
	m_redrawAll	=true;
	m_tileFactor=backgroundTileFactor;
	m_cLog		=cLog;
	m_cTexMan	=cTexMan;
	m_bottom	=bottom;
	m_left		=left;
	m_width		=width;
	m_height	=height;
	m_selectedIndex=initialSelect%numberOfElements;
	m_pressedButton=-1;
	m_numOfElements=numberOfElements;
	m_elementList=NULL;
	try
	{
		if (backgroundTexture!=NULL)
			m_backTexID = cTexMan->FindTextureIndex(backgroundTexture);
		else
			m_backTexID =-1;
		m_elementList = (glguiElementData_t *)malloc(sizeof(glguiElementData_t)*numberOfElements);
		if (m_elementList==NULL)
		{
			m_cLog->lprintf("GLGUIWindow error: failed to allocated needed ram.");
			return;
		}
		memcpy(m_elementList,windowDescription,sizeof(glguiElementData_t)*numberOfElements);
		glguiElementData_t *curElmt=NULL;
		for (int i=0; i<m_numOfElements; i++)
		{
			if (numberOfElements<2)
				curElmt=m_elementList;
			else
				curElmt=&m_elementList[i];
			{
				char *nameCpy=curElmt->elementName;
				if (nameCpy!=NULL)
				{
					curElmt->elementName=NULL;
					curElmt->elementName=(char *)malloc(strlen(nameCpy)+1);
					if (curElmt->elementName!=NULL)
						strcpy(curElmt->elementName,nameCpy);
				}
			}
			switch (curElmt->elementType)
			{
			case GLGUI_ELEMENT_TEXTFEILD:
				curElmt->element.textFeild.pElement=new CTextFeildRenderer(curElmt->font,
					curElmt->element.textFeild.displayWidth,
					curElmt->element.textFeild.maxChars,
					curElmt->textHeight,
					curElmt->textWidth,
					curElmt->posY+m_bottom,
					curElmt->posX+m_left,
					curElmt->element.textFeild.padding,
					&m_cLog);
				if (curElmt->element.textFeild.setColor)
					curElmt->element.textFeild.pElement->SetColor(curElmt->element.textFeild.backgroundColor[0],
						curElmt->element.textFeild.backgroundColor[1],
						curElmt->element.textFeild.backgroundColor[2],
						curElmt->element.textFeild.textColor[0],
						curElmt->element.textFeild.textColor[1],
						curElmt->element.textFeild.textColor[2]);
				if (curElmt->element.textFeild.initialText!=NULL)
					curElmt->element.textFeild.pElement->SetTextString(curElmt->element.textFeild.initialText);
				break;
			case GLGUI_ELEMENT_SLIDER:
				curElmt->element.slider.pElement=new CSliderRenderer(curElmt->posX+m_left,
					curElmt->posY+m_bottom,
					curElmt->element.slider.width,
					curElmt->element.slider.height,
					curElmt->element.slider.isHorizontal,
					curElmt->element.slider.backTexName,
					curElmt->element.slider.buttonTexName,
					m_cLog,
					m_cTexMan);
				curElmt->element.slider.pElement->SetPosition(curElmt->element.slider.position);
				break;
			case GLGUI_ELEMENT_IMAGE:
				curElmt->element.image.pElement=new CGUIImageRenderer(curElmt->element.image.texName,
					curElmt->posX+m_left,curElmt->posY+m_bottom,
					curElmt->element.image.width,
					curElmt->element.image.height,
					m_cLog,m_cTexMan);
				break;
			case GLGUI_ELEMENT_BUTTON:
				curElmt->element.button.pElement=new CButtonRenderer(curElmt->posY+m_bottom,
					curElmt->posX+m_left,
					curElmt->element.button.width,
					curElmt->element.button.height,
					curElmt->element.button.buttonText,
					curElmt->textWidth,
					curElmt->textHeight,
					curElmt->element.button.defaultTexName,
					curElmt->element.button.mouseOverTexName,
					curElmt->element.button.mouseDownTexName,
					curElmt->font,
					m_cLog,
					m_cTexMan);
				if (curElmt->element.button.setColor)
					curElmt->element.button.pElement->ChangeTextColor(curElmt->element.button.textColor[0],
					curElmt->element.button.textColor[1],curElmt->element.button.textColor[2]);
				break;
			case GLGUI_ELEMENT_TOGGLEBUTTON:
				curElmt->element.toggleButton.pElement=new CButtonRenderer(curElmt->posY+m_bottom,
					curElmt->posX+m_left,
					curElmt->element.toggleButton.width,
					curElmt->element.toggleButton.height,
					curElmt->element.toggleButton.buttonText,
					curElmt->textWidth,
					curElmt->textHeight,
					curElmt->element.toggleButton.defaultTexName,
					curElmt->element.toggleButton.defaultTexName,
					curElmt->element.toggleButton.OnTexName,
					curElmt->font,
					m_cLog,
					m_cTexMan, true);
				if (curElmt->element.toggleButton.setColor)
					curElmt->element.toggleButton.pElement->ChangeTextColor(curElmt->element.toggleButton.textColor[0],
					curElmt->element.toggleButton.textColor[1],curElmt->element.toggleButton.textColor[2]);
				break;
			case GLGUI_ELEMENT_SELECTLIST:
				curElmt->element.selectList.pElement=new CSelectionListRenderer(curElmt->font,
					curElmt->element.selectList.displayWidth,
					curElmt->element.selectList.displayHeight,
					curElmt->textHeight,
					curElmt->textWidth,
					curElmt->posY+m_bottom,
					curElmt->posX+m_left,
					curElmt->element.selectList.padding,
					curElmt->element.selectList.listText,
					&m_cLog);
				if (curElmt->element.selectList.setColor)
				{
					curElmt->element.selectList.pElement->SetColor(curElmt->element.selectList.backgroundColor[0],
						curElmt->element.selectList.backgroundColor[1],
						curElmt->element.selectList.backgroundColor[2],
						curElmt->element.selectList.textColor[0],
						curElmt->element.selectList.textColor[1],
						curElmt->element.selectList.textColor[2]);
					curElmt->element.selectList.pElement->SetHighlightColor(
						curElmt->element.selectList.highlightBackgroundColor[0],
						curElmt->element.selectList.highlightBackgroundColor[1],
						curElmt->element.selectList.highlightBackgroundColor[2],
						curElmt->element.selectList.highlightTextColor[0],
						curElmt->element.selectList.highlightTextColor[1],
						curElmt->element.selectList.highlightTextColor[2]);
				}
				break;
			case GLGUI_ELEMENT_DROPDOWNLIST:
				curElmt->element.dropDownList.pElement=new CSelectionListRenderer(curElmt->font,
					curElmt->element.dropDownList.displayWidth,
					curElmt->element.dropDownList.displayHeight,
					curElmt->textHeight,
					curElmt->textWidth,
					curElmt->posY+m_bottom,
					curElmt->posX+m_left,
					curElmt->element.dropDownList.padding,
					curElmt->element.dropDownList.listText,
					&m_cLog);
				if (curElmt->element.dropDownList.setColor)
				{
					curElmt->element.dropDownList.pElement->SetColor(curElmt->element.dropDownList.backgroundColor[0],
						curElmt->element.dropDownList.backgroundColor[1],
						curElmt->element.dropDownList.backgroundColor[2],
						curElmt->element.dropDownList.textColor[0],
						curElmt->element.dropDownList.textColor[1],
						curElmt->element.dropDownList.textColor[2]);
					curElmt->element.dropDownList.pElement->SetHighlightColor(
						curElmt->element.dropDownList.highlightBackgroundColor[0],
						curElmt->element.dropDownList.highlightBackgroundColor[1],
						curElmt->element.dropDownList.highlightBackgroundColor[2],
						curElmt->element.dropDownList.highlightTextColor[0],
						curElmt->element.dropDownList.highlightTextColor[1],
						curElmt->element.dropDownList.highlightTextColor[2]);
				}
				{
					int pX, p1Y, p2Y;
					int n, y1;
					curElmt->element.dropDownList.pElement->ChangeDisplayHeight(curElmt->element.dropDownList.maxHeight);
					curElmt->element.dropDownList.pElement->GetCoordinates(pX,p1Y,n,y1);
					y1=(p1Y-y1);
						
					int  y2;
					curElmt->element.dropDownList.pElement->ChangeDisplayHeight(curElmt->element.dropDownList.displayHeight);
					curElmt->element.dropDownList.pElement->GetCoordinates(n,y2,n,n);

					p2Y=p1Y+(p1Y-y2);
					p2Y-=y1;
					p1Y-=y1;
					curElmt->element.dropDownList.bX=pX;
					curElmt->element.dropDownList.y2=p1Y;
					curElmt->posY=p2Y;

					curElmt->element.dropDownList.y2-=(p2Y-p1Y);
					curElmt->posY-=(p2Y-p1Y);
				}
				break;
			default:
				break;
			}
		}
		/////// SELECT INDEX SETTING
		for (i=0; i<m_numOfElements; i++)
		{
			if (numberOfElements<2)
				curElmt=m_elementList;
			else
				curElmt=&m_elementList[i];
			if (curElmt->elementType==GLGUI_ELEMENT_BUTTON||curElmt->elementType==GLGUI_ELEMENT_SLIDER)
			{
				int sIndex=-1;
				int checkIndex=i;
				glguiElementData_t *checkElmt=NULL;
				for (int i2=0; i2<m_numOfElements; i2++)
				{
					if (numberOfElements<2)
						checkElmt=m_elementList;
					else
						checkElmt=&m_elementList[i2];
					switch (checkElmt->elementType)
					{
					case GLGUI_ELEMENT_SELECTLIST:
						if (checkElmt->element.selectList.scrollDownIndex==checkIndex ||
							checkElmt->element.selectList.scrollUpIndex  ==checkIndex)
							sIndex=i2;
						break;
					case GLGUI_ELEMENT_DROPDOWNLIST:
						if (checkElmt->element.dropDownList.scrollDownIndex==checkIndex ||
							checkElmt->element.dropDownList.scrollUpIndex  ==checkIndex ||
							checkElmt->element.dropDownList.openIndex	   ==checkIndex)
							sIndex=i2;
						break;
					default:
						break;
					}
				}
				if (curElmt->elementType==GLGUI_ELEMENT_BUTTON)
					curElmt->element.button.selectIndex=sIndex;
				else if (curElmt->elementType==GLGUI_ELEMENT_SLIDER)
					curElmt->element.slider.selectIndex=sIndex;
			}
		}
		if (m_backTexID>0)
		{
			m_cTexMan->SelectTexture(m_backTexID);
			m_isAlpha =(m_cTexMan->GetTexBpp(m_backTexID)>24);
		}
	}
	catch (...)
	{
		m_cLog->lprintf("CGLGUIWindow error: failed to create window.");
		throw;
	}
}

bool CGLGUIWindow::UpdateAndRender(const char nextCharacter,int cursorX,int cursorY,bool mouseButton,float elapsedTime,bool redrawAll)
{
	bool result=false;
	m_pressedButton=-1;
	switch (nextCharacter)
	{
	case INPUTMAN_TEXT_CODE_TAB:
		m_selectedIndex++;
		break;
	case INPUTMAN_TEXT_CODE_SHIFTTAB:
		m_selectedIndex--;
		break;
	case INPUTMAN_TEXT_CODE_ENTER:
		result=true;
		break;
	case INPUTMAN_TEXT_CODE_UP:
		if (m_numOfElements>1)
		{
			if (m_elementList[m_selectedIndex].elementType!=GLGUI_ELEMENT_SELECTLIST&&
				m_elementList[m_selectedIndex].elementType!=GLGUI_ELEMENT_DROPDOWNLIST)
				m_selectedIndex++;
		}
		break;
	case INPUTMAN_TEXT_CODE_DOWN:
		if (m_numOfElements>1)
		{
			if (m_elementList[m_selectedIndex].elementType!=GLGUI_ELEMENT_SELECTLIST&&
				m_elementList[m_selectedIndex].elementType!=GLGUI_ELEMENT_DROPDOWNLIST)
				m_selectedIndex--;
		}
		break;
	default:
		break;
	}
	if (result)
		return true;
	if (redrawAll)
		m_redrawAll=true;
	bool drawBg=false;
	if (m_redrawAll)
	{
		if (m_backTexID>=0)
		{
			glEnable(GL_TEXTURE_2D);
			m_cTexMan->SelectTexture(m_backTexID);			
			if (m_isAlpha)
			{
				glEnable(GL_BLEND);
				glDepthMask(GL_FALSE);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			glBegin(GL_QUADS);
				glNormal3f(0.0f,0.0f,1.0f*(float)m_tileFactor);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2i(m_left, m_bottom+(m_height-1));
				glTexCoord2f(0.0f, 1.0f*(float)m_tileFactor);
				glVertex2i(m_left, m_bottom);
				glTexCoord2f(1.0f*(float)m_tileFactor, 1.0f*(float)m_tileFactor);
				glVertex2i(m_left+(m_width-1), m_bottom);
				glTexCoord2f(1.0f*(float)m_tileFactor, 0.0f);
				glVertex2i(m_left+(m_width-1), m_bottom+(m_height-1));
			glEnd();	
			glDisable(GL_TEXTURE_2D);
			if (m_isAlpha)
			{
				glDepthMask(GL_TRUE);
				glDisable(GL_BLEND);
			}
		}
		else
		{
			glBegin(GL_QUADS);
				glNormal3f(0.0f,0.0f,1.0f*(float)m_tileFactor);
				glVertex2i(m_left, m_bottom+(m_height-1));
				glVertex2i(m_left, m_bottom);
				glVertex2i(m_left+(m_width-1), m_bottom);
				glVertex2i(m_left+(m_width-1), m_bottom+(m_height-1));
			glEnd();
		}
		drawBg=true;
	}
	if (m_selectedIndex<0)
		m_selectedIndex=m_numOfElements-1;
	else if (m_selectedIndex>=m_numOfElements)
		m_selectedIndex=0;
	glguiElementData_t *curElmt=NULL;
	bool oldRedrawAll=m_redrawAll;
	for (int i=0; i<m_numOfElements; i++)
	{
		if (m_numOfElements<2)
			curElmt=m_elementList;
		else
			curElmt=&m_elementList[i];
		char send=NULL;
		if (m_selectedIndex==i)
			send=nextCharacter;
		switch (curElmt->elementType)
		{
		case GLGUI_ELEMENT_TEXTFEILD:
			if (mouseButton)
				if (curElmt->element.textFeild.pElement->IsClicked(cursorX,cursorY))
					m_selectedIndex=i;
			if (!m_redrawAll && (m_selectedIndex!=i))
				break;
			curElmt->element.textFeild.pElement->UpdateAndRender(send,curElmt->element.textFeild.drawBackground);
			break;
		case GLGUI_ELEMENT_SLIDER:
			if (!m_redrawAll && (m_selectedIndex!=i)&& (!curElmt->element.slider.pElement->IsOver(cursorX,cursorY)))
				break;
			curElmt->element.slider.position=curElmt->element.slider.pElement->Update(cursorX,cursorY,mouseButton);
			if (curElmt->element.slider.position==SLIDERRENDERER_MAXVALUE&&mouseButton)
				int donothing=SLIDERRENDERER_MAXVALUE;
			if ((mouseButton&&curElmt->element.slider.pElement->IsOver(cursorX,cursorY))&&
				(curElmt->element.slider.selectIndex>=0))
				m_selectedIndex=curElmt->element.slider.selectIndex;
			break;
		case GLGUI_ELEMENT_IMAGE:
			if (curElmt->element.image.pElement->IsMouseOver(cursorX,cursorY)||drawBg)
				curElmt->element.image.pElement->Render();
			break;
		case GLGUI_ELEMENT_BUTTON:
			{
				bool sentMouseButton=mouseButton;
				if (m_selectedIndex!=i && m_selectedIndex>=0)
				{
					switch (m_elementList[m_selectedIndex].elementType)
					{
					case GLGUI_ELEMENT_SELECTLIST:
						if (m_elementList[m_selectedIndex].element.selectList.pElement->IsOver(cursorX,cursorY))
							sentMouseButton=false;
						break;
					case GLGUI_ELEMENT_DROPDOWNLIST:
						if (m_elementList[m_selectedIndex].element.dropDownList.pElement->IsOver(cursorX,cursorY))
							sentMouseButton=false;
						break;
					default:
						break;
					}
				}
				curElmt->element.button.status=curElmt->element.button.pElement->UpdateAndRender(cursorX,cursorY,(m_selectedIndex==i),sentMouseButton);
			}
			if (curElmt->element.button.pElement->GetCurrentState()==BUTTONRENDERER_STATE_MOUSEDOWN)
				curElmt->element.button.depressedTime+=elapsedTime;
			else
				curElmt->element.button.depressedTime=0.0f;
			if ((m_selectedIndex==i)&&(nextCharacter==' '))
			{
				curElmt->element.button.status=true;
				m_redrawAll=true;
			}
			if (curElmt->element.button.status&&curElmt->submitButton)
			{
				m_pressedButton=i;
				m_redrawAll=true;
				result=true;
			}
			if (curElmt->element.button.status&&(curElmt->element.button.selectIndex>=0))
				m_selectedIndex=curElmt->element.button.selectIndex;
 			break;
		case GLGUI_ELEMENT_TOGGLEBUTTON:
			if ((!IsMouseOver(cursorX,cursorY))&&(!m_redrawAll))
				break;
			if((curElmt->element.toggleButton.pElement->UpdateAndRender(cursorX,cursorY,curElmt->element.toggleButton.status,mouseButton))||((m_selectedIndex==i)&&(nextCharacter==' ')))
				curElmt->element.toggleButton.status=(!curElmt->element.toggleButton.status);
			if ((m_selectedIndex==i)&&(nextCharacter==' '))
				m_redrawAll=true;
			break;
		case GLGUI_ELEMENT_SELECTLIST:
			if (mouseButton||(nextCharacter==' '&&m_selectedIndex==i))
				if (curElmt->element.selectList.pElement->IsClicked(cursorX,cursorY)||nextCharacter==' ')
				{
					bool isOnTop = (i>=m_selectedIndex);
					m_selectedIndex=i;
					if (curElmt->submitButton&&isOnTop)
					{
						m_pressedButton=i;
						m_redrawAll=true;
						result=true;
					}
				}
			if (!m_redrawAll && (m_selectedIndex!=i))
				break;
			curElmt->element.selectList.pElement->UpdateAndRender(send,
				curElmt->element.selectList.drawBackground,
				curElmt->element.selectList.drawHighlight);
			if (curElmt->element.selectList.scrollDownIndex>=0)
				if (m_elementList[curElmt->element.selectList.scrollDownIndex].elementType==GLGUI_ELEMENT_BUTTON)
					if (m_elementList[curElmt->element.selectList.scrollDownIndex].element.button.depressedTime>GLGUI_SCROLLDELAY)
					{
						curElmt->element.selectList.pElement->ScrollDown();
						m_elementList[curElmt->element.selectList.scrollDownIndex].element.button.depressedTime=0.0f;
					}
			if (curElmt->element.selectList.scrollUpIndex>=0)
				if (m_elementList[curElmt->element.selectList.scrollUpIndex].elementType==GLGUI_ELEMENT_BUTTON)
					if (m_elementList[curElmt->element.selectList.scrollUpIndex].element.button.depressedTime>GLGUI_SCROLLDELAY)
					{
						curElmt->element.selectList.pElement->ScrollUp();
						m_elementList[curElmt->element.selectList.scrollUpIndex].element.button.depressedTime=0.0f;
					}
			if (curElmt->element.selectList.sliderIndex>=0)
			{
				if (mouseButton&&m_elementList[curElmt->element.selectList.sliderIndex].element.slider.pElement->IsOver(cursorX,cursorY))
					curElmt->element.selectList.pElement->ScrollTo(
					(float)(SLIDERRENDERER_MAXVALUE-m_elementList[curElmt->element.selectList.sliderIndex].element.slider.position)/
					(float)SLIDERRENDERER_MAXVALUE);
				else
				{
					m_elementList[curElmt->element.selectList.sliderIndex].element.slider.pElement->SetPosition(SLIDERRENDERER_MAXVALUE-
					(int)(curElmt->element.selectList.pElement->ScrollPosition()*(float)SLIDERRENDERER_MAXVALUE));
					m_redrawAll=true;
				}
			}
			break;
		case GLGUI_ELEMENT_DROPDOWNLIST:
			if (mouseButton||(nextCharacter==' '&&m_selectedIndex==i))
				if (curElmt->element.dropDownList.pElement->IsClicked(cursorX,cursorY)||nextCharacter==' ')
				{
					bool isOnTop = (i>=m_selectedIndex);
					m_selectedIndex=i;
					if (curElmt->submitButton&&isOnTop)
					{
						m_pressedButton=i;
						m_redrawAll=true;
						result=true;
					}
				}
			{
				static oldSelectedIndex=m_selectedIndex;
				if (m_selectedIndex!=i && m_selectedIndex!=oldSelectedIndex)
				{
					curElmt->element.dropDownList.oldState=curElmt->element.dropDownList.state;
					curElmt->element.dropDownList.state=false;
					
						curElmt->element.dropDownList.pElement->ChangeDisplayHeight(curElmt->element.dropDownList.displayHeight);
						curElmt->element.dropDownList.pElement->MoveTo(curElmt->element.dropDownList.bX,curElmt->posY);
				}
				oldSelectedIndex=m_selectedIndex;					
			}
			if (!m_redrawAll && (m_selectedIndex!=i))
				if (!curElmt->element.dropDownList.state)
					break;
			curElmt->element.dropDownList.pElement->UpdateAndRender(send,
				curElmt->element.dropDownList.drawBackground,
				curElmt->element.dropDownList.drawHighlight);
			if (curElmt->element.dropDownList.scrollDownIndex>=0)
				if (m_elementList[curElmt->element.dropDownList.scrollDownIndex].elementType==GLGUI_ELEMENT_BUTTON)
					if (m_elementList[curElmt->element.dropDownList.scrollDownIndex].element.button.depressedTime>GLGUI_SCROLLDELAY)
					{
						curElmt->element.dropDownList.pElement->ScrollDown();
						m_elementList[curElmt->element.dropDownList.scrollDownIndex].element.button.depressedTime=0.0f;
					}
			if (curElmt->element.dropDownList.scrollUpIndex>=0)
				if (m_elementList[curElmt->element.dropDownList.scrollUpIndex].elementType==GLGUI_ELEMENT_BUTTON)
					if (m_elementList[curElmt->element.dropDownList.scrollUpIndex].element.button.depressedTime>GLGUI_SCROLLDELAY)
					{
						curElmt->element.dropDownList.pElement->ScrollUp();
						m_elementList[curElmt->element.dropDownList.scrollUpIndex].element.button.depressedTime=0.0f;
					}
			if (curElmt->element.dropDownList.openIndex>=0)
				if (m_elementList[curElmt->element.dropDownList.openIndex].elementType==GLGUI_ELEMENT_BUTTON)
				{
					if (m_elementList[curElmt->element.dropDownList.openIndex].element.button.status &&
						(!curElmt->element.dropDownList.oldState))
					{
						curElmt->element.dropDownList.oldState=true;
						curElmt->element.dropDownList.state=(!curElmt->element.dropDownList.state);
						if (curElmt->element.dropDownList.state)
						{
							curElmt->element.dropDownList.pElement->ChangeDisplayHeight(curElmt->element.dropDownList.maxHeight);
							curElmt->element.dropDownList.pElement->MoveTo(curElmt->element.dropDownList.bX,curElmt->element.dropDownList.y2);
						}
						else
						{
							curElmt->element.dropDownList.pElement->ChangeDisplayHeight(curElmt->element.dropDownList.displayHeight);
							curElmt->element.dropDownList.pElement->MoveTo(curElmt->element.dropDownList.bX,curElmt->posY);
						}
					}
					else if (!(m_elementList[curElmt->element.dropDownList.openIndex].element.button.status) &&
						curElmt->element.dropDownList.oldState)
						curElmt->element.dropDownList.oldState=false;
				}
			break;
		default:
			break;
		}
	}
	if (oldRedrawAll==m_redrawAll)
		m_redrawAll=false;
	return result;
}
/***************************************************************************************************

  Returns the index of the buttons that were pressed since the last window update. -1 if none.

***************************************************************************************************/
int CGLGUIWindow::GetPressedButton()
{
	return m_pressedButton;
}
/***************************************************************************************************

  Returns the index of the named element. Returns -1 on failure.

***************************************************************************************************/
int CGLGUIWindow::FindElement(const char *elmtName)
{
	if (elmtName==NULL)
		return -1;	
	glguiElementData_t *curElmt=NULL;
	for (int i=0; i<m_numOfElements; i++)
	{
		if (m_numOfElements<2)
			curElmt=m_elementList;
		else
			curElmt=&m_elementList[i];
		if (strcmp(curElmt->elementName,elmtName)==NULL)
			return i;
	}
	return -1;
}
/***************************************************************************************************

  Returns the current integer value of element (0=False 1=True)

***************************************************************************************************/
int CGLGUIWindow::GetElementStatus(int elmtIndex)
{
	if ((elmtIndex<0)&&(elmtIndex>=m_numOfElements))
		return 0;
	glguiElementData_t *curElmt=NULL;
	if (m_numOfElements<2)
		curElmt=m_elementList;
	else
		curElmt=&m_elementList[elmtIndex];
	switch (curElmt->elementType)
	{
	case GLGUI_ELEMENT_BUTTON:
		return (curElmt->element.button.status ? 1 : 0);
		break;
	case GLGUI_ELEMENT_TOGGLEBUTTON:
		return (curElmt->element.toggleButton.status ? 1 : 0);
		break;
	case GLGUI_ELEMENT_SLIDER:
		return curElmt->element.slider.position;
		break;
	default:
		return 0;
		break;
	}
	return 0;
}
/***************************************************************************************************

  Returns the current string contents of element. (NULL if none or failure)
  NOTE: Calls Element's Renderering Routine.

***************************************************************************************************/
const char *CGLGUIWindow::GetReturnedString(int elmtIndex)
{
	if ((elmtIndex<0)&&(elmtIndex>=m_numOfElements))
		return NULL;
	glguiElementData_t *curElmt=NULL;
	if (m_numOfElements<2)
		curElmt=m_elementList;
	else
		curElmt=&m_elementList[elmtIndex];
	switch (curElmt->elementType)
	{
	case GLGUI_ELEMENT_TEXTFEILD:
		return curElmt->element.textFeild.pElement->UpdateAndRender(INPUTMAN_TEXT_CODE_ENTER,
			curElmt->element.textFeild.drawBackground);
		break;
	case GLGUI_ELEMENT_SELECTLIST:
		return curElmt->element.selectList.pElement->UpdateAndRender(INPUTMAN_TEXT_CODE_ENTER,
			curElmt->element.selectList.drawBackground,
			curElmt->element.selectList.drawHighlight);
		break;
	case GLGUI_ELEMENT_DROPDOWNLIST:
		return curElmt->element.dropDownList.pElement->UpdateAndRender(INPUTMAN_TEXT_CODE_ENTER,
			curElmt->element.dropDownList.drawBackground,
			curElmt->element.dropDownList.drawHighlight);
		break;
	default:
		return NULL;
		break;
	}
	return NULL;
}
/***************************************************************************************************

  Returns true if the specifed coordinates occur over the window.

***************************************************************************************************/
bool CGLGUIWindow::IsMouseOver(int cursorX, int cursorY)
{
	return ((cursorX>=m_left)&&(cursorX<=(m_left+(m_width-1)))&&(cursorY>=m_bottom)&&(cursorY<=(m_bottom+(m_height-1))));
}
/***********************************************************************

  Moves the window to the specified coordinates, from the left
  bottom corner.  Returns true if the window has moved.

***********************************************************************/
bool CGLGUIWindow::MoveTo(int x, int y)
{
	int dx = x-m_left;
	int dy = y-m_bottom;
	m_left+=dx;
	m_bottom+=dy;
	glguiElementData_t *curElmt=NULL;
	for (int i=0; i<m_numOfElements; i++)
	{
		if (m_numOfElements<2)
			curElmt=m_elementList;
		else
			curElmt=&m_elementList[i];
		switch (curElmt->elementType)
		{
		case GLGUI_ELEMENT_TEXTFEILD:
			if (curElmt->element.textFeild.pElement!=NULL)
			{
				curElmt->element.textFeild.pElement->MoveTo(m_left+curElmt->posX,m_bottom+curElmt->posY);
			}
			break;
		case GLGUI_ELEMENT_SLIDER:
			if (curElmt->element.slider.pElement!=NULL)
			{
				curElmt->element.slider.pElement->MoveTo(m_left+curElmt->posX,m_bottom+curElmt->posY);
			}
			break;
		case GLGUI_ELEMENT_IMAGE:
			if (curElmt->element.image.pElement!=NULL)
			{
				curElmt->element.image.pElement->MoveTo(m_left+curElmt->posX,m_bottom+curElmt->posY);
			}
			break;
		case GLGUI_ELEMENT_BUTTON:
			if (curElmt->element.button.pElement!=NULL)
			{
				curElmt->element.button.pElement->MoveTo(m_left+curElmt->posX,m_bottom+curElmt->posY);
			}
			break;
		case GLGUI_ELEMENT_TOGGLEBUTTON:
			if (curElmt->element.toggleButton.pElement!=NULL)
			{
				curElmt->element.toggleButton.pElement->MoveTo(m_left+curElmt->posX,m_bottom+curElmt->posY);
			}
			break;
		case GLGUI_ELEMENT_SELECTLIST:
			if (curElmt->element.selectList.pElement!=NULL)
			{
				curElmt->element.selectList.pElement->MoveTo(m_left+curElmt->posX,m_bottom+curElmt->posY);
			}
			break;
		case GLGUI_ELEMENT_DROPDOWNLIST:
			if (curElmt->element.dropDownList.pElement!=NULL)
			{
				curElmt->posX+=dx;
				curElmt->posY+=dy;
				curElmt->element.dropDownList.y2+=dy;
				curElmt->element.dropDownList.bX+=dx;
				if (curElmt->element.dropDownList.state)
				{
					curElmt->element.dropDownList.pElement->ChangeDisplayHeight(curElmt->element.dropDownList.maxHeight);
					curElmt->element.dropDownList.pElement->MoveTo(curElmt->element.dropDownList.bX,curElmt->element.dropDownList.y2);
				}
				else
				{
					curElmt->element.dropDownList.pElement->ChangeDisplayHeight(curElmt->element.dropDownList.displayHeight);
					curElmt->element.dropDownList.pElement->MoveTo(curElmt->element.dropDownList.bX,curElmt->posY);
				}
			}
			break;
		default:
			break;
		}
	}
	m_redrawAll=true;
	return ((dx!=0)||(dy!=0));
}
/***********************************************************************

  Gets The dimensions of the Window.

***********************************************************************/
void CGLGUIWindow::GetDimensions(int &width, int &height)
{
	width=m_width;
	height=m_height;
}
/***********************************************************************

  Gets The position of element within window.

***********************************************************************/
void CGLGUIWindow::GetElmtPosition(int elmtIndex, int &x, int &y)
{
	if ((elmtIndex<0)&&(elmtIndex>=m_numOfElements))
		return;
	glguiElementData_t *curElmt=NULL;
	if (m_numOfElements<2)
		curElmt=m_elementList;
	else
		curElmt=&m_elementList[elmtIndex];
	x=curElmt->posX;
	y=curElmt->posY;
}
/***********************************************************************

  Gets The center x y coordinates of button within window.
  Works for toggle buttons too.

***********************************************************************/
void CGLGUIWindow::GetButtonCenter(int elmtIndex, int &x, int &y)
{
	if ((elmtIndex<0)&&(elmtIndex>=m_numOfElements))
		return;
	glguiElementData_t *curElmt=NULL;
	if (m_numOfElements<2)
		curElmt=m_elementList;
	else
		curElmt=&m_elementList[elmtIndex];
	int width, height;
	switch (curElmt->elementType)
	{
	case GLGUI_ELEMENT_BUTTON:
		width = curElmt->element.button.width;
		height = curElmt->element.button.height;
		break;
	case GLGUI_ELEMENT_TOGGLEBUTTON:
		width = curElmt->element.toggleButton.width;
		height = curElmt->element.toggleButton.height;
		break;
	default:
		return;
		break;
	}
	x=curElmt->posX+width/2;
	y=curElmt->posY+height/2;
}
/***********************************************************************

  Changes the contents of an element

***********************************************************************/
void CGLGUIWindow::ChangeElmtData(int elmtIndex, const char *string)
{
	if ((elmtIndex<0)&&(elmtIndex>=m_numOfElements))
		return;
	glguiElementData_t *curElmt=NULL;
	if (m_numOfElements<2)
		curElmt=m_elementList;
	else
		curElmt=&m_elementList[elmtIndex];
	switch (curElmt->elementType)
	{
	case GLGUI_ELEMENT_SELECTLIST:
		curElmt->element.selectList.pElement->ChangeString(string);
		if (!curElmt->element.selectList.drawBackground)
			m_redrawAll=true;
		break;
	case GLGUI_ELEMENT_DROPDOWNLIST:
		curElmt->element.dropDownList.pElement->ChangeString(string);
		if (!curElmt->element.dropDownList.drawBackground)
			m_redrawAll=true;
		break;
	case GLGUI_ELEMENT_TEXTFEILD:
		curElmt->element.textFeild.pElement->SetTextString(string);
		break;
	case GLGUI_ELEMENT_IMAGE:
		curElmt->element.image.pElement->ChangeImage(string);
		m_redrawAll=true;
		break;
	default:
		break;
	}
}
/***********************************************************************

  Gets coordinates of image if element is an image.

  NOTE: Useful for defining a rendering area in a window.

***********************************************************************/
void CGLGUIWindow::GetImageCoordinates(int elmtIndex, int &left,int &top,int &right,int &bottom)
{
	if ((elmtIndex<0)&&(elmtIndex>=m_numOfElements))
		return;	
	glguiElementData_t *curElmt=NULL;
	if (m_numOfElements<2)
		curElmt=m_elementList;
	else
		curElmt=&m_elementList[elmtIndex];
	if (curElmt->elementType!=GLGUI_ELEMENT_IMAGE)
		return;
	curElmt->element.image.pElement->GetCoordinates(left,top,right,bottom);
}
/***********************************************************************

  Changes the contents of an element

***********************************************************************/
void CGLGUIWindow::ChangeElmtData(int elmtIndex,int value)
{
	if ((elmtIndex<0)&&(elmtIndex>=m_numOfElements))
		return;
	glguiElementData_t *curElmt=NULL;
	if (m_numOfElements<2)
		curElmt=m_elementList;
	else
		curElmt=&m_elementList[elmtIndex];
	switch (curElmt->elementType)
	{
	case GLGUI_ELEMENT_SLIDER:
		if (value<0)
			value=0;
		else if (value>SLIDERRENDERER_MAXVALUE)
			value=SLIDERRENDERER_MAXVALUE;
		curElmt->element.slider.position=value;
		curElmt->element.slider.pElement->SetPosition(value);
		m_redrawAll=true;
		break;
	case GLGUI_ELEMENT_IMAGE:
		if (value==NULL)
		{
			curElmt->element.image.pElement->ChangeImage(NULL);
			m_redrawAll=true;
		}
		break;
	default:
		break;
	}
}
/***********************************************************************

  Returns Index of Selected Element.

***********************************************************************/
int CGLGUIWindow::GetSelectedIndex()
{
	return m_selectedIndex;
}
/***********************************************************************

  Returns True if the mouse is over the element.

***********************************************************************/
bool CGLGUIWindow::IsMouseOverElmt(int elmtIndex,int cursorX,int cursorY)
{
	if ((elmtIndex<0)&&(elmtIndex>=m_numOfElements))
		return false;
	glguiElementData_t *curElmt=NULL;
	if (m_numOfElements<2)
		curElmt=m_elementList;
	else
		curElmt=&m_elementList[elmtIndex];
	switch (curElmt->elementType)
	{
	case GLGUI_ELEMENT_TEXTFEILD:
		return curElmt->element.textFeild.pElement->IsOver(cursorX,cursorY);
		break;
	case GLGUI_ELEMENT_BUTTON:
		{
			int r = curElmt->element.button.pElement->GetCurrentState();
			return (r==BUTTONRENDERER_STATE_MOUSEOVER||
					r==BUTTONRENDERER_STATE_MOUSEDOWN);
		}
		break;
	case GLGUI_ELEMENT_TOGGLEBUTTON:
		{
			int r = curElmt->element.toggleButton.pElement->GetCurrentState();
			return (r==BUTTONRENDERER_STATE_MOUSEOVER||
					r==BUTTONRENDERER_STATE_MOUSEDOWN);
		}
		break;
	case GLGUI_ELEMENT_SELECTLIST:
		return curElmt->element.selectList.pElement->IsOver(cursorX,cursorY);
		break;
	case GLGUI_ELEMENT_DROPDOWNLIST:
		return curElmt->element.dropDownList.pElement->IsOver(cursorX,cursorY);
		break;
	case GLGUI_ELEMENT_SLIDER:
		return curElmt->element.slider.pElement->IsOver(cursorX,cursorY);
		break;
	case GLGUI_ELEMENT_IMAGE:
		return curElmt->element.image.pElement->IsMouseOver(cursorX,cursorY);
		break;
	default:
		break;
	}
	return false;
}
/***********************************************************************

  Destroys Window.

***********************************************************************/
CGLGUIWindow::~CGLGUIWindow()
{
	try
	{
		glguiElementData_t *curElmt=NULL;
		if (m_elementList!=NULL)
		{
			for (int i=0; i<m_numOfElements; i++)
			{
				if (m_numOfElements<2)
					curElmt=m_elementList;
				else
					curElmt=&m_elementList[i];
				if (curElmt->elementName!=NULL)
					free(curElmt->elementName);
				switch (curElmt->elementType)
				{
				case GLGUI_ELEMENT_TEXTFEILD:
					if (curElmt->element.textFeild.pElement!=NULL)
						delete curElmt->element.textFeild.pElement;
					curElmt->element.textFeild.pElement=NULL;
					break;
				case GLGUI_ELEMENT_SLIDER:
					if (curElmt->element.slider.pElement!=NULL)
						delete curElmt->element.slider.pElement;
					curElmt->element.slider.pElement=NULL;
					break;
				case GLGUI_ELEMENT_IMAGE:
					if (curElmt->element.image.pElement!=NULL)
						delete curElmt->element.image.pElement;
					curElmt->element.image.pElement=NULL;
					break;
				case GLGUI_ELEMENT_BUTTON:
					if (curElmt->element.button.pElement!=NULL)
						delete curElmt->element.button.pElement;
					curElmt->element.button.pElement=NULL;
					break;
				case GLGUI_ELEMENT_TOGGLEBUTTON:
					if (curElmt->element.toggleButton.pElement!=NULL)
						delete curElmt->element.toggleButton.pElement;
					curElmt->element.toggleButton.pElement=NULL;
					break;
				case GLGUI_ELEMENT_SELECTLIST:
					if (curElmt->element.selectList.pElement!=NULL)
						delete curElmt->element.selectList.pElement;
					curElmt->element.selectList.pElement=NULL;
					break;
				case GLGUI_ELEMENT_DROPDOWNLIST:
					if (curElmt->element.dropDownList.pElement!=NULL)
						delete curElmt->element.dropDownList.pElement;
					curElmt->element.dropDownList.pElement=NULL;
					break;
				default:
					break;
				}
			}
			free(m_elementList);
			m_elementList=NULL;
		}
	}
	catch (...)
	{
		m_cLog->lprintf("CGLGUIWindow error: failed to destroy window.");
		throw;
	}
}
/***********************************************************************

  Cursor Renderer Function.

***********************************************************************/
void GLGUIRenderCursor(int cursorWidth,int cursorHeight, int cursorX, int cursorY,void *pxBuffer, bool isAlpha)
{
	static oldX=cursorX, oldY=cursorY;
	static bool firstTime=true;
	if (pxBuffer!=NULL&&(!firstTime))
	{
		glRasterPos2i(oldX,oldY-cursorHeight);
		glDrawPixels(cursorWidth,cursorHeight,GL_RGB,GL_UNSIGNED_BYTE,pxBuffer);
	}
	if (pxBuffer!=NULL)
	{
		glReadPixels(cursorX,cursorY-cursorHeight,cursorWidth-1,cursorHeight-1,GL_RGB,GL_UNSIGNED_BYTE,pxBuffer);		
		firstTime=false;
	}
	oldX=cursorX;
	oldY=cursorY;

	if (isAlpha)
	{
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	glBegin(GL_QUADS);
		if (isAlpha)
			glColor4f(1.0f,1.0f,1.0f,1.0f);
		else
			glColor3f(1.0f,1.0f,1.0f);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2i(cursorX, cursorY);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2i(cursorX, cursorY-(cursorHeight-1));
		glTexCoord2f(1.0f, 1.0f);
		glVertex2i(cursorX+cursorWidth-1, cursorY-(cursorHeight-1));
		glTexCoord2f(1.0f, 0.0f);
		glVertex2i(cursorX+cursorWidth-1, cursorY);
	glEnd();
	if (isAlpha)
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}
}
/***********************************************************************

  Generator Functions

***********************************************************************/
void GLGUICreateTextFeild(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int displayWidth, int maxChars,int padding,char *initialText,
						  bool drawBackground,bool setColor,float bgColorR,float bgColorG,float bgColorB,
						  float textColorR,float textColorG,float textColorB)
{
	if (windowDescriptor==NULL)
		return;
	windowDescriptor->elementName	=elementName;
	windowDescriptor->elementType	=GLGUI_ELEMENT_TEXTFEILD;
	windowDescriptor->font			=fontListBase;
	windowDescriptor->posX			=Xposition;
	windowDescriptor->posY			=Yposition;
	windowDescriptor->submitButton	=false;
	windowDescriptor->textHeight	=textHeight;
	windowDescriptor->textWidth	=textWidth;
	windowDescriptor->element.textFeild.backgroundColor[0]=bgColorR;
	windowDescriptor->element.textFeild.backgroundColor[1]=bgColorG;
	windowDescriptor->element.textFeild.backgroundColor[2]=bgColorB;
	windowDescriptor->element.textFeild.displayWidth=displayWidth;
	windowDescriptor->element.textFeild.drawBackground=drawBackground;
	windowDescriptor->element.textFeild.initialText=initialText;
	windowDescriptor->element.textFeild.maxChars=maxChars;
	windowDescriptor->element.textFeild.padding=padding;
	windowDescriptor->element.textFeild.pElement=NULL;
	windowDescriptor->element.textFeild.setColor=setColor;
	windowDescriptor->element.textFeild.textColor[0]=textColorR;
	windowDescriptor->element.textFeild.textColor[1]=textColorG;
	windowDescriptor->element.textFeild.textColor[2]=textColorB;
}
void GLGUICreateSelectionList(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int displayWidth, int displayHeight,int scrollUpIndex,int scrollDownIndex,
						  int sliderIndex, int padding, bool  drawBackground, bool drawHighlight, bool setColor,
						  float bgColorR,float bgColorG,float bgColorB,
						  float textColorR,float textColorG,float textColorB,
						  float highlightBgColorR,float highlightBgColorG,float highlightBgColorB,
						  float highlightTextColorR,float highlightTextColorG,float highlightTextColorB,
						  char *listText, bool submitButton)
{
	if (windowDescriptor==NULL)
		return;
	windowDescriptor->elementName	=elementName;
	windowDescriptor->elementType	=GLGUI_ELEMENT_SELECTLIST;
	windowDescriptor->font			=fontListBase;
	windowDescriptor->posX			=Xposition;
	windowDescriptor->posY			=Yposition;
	windowDescriptor->submitButton	=submitButton;
	windowDescriptor->textHeight	=textHeight;
	windowDescriptor->textWidth	=textWidth;
	windowDescriptor->element.selectList.backgroundColor[0]=bgColorR;
	windowDescriptor->element.selectList.backgroundColor[1]=bgColorG;
	windowDescriptor->element.selectList.backgroundColor[2]=bgColorB;
	windowDescriptor->element.selectList.displayHeight=displayHeight;
	windowDescriptor->element.selectList.displayWidth=displayWidth;
	windowDescriptor->element.selectList.drawBackground=drawBackground;
	windowDescriptor->element.selectList.drawHighlight=drawHighlight;
	windowDescriptor->element.selectList.highlightBackgroundColor[0]=highlightBgColorR;
	windowDescriptor->element.selectList.highlightBackgroundColor[1]=highlightBgColorG;
	windowDescriptor->element.selectList.highlightBackgroundColor[2]=highlightBgColorB;
	windowDescriptor->element.selectList.highlightTextColor[0]=highlightTextColorR;
	windowDescriptor->element.selectList.highlightTextColor[1]=highlightTextColorG;
	windowDescriptor->element.selectList.highlightTextColor[2]=highlightTextColorB;
	windowDescriptor->element.selectList.listText=listText;
	windowDescriptor->element.selectList.padding=padding;
	windowDescriptor->element.selectList.pElement=NULL;
	windowDescriptor->element.selectList.scrollDownIndex=scrollDownIndex;
	windowDescriptor->element.selectList.scrollUpIndex=scrollUpIndex;
	windowDescriptor->element.selectList.sliderIndex=sliderIndex;
	windowDescriptor->element.selectList.setColor=setColor;
	windowDescriptor->element.selectList.textColor[0]=textColorR;
	windowDescriptor->element.selectList.textColor[1]=textColorG;
	windowDescriptor->element.selectList.textColor[2]=textColorB;
}
void GLGUICreateButton(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition, bool submitButton,///
						  int width, int height, char *buttonText, char *defaultTexName,
						  char *mouseOverTexName, char *mouseDownTexName, bool setColor, float textColorR,
						  float textColorG, float textColorB)
{
	if (windowDescriptor==NULL)
		return;
	windowDescriptor->elementName	=elementName;
	windowDescriptor->elementType	=GLGUI_ELEMENT_BUTTON;
	windowDescriptor->font			=fontListBase;
	windowDescriptor->posX			=Xposition;
	windowDescriptor->posY			=Yposition;
	windowDescriptor->submitButton	=false;
	windowDescriptor->textHeight	=textHeight;
	windowDescriptor->textWidth	=textWidth;
	windowDescriptor->submitButton	=submitButton;
	windowDescriptor->element.button.buttonText=buttonText;
	windowDescriptor->element.button.defaultTexName=defaultTexName;
	windowDescriptor->element.button.depressedTime=0.0f;
	windowDescriptor->element.button.height=height;
	windowDescriptor->element.button.mouseDownTexName=mouseDownTexName;
	windowDescriptor->element.button.mouseOverTexName=mouseOverTexName;
	windowDescriptor->element.button.pElement=NULL;
	windowDescriptor->element.button.setColor=setColor;
	windowDescriptor->element.button.status=false;
	windowDescriptor->element.button.textColor[0]=textColorR;
	windowDescriptor->element.button.textColor[1]=textColorG;
	windowDescriptor->element.button.textColor[2]=textColorB;
	windowDescriptor->element.button.width=width;
}

void GLGUICreateDropDownList(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int displayWidth, int defaultHeight, int fullHeight, int openIndex,int scrollUpIndex,int scrollDownIndex,
						  int padding, bool  drawBackground, bool drawHighlight, bool setColor,
						  float bgColorR,float bgColorG,float bgColorB,
						  float textColorR,float textColorG,float textColorB,
						  float highlightBgColorR,float highlightBgColorG,float highlightBgColorB,
						  float highlightTextColorR,float highlightTextColorG,float highlightTextColorB,
						  char *listText, bool submitButton)
{
	if (windowDescriptor==NULL)
		return;
	windowDescriptor->elementName	=elementName;
	windowDescriptor->elementType	=GLGUI_ELEMENT_DROPDOWNLIST;
	windowDescriptor->font			=fontListBase;
	windowDescriptor->posX			=Xposition;
	windowDescriptor->posY			=Yposition;
	windowDescriptor->submitButton	=submitButton;
	windowDescriptor->textHeight	=textHeight;
	windowDescriptor->textWidth	=textWidth;
	windowDescriptor->element.dropDownList.backgroundColor[0]=bgColorR;
	windowDescriptor->element.dropDownList.backgroundColor[1]=bgColorG;
	windowDescriptor->element.dropDownList.backgroundColor[2]=bgColorB;
	windowDescriptor->element.dropDownList.bX=0;
	windowDescriptor->element.dropDownList.displayHeight=defaultHeight;
	windowDescriptor->element.dropDownList.displayWidth=displayWidth;
	windowDescriptor->element.dropDownList.drawBackground=drawBackground;
	windowDescriptor->element.dropDownList.drawHighlight=drawHighlight;
	windowDescriptor->element.dropDownList.highlightBackgroundColor[0]=highlightBgColorR;
	windowDescriptor->element.dropDownList.highlightBackgroundColor[1]=highlightBgColorG;
	windowDescriptor->element.dropDownList.highlightBackgroundColor[2]=highlightBgColorB;
	windowDescriptor->element.dropDownList.highlightTextColor[0]=highlightTextColorR;
	windowDescriptor->element.dropDownList.highlightTextColor[1]=highlightTextColorG;
	windowDescriptor->element.dropDownList.highlightTextColor[2]=highlightTextColorB;
	windowDescriptor->element.dropDownList.listText=listText;
	windowDescriptor->element.dropDownList.maxHeight=fullHeight;
	windowDescriptor->element.dropDownList.oldState=false;
	windowDescriptor->element.dropDownList.openIndex=openIndex;
	windowDescriptor->element.dropDownList.padding=padding;
	windowDescriptor->element.dropDownList.pElement=NULL;
	windowDescriptor->element.dropDownList.scrollDownIndex=scrollDownIndex;
	windowDescriptor->element.dropDownList.scrollUpIndex=scrollUpIndex;
	windowDescriptor->element.dropDownList.setColor=setColor;
	windowDescriptor->element.dropDownList.state=false;
	windowDescriptor->element.dropDownList.textColor[0]=textColorR;
	windowDescriptor->element.dropDownList.textColor[1]=textColorG;	
	windowDescriptor->element.dropDownList.textColor[2]=textColorB;
	windowDescriptor->element.dropDownList.y2=0;
}
void GLGUICreateToggleButton(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int width, int height, char *buttonText, char *OffTexName,
						  char *OnTexName, bool setColor, float textColorR,float textColorG, float textColorB,
						  bool defaultCheckStatus)
{
	if (windowDescriptor==NULL)
		return;
	windowDescriptor->elementName	=elementName;
	windowDescriptor->elementType	=GLGUI_ELEMENT_TOGGLEBUTTON;
	windowDescriptor->font			=fontListBase;
	windowDescriptor->posX			=Xposition;
	windowDescriptor->posY			=Yposition;
	windowDescriptor->submitButton	=false;
	windowDescriptor->textHeight	=textHeight;
	windowDescriptor->textWidth	=textWidth;
	windowDescriptor->submitButton	=false;
	windowDescriptor->element.toggleButton.buttonText=buttonText;
	windowDescriptor->element.toggleButton.defaultTexName=OffTexName;
	windowDescriptor->element.toggleButton.OnTexName=OnTexName;
	windowDescriptor->element.toggleButton.height=height;
	windowDescriptor->element.toggleButton.pElement=NULL;
	windowDescriptor->element.toggleButton.setColor=setColor;
	windowDescriptor->element.toggleButton.status=defaultCheckStatus;
	windowDescriptor->element.toggleButton.textColor[0]=textColorR;
	windowDescriptor->element.toggleButton.textColor[1]=textColorG;
	windowDescriptor->element.toggleButton.textColor[2]=textColorB;
	windowDescriptor->element.toggleButton.width=width;
}
void GLGUICreateTextDisplay(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  bool setColor,float textColorR,float textColorG,float textColorB,
						  char *text)
{
	if (windowDescriptor==NULL || text==NULL)
		return;
	int displayHeight=1, displayWidth=0, curLineWidth=0, textLength=strlen(text);
	for (int i=0; i<textLength; i++)
	{
		if (text[i]!='\n')
			curLineWidth++;
		else
		{
			if (curLineWidth>displayWidth)
				displayWidth=curLineWidth;
			curLineWidth=0;
			displayHeight++;
		}
	}
	if (curLineWidth>displayWidth)
		displayWidth=curLineWidth;
	windowDescriptor->elementName	=elementName;
	windowDescriptor->elementType	=GLGUI_ELEMENT_SELECTLIST;
	windowDescriptor->font			=fontListBase;
	windowDescriptor->posX			=Xposition;
	windowDescriptor->posY			=Yposition;
	windowDescriptor->submitButton	=false;
	windowDescriptor->textHeight	=textHeight;
	windowDescriptor->textWidth		=textWidth;
	windowDescriptor->element.selectList.backgroundColor[0]=0.0f;
	windowDescriptor->element.selectList.backgroundColor[1]=0.0f;
	windowDescriptor->element.selectList.backgroundColor[2]=0.0f;
	windowDescriptor->element.selectList.displayHeight=displayHeight;
	windowDescriptor->element.selectList.displayWidth=displayWidth;
	windowDescriptor->element.selectList.drawBackground=false;
	windowDescriptor->element.selectList.drawHighlight=false;
	windowDescriptor->element.selectList.highlightBackgroundColor[0]=0.0f;
	windowDescriptor->element.selectList.highlightBackgroundColor[1]=0.0f;
	windowDescriptor->element.selectList.highlightBackgroundColor[2]=0.0f;
	windowDescriptor->element.selectList.highlightTextColor[0]=textColorR;
	windowDescriptor->element.selectList.highlightTextColor[1]=textColorG;
	windowDescriptor->element.selectList.highlightTextColor[2]=textColorB;
	windowDescriptor->element.selectList.listText=text;
	windowDescriptor->element.selectList.padding=0;
	windowDescriptor->element.selectList.pElement=NULL;
	windowDescriptor->element.selectList.scrollDownIndex=-1;
	windowDescriptor->element.selectList.scrollUpIndex=-1;
	windowDescriptor->element.selectList.setColor=setColor;
	windowDescriptor->element.selectList.textColor[0]=textColorR;
	windowDescriptor->element.selectList.textColor[1]=textColorG;
	windowDescriptor->element.selectList.textColor[2]=textColorB;
}
void GLGUICreateTextDisplay(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int padding, bool  drawBackground, bool setColor,
						  float bgColorR,float bgColorG,float bgColorB,
						  float textColorR,float textColorG,float textColorB,
						  char *text)
{
	if (windowDescriptor==NULL || text==NULL)
		return;
	int displayHeight=1, displayWidth=0, curLineWidth=0, textLength=strlen(text);
	for (int i=0; i<textLength; i++)
	{
		if (text[i]!='\n')
			curLineWidth++;
		else
		{
			if (curLineWidth>displayWidth)
				displayWidth=curLineWidth;
			curLineWidth=0;
			displayHeight++;
		}
	}
	if (curLineWidth>displayWidth)
		displayWidth=curLineWidth;
	windowDescriptor->elementName	=elementName;
	windowDescriptor->elementType	=GLGUI_ELEMENT_SELECTLIST;
	windowDescriptor->font			=fontListBase;
	windowDescriptor->posX			=Xposition;
	windowDescriptor->posY			=Yposition;
	windowDescriptor->submitButton	=false;
	windowDescriptor->textHeight	=textHeight;
	windowDescriptor->textWidth		=textWidth;
	windowDescriptor->element.selectList.backgroundColor[0]=bgColorR;
	windowDescriptor->element.selectList.backgroundColor[1]=bgColorG;
	windowDescriptor->element.selectList.backgroundColor[2]=bgColorB;
	windowDescriptor->element.selectList.displayHeight=displayHeight;
	windowDescriptor->element.selectList.displayWidth=displayWidth;
	windowDescriptor->element.selectList.drawBackground=drawBackground;
	windowDescriptor->element.selectList.drawHighlight=false;
	windowDescriptor->element.selectList.highlightBackgroundColor[0]=bgColorR;
	windowDescriptor->element.selectList.highlightBackgroundColor[1]=bgColorG;
	windowDescriptor->element.selectList.highlightBackgroundColor[2]=bgColorB;
	windowDescriptor->element.selectList.highlightTextColor[0]=textColorR;
	windowDescriptor->element.selectList.highlightTextColor[1]=textColorG;
	windowDescriptor->element.selectList.highlightTextColor[2]=textColorB;
	windowDescriptor->element.selectList.listText=text;
	windowDescriptor->element.selectList.padding=padding;
	windowDescriptor->element.selectList.pElement=NULL;
	windowDescriptor->element.selectList.scrollDownIndex=-1;
	windowDescriptor->element.selectList.scrollUpIndex=-1;
	windowDescriptor->element.selectList.setColor=setColor;
	windowDescriptor->element.selectList.textColor[0]=textColorR;
	windowDescriptor->element.selectList.textColor[1]=textColorG;
	windowDescriptor->element.selectList.textColor[2]=textColorB;
}
void GLGUICreateSlider(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int width,int height, int sliderPosition,bool isHorizontal,
						  const char *backTexName, const char *buttonTexName)
{
	if (windowDescriptor==NULL)
		return;
	windowDescriptor->elementName	=elementName;
	windowDescriptor->elementType	=GLGUI_ELEMENT_SLIDER;
	windowDescriptor->font			=fontListBase;
	windowDescriptor->posX			=Xposition;
	windowDescriptor->posY			=Yposition;
	windowDescriptor->submitButton	=false;
	windowDescriptor->textHeight	=textHeight;
	windowDescriptor->textWidth		=textWidth;
	windowDescriptor->element.slider.backTexName=backTexName;
	windowDescriptor->element.slider.buttonTexName=buttonTexName;
	windowDescriptor->element.slider.height=height;
	windowDescriptor->element.slider.isHorizontal=isHorizontal;
	windowDescriptor->element.slider.pElement=NULL;
	windowDescriptor->element.slider.position=0;
	windowDescriptor->element.slider.selectIndex=-1;
	windowDescriptor->element.slider.width=width;
	windowDescriptor->element.slider.position=sliderPosition;
}
void GLGUICreateImage(glguiElementData_t *windowDescriptor,char *elementName,int fontListBase,
						  int textWidth, int textHeight, int Xposition, int Yposition,///
						  int width,int height, const char *texName)
{
	if (windowDescriptor==NULL)
		return;
	windowDescriptor->elementName	=elementName;
	windowDescriptor->elementType	=GLGUI_ELEMENT_IMAGE;
	windowDescriptor->font			=fontListBase;
	windowDescriptor->posX			=Xposition;
	windowDescriptor->posY			=Yposition;
	windowDescriptor->submitButton	=false;
	windowDescriptor->textHeight	=textHeight;
	windowDescriptor->textWidth		=textWidth;
	windowDescriptor->element.image.height=height;
	windowDescriptor->element.image.width=width;
	windowDescriptor->element.image.texName=(char *)texName;
	windowDescriptor->element.image.pElement=NULL;
}
void GLGUICoordinateCalculator(int &left, int &top, int &right, int &bottom,glguiElementData_t *windowDescriptor)
{
	switch (windowDescriptor->elementType)
	{
	case GLGUI_ELEMENT_TEXTFEILD:
		TextFeildRendererDimensionCalculator(left,top,right,bottom,
			windowDescriptor->posX,windowDescriptor->posY,
			windowDescriptor->textWidth,windowDescriptor->textHeight,
			windowDescriptor->element.textFeild.displayWidth,
			windowDescriptor->element.textFeild.padding);
		break;
	case GLGUI_ELEMENT_SLIDER:
		SliderRendererCoordinateCalculator(left,top,right,bottom,
			windowDescriptor->posX,windowDescriptor->posY,
			windowDescriptor->element.slider.width,
			windowDescriptor->element.slider.height);
		break;
	case GLGUI_ELEMENT_BUTTON:
		ButtonRendererDimensionCalculator(left,top,right,bottom,
			windowDescriptor->posX,windowDescriptor->posY,
			windowDescriptor->element.button.width,
			windowDescriptor->element.button.height);
		break;
	case GLGUI_ELEMENT_TOGGLEBUTTON:
		ButtonRendererDimensionCalculator(left,top,right,bottom,
			windowDescriptor->posX,windowDescriptor->posY,
			windowDescriptor->element.toggleButton.width,
			windowDescriptor->element.toggleButton.height);
		break;
	case GLGUI_ELEMENT_SELECTLIST:
		SelectionListRendererDimensionCalculator(left,top,right,bottom,
			windowDescriptor->posX,windowDescriptor->posY,
			windowDescriptor->textWidth,windowDescriptor->textHeight,
			windowDescriptor->element.selectList.displayWidth,
			windowDescriptor->element.selectList.displayHeight,
			windowDescriptor->element.selectList.padding);
		break;
	case GLGUI_ELEMENT_DROPDOWNLIST:
		SelectionListRendererDimensionCalculator(left,top,right,bottom,
			windowDescriptor->posX,windowDescriptor->posY,
			windowDescriptor->textWidth,windowDescriptor->textHeight,
			windowDescriptor->element.dropDownList.displayWidth,
			windowDescriptor->element.dropDownList.displayHeight,
			windowDescriptor->element.dropDownList.padding);
		break;
	case GLGUI_ELEMENT_IMAGE:
		GUIImageRendererCoordinateCalculater(left,top,right,bottom,
			windowDescriptor->posX,windowDescriptor->posY,
			windowDescriptor->element.image.width,
			windowDescriptor->element.image.height);
		break;
	default:
		break;
	}
}

int GLGUITextDispWidthEstCalculator(char *string, int maxTextWidth)
{
	if (string==NULL)
		return 0;
	register int length = strlen(string);
	register int widthTally=0;
	for (register int i=0; i<length; i++)
	{
		switch (string[i])
		{
		case ' ':case '|':case '.':
			widthTally+=(maxTextWidth*230/1000);
			break;
		case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':
			case 'j':case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':
				case 's':case 't':case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':
					widthTally+=maxTextWidth/2;//(maxTextWidth*692/1000);
					break;
		case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':
			case 'J':case 'K':case 'L':case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':
				case 'S':case 'T':case 'U':case 'V':case 'X':case 'Y':case 'Z':
					widthTally+=((maxTextWidth*846)/1000);
					break;
				case 'W':
					widthTally+=maxTextWidth;
					break;
				default:
					widthTally+=(maxTextWidth*692/1000);
					break;
		}
	}
	return widthTally;
}
