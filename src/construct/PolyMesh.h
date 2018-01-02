///////////////////////////////////////////////////////////////
//
// PolyMesh.h
//
//   Convert a Triangular Mesh into a Polygonal Mesh
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 20/Dec/2017
//
//
///////////////////////////////////////////////////////////////

#ifndef _POLY_MESH_H
#define _POLY_MESH_H

#include "vec.h"
#include <vector>
#include "HelpStruct.h"

using namespace std;

class Polygon;

class PolyMesh
{
public:
	vector<Vector3f> verList;           // Vertices of polygonal mesh
	vector<Polygon*> polyList;          // Faces of polygonal mesh

	Vector3f centroid;                  // Centroid of polygonal mesh
	Box bbox;                           // Bounding box of polygonal mesh


public:
	PolyMesh();
	~PolyMesh();

	// Mesh Operations
	void ComputeCentroid();
	void ComputeBBox();
	void ComputePolygons();

	// Convert Triangular Mesh to Polygonal Mesh
	void Convert2PolyMesh(vector<Vector3f> inVerList, vector<Vector3i> inTriList);
	void InitPolyMesh(vector<Vector3f> inVerList, vector<Vector3i> inTriList);
	bool IsCoplanar(Polygon *triA, Polygon *triB);
	Polygon* MergePolygons(Polygon *polyA, Polygon *polyB);
	void UpdatePolyList(Polygon *polyA, int polyAID, Polygon *polyB, int polyBID, vector<Polygon*> &polyList);

	// Create Mesh
	void CreateMesh_Fan(Vector3f edgeA, Vector3f edgeB, Vector3f edgeMid);

	// Save OBJ Model
	void WriteOBJModel(char *objFileName);

	// Rendering
	void DrawMesh();
	void DrawMeshWire(float width, Vector3f color);
	void DrawFaceNormals(Vector3f color);
};


#endif