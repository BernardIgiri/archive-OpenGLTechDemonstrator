#include "FaceClass.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>

void CFace::Draw()
{
	static int		nVertexCountDown;
	static VERTEX*	pCurVertex;

	pCurVertex = m_pVertices;
	nVertexCountDown = m_nVertexCount;
	glBegin(m_glCommand);
	while (nVertexCountDown--)
		glVertex3fv((float *)pCurVertex++);
	glEnd();
}