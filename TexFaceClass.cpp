#include "TexFaceClass.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>

void CTexFace::Draw()
{
	static int			nVertexCountDown;
	static VERTEX*		pCurVertex;
	static TEXCOORD*	pCurTexCoord;
	pCurVertex = m_pVertices;
	pCurTexCoord = m_pTexCoords;
	nVertexCountDown = m_nVertexCount;
	glBegin(m_glCommand);
	while (nVertexCountDown--)
	{
		glTexCoord2fv((float *)pCurTexCoord);
		glVertex3fv((float *)pCurVertex);
		pCurTexCoord++;
		pCurVertex++;
	}
	glEnd();
}
