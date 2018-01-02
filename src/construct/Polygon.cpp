///////////////////////////////////////////////////////////////
//
// Polygon.cpp
//
//   3D Polygon (vertices may or may not be co-planar)
//
// by Peng SONG  ( songpenghit@gmail.com )
// 
// 10/Nov/2017
//
//
///////////////////////////////////////////////////////////////


#include "HelpStruct.h"
#include "HelpDefine.h"
#include "HelpFunc.h"
#include "Polygon.h"
//#include <GL/glut.h>



//**************************************************************************************//
//                                    Initialization
//**************************************************************************************//

Polygon::Polygon()
{

}

Polygon::~Polygon()
{

}

Polygon & Polygon::operator=(const Polygon &poly)
{
	if( this == &poly )
		return *this;

	this->vers   = poly.vers;
	this->normal = poly.normal;
	this->center = poly.center;
	this->area   = poly.area;

	return *this;
}

//bool Polygon::IsEqual(Polygon *poly)
//{
//	if (this->verIDs.size() != poly->verIDs.size())
//		return false;
//
//	for (int i = 0; i < this->verIDs.size(); i++)
//	{
//		if (GetElementIndexInList(poly->verIDs, this->verIDs[i]) == ELEMENT_OUT_LIST)
//		{
//			return false;
//		}
//	}
//
//	return true;
//}

void Polygon::Print()
{
	printf("verNum %d: [ ", verIDs.size());

	for (int i = 0; i < verIDs.size(); i++)
	{
		printf(" %d ", verIDs[i]);
	}
	printf("]\n");
}




//**************************************************************************************//
//                                    Compute Properties
//**************************************************************************************//

void Polygon::ComputeCenter()
{
	center = Vector3f(0,0,0);
	for (int i=0; i<vers.size(); i++)
	{
		center += vers[i];
	}

	center = center / float(vers.size());
}


void Polygon::ComputeNormal()
{
	//int verNum = vers.size();
	//int index1;
	//int index2;

	//if ( verNum < 6 )
	//{ 
	//	index1 = 1;                 
	//	index2 = 2;                 
	//}
	//else             
	//{ 
	//	index1 = verNum * (1/3.0)-1;  // Avoid selecting vertices that are too close to each other
	//	index2 = verNum * (2/3.0)-1;
	//}
	//
	//Vector3f tempNor = (vers[index1] - vers[0]) CROSS (vers[index2] - vers[0]);  // Assume the vertices are saved in counter-clockwise	
	//float tempNorLen = len(tempNor);

	//if ( tempNorLen != 0 )    normal = tempNor / tempNorLen;
	//else                      normal = Vector3f(1,0,0);     // Note: this default vector also can be others

	ComputeCenter();

	Vector3f tempNor = (vers[0] - center) CROSS (vers[1] - center);

	normal = tempNor / len(tempNor);
}

void Polygon::ComputeArea()
{
	if ( normal == Vector3f(0,0,0) )
		ComputeNormal();

	float signedArea = 0;
	for (int i=0; i<vers.size(); i++)
	{
		Vector3f crossVec;

		if ( i < vers.size()-1 )    crossVec = vers[i] CROSS vers[i+1];
		else                        crossVec = vers[i] CROSS vers[0];

		signedArea += 0.5f * (normal DOT crossVec);
	}

	area = fabs( signedArea );
}

void Polygon::ComputeMidPoints()
{
	midPts.clear();

	for (int i = 0; i < vers.size(); i++)
	{
		Vector3f staPt = vers[i];
		Vector3f endPt = vers[(i + 1) % vers.size()];

		Vector3f midPt = 0.5f * (staPt + endPt);

		midPts.push_back(midPt);
	}
}

void Polygon::ReverseVertices()
{
	// Reverse vertex IDs
	vector<int> newVerIDs;
	for (int i = verIDs.size() - 1; i >= 0; i--)
	{
		newVerIDs.push_back(verIDs[i]);
	}

	verIDs = newVerIDs;

	// Reverse vertices
	vector<Vector3f> newVers;
	for (int i=vers.size()-1; i>=0; i--)
	{
		newVers.push_back( vers[i] );
	}

	vers = newVers;

	// Reverse normal
	normal = -1.0f * normal;
}




//**************************************************************************************//
//                                    Create Polygon
//**************************************************************************************//

void Polygon::CreatePolygon_Regular(int edgeNum, float radius)
{
	for (int i = 0; i < edgeNum; i++)
	{
		float angle = (i / edgeNum) * M_PI / 180.0;
		float x = radius * sin(angle);
		float z = radius * cos(angle);

		//printf("v %6.3 %6.3 %6.3 \n", x, 0, z);
	}
}

void Polygon::CreatePolygon_Fan(Vector3f verA, Vector3f verB, Vector3f verM)
{
	Vector3f edgeDirA = verA / len(verA);
	Vector3f edgeDirB = verB / len(verB);
	Vector3f edgeDirM = verM / len(verM);

	Vector3f rotAxis = edgeDirA CROSS edgeDirM;

	// Compute fan angle
	float dotp = edgeDirA DOT edgeDirB;
	float angle;
	if (fabs(dotp + 1) < FLOAT_ERROR_LARGE)
	{
		angle = 180;

	}
	else
	{
		angle = acos(dotp) * 180 / M_PI;
	}

	// Compute fan vertices
	vers.clear();
	vers.push_back(Vector3f(0, 0, 0));
	vers.push_back(verA);

	const float angleStep = 10; // Unit: degree
	float sampleAngle = 0;
	do
	{
		Vector3f ver = RotateVector(Vector3f(0, 0, 0), rotAxis, sampleAngle, verA);

		vers.push_back(ver);

		sampleAngle += angleStep;
	} while (sampleAngle < angle);

	vers.push_back(verB);

	// Construct the polygon
	verIDs.clear();
	for (int i = 0; i < vers.size(); i++)
	{
		verIDs.push_back(i);
	}

	ComputeCenter();
	ComputeNormal();
}




//**************************************************************************************//
//                                      Rendering
//**************************************************************************************//

//void Polygon::Draw(float width, Vector3f color)
//{
//	glDisable(GL_LIGHTING);
//	glPointSize(12.0);
//	glLineWidth(width);
//
//	glColor3f(color.x, color.y, color.z);
//	glBegin(GL_LINE_LOOP);
//	for (int i = 0; i < vers.size(); i++)
//	{
//		glVertex3f(vers[i].x, vers[i].y, vers[i].z);
//	}
//	glEnd();
//
//	glPointSize(1.0);
//	glLineWidth(1.0);
//	glEnable(GL_LIGHTING);
//}

