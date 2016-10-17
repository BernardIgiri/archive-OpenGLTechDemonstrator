#ifndef __INCLUDED_CTRIANGLE__
#define __INCLUDED_CTRIANGLE__

struct VERTEX
{
	float x,y,z;
};

struct INDEX
{
	int nVertex;
};

struct FACE
{
	INDEX* pIndices;
	int nCount;
};

class CTriangle
{
public:
	CTriangle() : m_pVertices(NULL), m_pFaces(NULL), m_nFaces(0), m_nVertices(0) {};
	~CTriangle() {delete [] m_pVertices, delete m_pFaces;}
	void Draw();

	VERTEX*	m_pVertices;
	int		m_nVertices;
	FACE*	m_pFaces;
	int		m_nFaces;
};

void C




#endif