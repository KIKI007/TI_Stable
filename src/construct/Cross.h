///////////////////////////////////////////////////////////////
//
// Cross.h
//
//   Cross section
//
// by Peng SONG  ( songpenghit@gmail.com )
// 
// 15/Nov/2017
//
///////////////////////////////////////////////////////////////

#include "vec.h"
#include <vector>
#include "HelpStruct.h"
#include "Polygon.h"


#define CROSS_THETA       35.264389     // theta = asin(sqrt(3.0) / 3.0) * (180 / M_PI)
#define CROSS_L           0.4

using namespace std;

class Cross
{
public:
	int crossID;                        // Cross ID in the list
	int assoPartID;                     // Associated part ID

	int type;                           // Type of cross (i.e., regular polygon)
	float L;                            // Edge length of the regular polygon
	
	Polygon poly2D;                     // Polygon on 2D pattern (parameterization space)
	vector<Cross*> neighbors;           // Neighbors of the cross
	int dist;                           // Discrete distance to the root

	bool isValid;                       // If 3D polygon is valid or not
	Polygon poly3D;                     // Polygon on 3D surface (Euclidean space)
	vector<float> tiltAngles;           // Specified tilt angles from surface normal

	int id;                             // ID in the list
	bool isVisited;                     // For specifying tilt angles

	vector<OrientPoint> oriPointList;   // A set of oriented points


public:
	Cross();
	Cross(int _type, float _L);
	~Cross();

	// Initialize Cross
	void CreateCross_Hexagon(float L);
	void CreateCross_Square(float L);

	// Compute Neighbors
	void ComputeNeighbors();
	Cross* ComputeNeighbor(int type, Vector3f tagtStaPt, Vector3f tagtEndPt, float tagtTheta);
	void Translate(Vector3f transVec);
	bool IsEqual(Cross *cross);

	// Compute Tilt Normals
	void ComputeTiltNormals();
	Vector3f RotateNormal(Vector3f normal, Vector3f rotAxis, float rotAngle);

	// Rendering
	void DrawCross3D();

	void DrawConnection();
};

