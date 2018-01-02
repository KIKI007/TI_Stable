///////////////////////////////////////////////////////////////
//
// HEdgeMesh.h
//
//   Half-Edge Mesh Structure Class
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 08/Nov/2017
//
///////////////////////////////////////////////////////////////

#ifndef _HALF_EDGE_MESH_H
#define _HALF_EDGE_MESH_H

#include "vec.h"
#include <vector>

using namespace std;

struct Triangle;
class Vertex;
class Edge;
class Face;

class Vertex
{
public:
	int id;                      // Vertex ID
	Vector3f point;              // Vertex position 
	Vector3f normal;             // Vertex normal 

	vector<Edge*> outEdges;      // All the outgoing edges from vertex
};


class Edge
{
public:
	int id;                      // Half edge ID
	Vertex *staVer;              // Vertex at starting point
	Vertex *endVer;              // Vertex at ending point
  
	Edge *next;                  // Next half edge
	Edge *prev;                  // Previous half edge
	Edge *twin;                  // Opposite half edge
	Face *face;                  // Incident face

public:
	Edge()
	{
		staVer = NULL;
		endVer = NULL;

		next = NULL;
		twin = NULL;
		prev = NULL;
		face = NULL;
	}

	bool IsEqual(Edge *edge)
	{
		if ( this->id         == edge->id &&
			 this->staVer->id == edge->staVer->id &&
			 this->endVer->id == edge->endVer->id )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};


class Face
{
public:
	int id;                      // Face ID 
	Vector3f normal;             // Face normal

	vector<Vertex*> vertices;    // Face vertices
	Edge *startEdge;             // Starting edge of face
};


class HEdgeMesh
{
public:
	vector<Vector3f> vertices;   // Model vertices
	vector<Vector3i> triangles;  // Model triangles (vertex indices)

	vector<int> sortedTriIDs;    // For render transparent model
	double prevMVMatrix[16];     // For re-render model after manipulating it

public:
	vector<Vertex*>  vertexList; // Vertex list for half edge mesh 
	vector<Edge*>    edgeList;   // Edge list for half edge mesh
	vector<Face*>    faceList;   // Face list for half edge mesh


public:
	HEdgeMesh();
	~HEdgeMesh();
	void InitMesh(vector<Vector3f> _vertices, vector<Vector3i> _triangles);

	// Build Half-Edge Structure
	void BuildMesh();
	void GenerateVertices();
	void GenerateFaces();
	void GenerateEdges();
	void UpdateVertices();
	void CreateLightFaces();
	void UpdateEdges(); 

	// Utility Functions
	int GetNextEdgeID(int edgeID);
	int GetPrevEdgeID(int edgeID);
	bool IsTwinEdges(Edge *edgeA, Edge *edgeB);
	void ComputeFaceNormal(Face *face);
	void ComputeVertexNormal(Vertex *vertex);

	// Get Neighbors
	vector<Vertex*> GetNeighborVertices(Vertex *vertex);
	vector<Face*> GetNeighborFaces(Vertex *vertex);
	vector<int> GetNeighborFaces(int faceID);
	vector<Edge*> GetNonSharedEdges(vector<int> faceIDs);
	
	// Draw Mesh
	void DrawMesh(float mtlAmbient[], float mtlDiffuse[], float mtlSpecular[], float mtlEmission[]);
	void DrawMesh(float mtlAmbient[], float mtlDiffuse[], float mtlSpecular[], float mtlEmission[], bool updateZOrder);
	void SortZBufferTriangle();
	void DrawMeshWire(Vector3f color, float width);
	void DrawMeshVertices(Vector3f color, float size);
	void DrawMeshEdges(Vector3f color, float width);

	// Draw Debug
	void DrawNeighborEdges(int edgeID);
	void DrawEdge(Edge *edge, Vector3f color);
};

#endif
