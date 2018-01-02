///////////////////////////////////////////////////////////////
//
// HelpStruct.h
//
//   Common Structures
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 10/Nov/2017
//
//
///////////////////////////////////////////////////////////////

#ifndef _HELP_STRUCT_H
#define _HELP_STRUCT_H

#include "vec.h"
#include <vector>

using namespace std;

// Check if a point is on a plane
#define SMALL_ERROR                  0.000001     

// Point position w.r.t a plane
#define POINT_PLANE_UNKWONN          -1
#define POINT_PLANE_INTERSECT         0
#define POINT_PLANE_POSITIVE_SIDE     1
#define POINT_PLANE_NEGATIVE_SIDE     2

// Line position w.r.t a plane
#define LINE_PLANE_UNKWONN           -1
#define LINE_PLANE_INTERSECT          0
#define LINE_PLANE_POSITIVE_SIDE      1
#define LINE_PLANE_NEGATIVE_SIDE      2

// Face position w.r.t a plane
#define FACE_PLANE_UNKWONN           -1
#define FACE_PLANE_INTERSECT          0
#define FACE_PLANE_POSITIVE_SIDE      1
#define FACE_PLANE_NEGATIVE_SIDE      2



////////////////////////////////////////////
// 3D Point
////////////////////////////////////////////

// 3D Point (with normal) 
struct Point
{
	Vector3f pos;     // Point position
	Vector3f nor;     // Point normal

	Vector3f color;   // Point color (if available)
	float curv;       // Point curvature

	int dist;         // Distance to the object surface

	Point & operator=(const Point &pt)
	{
		if( this == &pt )
			return *this;

		this->pos   = pt.pos;
		this->nor   = pt.nor;

		this->color = pt.color;
		this->curv  = pt.curv;

		return *this;
	};
};




////////////////////////////////////////////
// 3D Line
////////////////////////////////////////////

struct Line
{
	Vector3f point1;
	Vector3f point2;
};


////////////////////////////////////////////
// 3D Plane
////////////////////////////////////////////

struct Plane
{
	Vector3f point;
	Vector3f normal;

	float radius;             // For define a limited plane in 3D

	Plane & operator=(const Plane &plane);

	float PointPlaneDistance(Vector3f tagtPt);

	int PointPlaneIntersect(Vector3f tagtPt, bool isPrint);
	int LinePlaneIntersect(Line line);
	int LineIntersectPoint(Line line, Vector3f &crossPt);

//	void DrawFace(float radius, Vector3f color);
};




////////////////////////////////////////////
// 3D Box
////////////////////////////////////////////

struct Box 
{
	Vector3f minPt;
	Vector3f maxPt;

	Vector3f cenPt;
	Vector3f size;

	Box();
	Box & operator=(const Box &box);
	void PrintBox();

	void GetCenter();
	void GetSize();

	void Transform(Vector3f transVec, Vector3f scale);
	float GetQuadArea();
};




////////////////////////////////////////////
// 3D Triangle
////////////////////////////////////////////

struct Triangle
{
	Vector3f v[3];
	Vector3f normal;
	float area;            // Face area
	Vector3f center;

	int vIndices[3];       // Index of each vertex


	void Init(Vector3f _v0, Vector3f _v1, Vector3f _v2);
	Triangle & operator=(const Triangle &tri);
	bool IsEqual(Triangle *tri);
	void PrintTriangle();

	Vector3f GetBBoxMinPt();
	Vector3f GetBBoxMaxPt();

	void ComputeCenter();
	void ComputeArea();
	void ComputeNormal();
	void CorrectNormal(Vector3f tagtNormal);

//	void Draw();
//	void DrawWire(float width, Vector3f color);
};




////////////////////////////////////////////
// For computing part geometry and mobility
////////////////////////////////////////////

struct OrientPoint
{
	Vector3f point;
	Vector3f normal;
};

struct HypVertex
{
public:
	int verID;
	Vector3f point;

	//int planeIDs[3];
	int edgeID;         // For part mobility

	bool isValid;
};


struct HypEdge
{
public:
	int edgeID;
	Vector3f point;
	Vector3f normal;

	int planeIDs[2];
};

struct HypPlane
{
public:
	int planeID;
	Vector3f point;
	Vector3f normal;

	float radius;             // For rendering a finite plane in 3D
};


#endif