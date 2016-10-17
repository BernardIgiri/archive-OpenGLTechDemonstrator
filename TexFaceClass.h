#ifndef __INCLUDED_CTEXFACE__
#define __INCLUDED_CTEXFACE__

#include <memory.h>

#ifndef __INCLUDED_VERTEXSTRUCT__
#define __INCLUDED_VERTEXSTRUCT__
struct VERTEX
{
	float x,y,z;
};
#endif

#ifndef __INCLUDED_TEXCOORDSTRUCT__
#define __INCLUDED_TEXCOORDSTRUCT__
struct TEXCOORD
{
	float s,t;
};
#endif

/*struct VERTEXDATA
{
	VERTEX	 vertex;
	TEXCOORD texCoord;
};*/

#define texface_set(texFace,glCommand,vertexCount,pVertices,pTexCoords) (texFace)->m_glCommand=glCommand;(texFace)->m_pVertices=pVertices;(texFace)->m_pTexCoords=pTexCoords;(texFace)->m_nVertexCount=vertexCount
class CTexFace
{
public:
	CTexFace() : m_pVertices(0),m_pTexCoords(0), m_nVertexCount(0), m_glCommand(0) {};
	~CTexFace() {};

/*	virtual void inline SetData(int glCommand,int nVertexCount,VERTEXDATA *pVertexData);
	virtual void inline SetData(int glCommand,int nVertexCount,VERTEXDATA* pVertexDataSpace,VERTEX *pVertices,TEXCOORD *pTexCoords);*/
	virtual void Draw();

	//			Member Data
	VERTEX*		m_pVertices;
	TEXCOORD*	m_pTexCoords;
	int			m_nVertexCount;
	int			m_glCommand;
};

/*void CTexFace::SetData(int glCommand,int nVertexCount,VERTEXDATA *pVertexData)
{
	m_glCommand=glCommand;
	m_nVertexCount=nVertexCount;
	m_pVertexData=pVertexData;
}
/**********************************************************************************************************************

  Does no memory allocation/management
  pVertexDataSpace must point to a preallocated memory space large enough for the vertex data
  ie: sizeof(VERTEXDATA)*nVertexCount

  NOTE: Structures are in perfect memory alignment with with coorisponding OpenGL float arrays.

*********************************************************************************************************************
void CTexFace::SetData(int glCommand,int nVertexCount,VERTEXDATA* pVertexDataSpace,VERTEX *pVertices,TEXCOORD *pTexCoords)
{
	m_glCommand=glCommand;
	m_nVertexCount=nVertexCount;
	m_pVertexData=pVertexDataSpace;
	static int			nVertexCountDown;
	static VERTEXDATA*	pCurVertex;
	pCurVertex = m_pVertexData;
	nVertexCountDown = m_nVertexCount;
	while (nVertexCountDown--)
	{
		memcpy(&pCurVertex->texCoord,pTexCoords,sizeof(TEXCOORD));
		memcpy(&pCurVertex->vertex,pVertices,sizeof(VERTEX));
		pTexCoords++;
		pVertices++;
		pCurVertex++;
	}
}
*/
#endif