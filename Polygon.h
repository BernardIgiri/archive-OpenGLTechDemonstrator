#ifndef __INCLUDED_CPOLYGON__
#define __INCLUDED_CPOLYGON__

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

class CPolygon
{
public:
	CPolygon() : m_pVertices(NULL), m_pFaces(NULL), m_nFaces(0), m_nVertices(0) {};
	~CPolygon() {delete [] m_pVertices, delete m_pFaces;}
	void Draw();

	VERTEX*	m_pVertices;
	int		m_nVertices;
	FACE*	m_pFaces;
	int		m_nFaces;
};

void CPolygon::Draw()
{
	FACE *ptrFace = m_pFaces[0];
	INDEX* ptrIndex = NULL;
	int nFaceCount = m_nFaces, nIndexCount = 0;
	while(nFaceCount--)
	{
		ptrIndex = ptrFace++->pIndices;
		nIndexCount = ptrFace->nCount;
		glBegin(GL_POLYGON);
		while(nIndexCount--)
			glVertex3fv(&m_pVertices[ptrIndex++->nVertex]);
		glEnd();
	}
}

#endif

/*

  CTriangle
  CTriangleStrip
  CTriangleFan
  CQuad
  CQuadStrip
  CPolygon

GL_TRIANGLES 
Treats each triplet of vertices as an independent triangle. Vertices 3n – 2, 3n –1, and 3n define triangle n. N/3 triangles are drawn. 
GL_TRIANGLE_STRIP 
Draws a connected group of triangles. One triangle is defined for each vertex presented after the first two vertices. For odd n, vertices n, n + 1, and n + 2 define triangle n. For even n, vertices n + 1, n, and n + 2 define triangle n. N – 2 triangles are drawn. 
GL_TRIANGLE_FAN 
Draws a connected group of triangles. One triangle is defined for each vertex presented after the first two vertices. Vertices 1, n + 1, and n + 2 define triangle n. N – 2 triangles are drawn. 
GL_QUADS 
Treats each group of four vertices as an independent quadrilateral. Vertices 4n – 3, 4n – 2, 4n – 1, and 4n define quadrilateral n. N/4 quadrilaterals are drawn. 
GL_QUAD_STRIP 
Draws a connected group of quadrilaterals. One quadrilateral is defined for each pair of vertices presented after the first pair. Vertices 2n – 1, 2n, 2n + 2, and 2n + 1 define quadrilateral n. N quadrilaterals are drawn. Note that the order in which vertices are used to construct a quadrilateral from strip data is different from that used with independent data. 
GL_POLYGON */