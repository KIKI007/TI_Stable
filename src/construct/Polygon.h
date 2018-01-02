///////////////////////////////////////////////////////////////
//
// Polygon.h
//
//   3D Polygon (vertices may or may not be co-planar)
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 20/Dec/2017
//
//
///////////////////////////////////////////////////////////////

#ifndef _POLYGON_H
#define _POLYGON_H

#include "vec.h"
#include <vector>

using namespace std;

class Polygon
{
public:
	vector<int> verIDs;                  // Vertex IDs
	vector<Vector3f> vers;               // Vertices

	Vector3f normal;                     // Normal vector
	Vector3f center;                     // Center point
	float area;                          // Area size

	vector<Vector3f> midPts;             // Edge middle points (same ordering as vers)

public:
	Polygon();
	~Polygon();
	Polygon & operator=(const Polygon &poly);
	//bool IsEqual(Polygon *poly); // TODO: test this function later
	void Print();

	// Compute Properties
	void ComputeCenter();
	void ComputeArea();
	void ComputeNormal();
	void ComputeMidPoints();
	void ReverseVertices();

	// Create Polygon
	void CreatePolygon_Regular(int edgeNum, float radius);
	void CreatePolygon_Fan(Vector3f verA, Vector3f verB, Vector3f verM);

//	// Rendering
//	void Draw(float width, Vector3f color);
};

#endif