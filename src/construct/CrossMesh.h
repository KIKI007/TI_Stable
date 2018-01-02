///////////////////////////////////////////////////////////////
//
// Cross.h
//
//   Cross section
//
// by Peng SONG  ( songpenghit@gmail.com )
// 
// 17/Nov/2017
//
///////////////////////////////////////////////////////////////

#include "vec.h"
#include <vector>

using namespace std;

class Cross;


class CrossMesh
{
public:
	vector<Cross*> crossList;                // Cross list of the mesh


public:
	CrossMesh();
	~CrossMesh();
	void ResetCrossMesh();

	// Construct 2D Cross Mesh
	void CreateCrossMesh2D(int crossType, int maxDist);
	int GetCrossIndexInList(Cross *tagtCross, vector<Cross*> crossList);

	// Render 2D Pattern Offline
//	GLubyte* RenderOffline2D(float viewW, float viewH, GLint iWidth, GLint iHeight);
//	bool GetPixelLocation(Vector3f crossPt, float viewW, float viewH, GLint iWidth, GLint iHeight, Vector3i &pixelPos);
//	vector<Vector3i> Rasterize2DLine(Vector3i staPixelPos, Vector3i endPixelPos);

	// Rendering
//	void DrawCrossMesh2D();
//	void DrawCrossMesh3D();
//	void DrawCrossMesh3D_Dual();
};

