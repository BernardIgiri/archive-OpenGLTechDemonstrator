#ifndef __INCLUDED_CFACE__
#define __INCLUDED_CFACE__

#define face_set(face,glCommand,nVertexCount,pVertices) (face)->m_glCommand=glCommand;(face)->m_nVertexCount=nVertexCount;(face)->m_pVertices=pVertices

#ifndef __INCLUDED_VERTEXSTRUCT__
#define __INCLUDED_VERTEXSTRUCT__
struct VERTEX
{
	float x,y,z;
};
#endif

class CFace
{
public:
	CFace() : m_pVertices(0), m_nVertexCount(0), m_glCommand(0) {};
	~CFace() {};

	virtual void Draw();

	//		Member Data
	VERTEX*	m_pVertices;
	int		m_nVertexCount;
	int		m_glCommand;
};

#endif