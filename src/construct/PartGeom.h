///////////////////////////////////////////////////////////////
//
// PartGeom.h
//
//   Construct Part Geometry
//
// by Peng SONG  ( songpenghit@gmail.com )
// 
// 16/Dec/2017
//
///////////////////////////////////////////////////////////////

#include "vec.h"
#include <vector>
#include "HelpStruct.h"

using namespace std;


struct OrientPoint;
class PolyMesh;


class PartGeom 
{
public:
	vector<OrientPoint> oriPointList;       // Input oriented points
	                      
	vector<HypPlane*> faceList;             // Constructed faces
	vector<HypEdge*> edgeList;              // Constructed edges
	vector<HypVertex*> verList;             // Constructed vertices


public:
	PartGeom(vector<OrientPoint> _oriPointList);
	~PartGeom();
	void Clear();

	// Compute Part Geometry
	PolyMesh* ComputePartGeometry();
	void ComputePartGeometry_Matrix();

	// Compute Faces and Edges
	void ComputeFaces();
	void ComputeEdges(vector<HypPlane*> _planeList);
	HypEdge* PlanePlaneIntersect(HypPlane *faceA, HypPlane *faceB);
	bool EdgeHitFace(Vector3f edgePt, Vector3f edgeDir, HypPlane *plane, Vector3f &hitPt);

	// Compute Vertices
	void ComputeVertices(vector<HypPlane*> _faceList, vector<HypEdge*> _edgeList);
	bool IsPointInList(Vector3f tagtPoint, vector<HypVertex*> _verList);

	// Validate Vertices
	vector<Vector3f> ValidateVertices();
	bool IsValidVertex(Vector3f point);

//	// Draw Base Polygon
//	void DrawInnerPolygon();
//	void DrawOriPoints();
//
//	// Draw Part Construction
//	void DrawFaces();
//	void DrawFace(HypPlane *plane);
//	void DrawEdges();
//	void DrawEdge(HypEdge *edge, float length);
//	void DrawVertices();
//	void DrawVertex(HypVertex *vertex);
};