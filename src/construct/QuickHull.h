///////////////////////////////////////////////////////////////
//
// QuickHull.h
//
//   Find Convex Hull of a Set of 3D Points using Quick Hull Algorithm
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 08/Nov/2017
//
//
///////////////////////////////////////////////////////////////

#ifndef _QUICK_HULL_H
#define _QUICK_HULL_H

#include "vec.h"
#include <Vector>

//#define DEBUG_QUICK_HULL


using namespace std;

struct Triangle;
class HEdgeMesh;

struct TriFace
{
	Triangle *tri;                      // Triangluar face
	vector<Vector3f> lightPts;          // All points that can see the face
};


class QuickHull
{
public:
	vector<Vector3f> pointList;          // Input point list
	double transMat[16];                 // Transform matrix of the points (for debug)

	vector<Vector3f> verList;            // Vertices of the convex hull
	vector<Vector3i> triList;            // Triangles of the convex hull (vertex indices)
	vector<Triangle*> triangles;         // Triangles of the convex hull (vertex positions)

public:
	HEdgeMesh *hedgeMesh;                // Half-edge structure for the mesh of faceList
	vector<TriFace> faceList;            // Faces for constructing the convex hull

#ifdef DEBUG_QUICK_HULL
	int iter;
#endif

public:
	QuickHull();
	~QuickHull();
	void Init(vector<Vector3f> _pointList, double _transMat[16]);
	void Clear();

	// Compute Convex Hull using Quick Hull
	void ComputeConvexHull();
	vector<Vector3f> CreateTetrahedron();
	void CorrectTriangleNormal();

	// Check If Terminate Expanding
	bool IsTerminateExpand();
	void CreateLightFaces(vector<Vector3f> vertices);

	// Expand Convex Hull
	void ExpandConvexHull(bool isAdd);
	bool FindNewVertex(int &triID, Vector3f &vertex);
	vector<int> GetLightFaces(int triID, Vector3f vertex);
	void AddNewTriangles(Vector3f vertex, vector<int> lightFaceIDs);
	void CorrectTriangleNormal(Vector3i &tri, Triangle *triangle, const vector<Vector3i> triList);

	// Utility Functions 
	bool IsPointInList(Vector3f tagtPt, vector<Vector3f> points);
	bool IsPointSeeTriangle(Vector3f point, Triangle *tri);
};


#endif