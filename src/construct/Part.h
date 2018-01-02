///////////////////////////////////////////////////////////////
//
// Part.h
//
//   Part Model Class
//
// by Peng SONG  ( songpenghit@gmail.com )
// 
// 16/Dec/2017
//
///////////////////////////////////////////////////////////////

#include "vec.h"
#include <vector>

using namespace std;

struct OrientPoint;
class PartGeom;
class PolyMesh;


class Part 
{
public:
	int partID;                             // Part ID in the structure
	vector<OrientPoint> oriPointList;       // Input oriented points

	bool touchGround;                       // If part touches the ground 
	bool atBoundary;                        // If part at boundary of the structure

	bool isRemove;                          // If part is removed from the structure (i.e., disassembly)           

	int assoCrossID;                        // Associated cross ID
	vector<Part*>  initNeighbors;           // Neighboring parts (saved in the same order as oriPointList)
	vector<Part*>  currNeighbors;           // Neighboring parts (saved in the same order as oriPointList)

	PolyMesh *polyMesh;                     // Resulted polyhedron
	PartGeom *partGeom;                     // Generate part geometry (for debug)


public:
	Part(vector<OrientPoint> _oriPointList);
	~Part();
	void Clear();
	void PrintPart();

	// Compute Part Geometry
	void ComputePartGeometry();
	void WriteOBJModel(char *objFileName);

	// Part Operations
	int GetNeighborIndex(Part *neiborPart);

	// Draw Part
	void DrawPart();
	void DrawPartWire(float lineWidth, Vector3f color);

	// Draw Part Construction
	void DrawOriPoints();
	void DrawInnerPolygon();
	void DrawGeomFaces();
	void DrawGeomEdges();
	void DrawGeomVertices();
};