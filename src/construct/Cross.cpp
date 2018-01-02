///////////////////////////////////////////////////////////////
//
// Cross.cpp
//
//   Cross section
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 15/Nov/2017
//
///////////////////////////////////////////////////////////////


#include "Controls.h"
#include "HelpDefine.h"
#include "HelpFunc.h"
#include "Cross.h"
#include "math3D.h"


//**************************************************************************************//
//                                   Initialization
//**************************************************************************************//

Cross::Cross()
{
	type = CROSS_QUAD;
	L = CROSS_L;

	dist = 0;

	isVisited = false;

	assoPartID = NONE_PART;
}

Cross::Cross(int _type, float _L)
{
	type = _type;
	L = _L;

	if      (type == CROSS_HEX)      CreateCross_Hexagon(L);
	else if (type == CROSS_QUAD)     CreateCross_Square(L);

	//poly2D.ComputeCenter();
	poly2D.ComputeNormal();
	poly2D.ComputeMidPoints();

	assoPartID = NONE_PART;
}

Cross::~Cross()
{

}




//**************************************************************************************//
//                                   Initialize Cross
//**************************************************************************************//

void Cross::CreateCross_Hexagon(float L)
{
	float A = 0.5f * L;
	float B = (sqrt(3) / 2.0f) * L;

	// Polygon
	poly2D.vers.clear();
	poly2D.vers.push_back(Vector3f(-L, 0, 0));
	poly2D.vers.push_back(Vector3f(-A, B, 0));
	poly2D.vers.push_back(Vector3f(A, B, 0));
	poly2D.vers.push_back(Vector3f(L, 0, 0));
	poly2D.vers.push_back(Vector3f(A, -B, 0));
	poly2D.vers.push_back(Vector3f(-A, -B, 0));

	// Tilt angles
	tiltAngles.clear();
	tiltAngles.push_back(-CROSS_THETA);
	tiltAngles.push_back( CROSS_THETA);
	tiltAngles.push_back(-CROSS_THETA);
	tiltAngles.push_back( CROSS_THETA);
	tiltAngles.push_back(-CROSS_THETA);
	tiltAngles.push_back( CROSS_THETA);
}

void Cross::CreateCross_Square(float L)
{
	float A = 0.5f * L;

	// Polygon
	poly2D.vers.clear();
	poly2D.vers.push_back(Vector3f(-A, -A, 0));
	poly2D.vers.push_back(Vector3f(-A, A, 0));
	poly2D.vers.push_back(Vector3f(A, A, 0));
	poly2D.vers.push_back(Vector3f(A, -A, 0));

	// Tilt angles
	tiltAngles.clear();
	tiltAngles.push_back(-CROSS_THETA);
	tiltAngles.push_back( CROSS_THETA);
	tiltAngles.push_back(-CROSS_THETA);
	tiltAngles.push_back( CROSS_THETA);
}




//**************************************************************************************//
//                                  Compute Neighbors
//**************************************************************************************//

void Cross::ComputeNeighbors()
{
	if ( neighbors.size() != 0 )
	{
		printf("Warning: The cross's neighbors are not empty. \n");
	}

	for (int i = 0; i <poly2D.vers.size(); i++)
	{
		Vector3f staPt = poly2D.vers[i];
		Vector3f endPt = poly2D.vers[(i + 1) % poly2D.vers.size()];
		float theta = tiltAngles[i];

		Cross *neighbor = ComputeNeighbor(type, staPt, endPt, theta);

		neighbors.push_back(neighbor);
	}
}

Cross* Cross::ComputeNeighbor(int tagtType, Vector3f tagtStaPt, Vector3f tagtEndPt, float tagtTheta)
{
	Vector3f tagtVec = (tagtEndPt - tagtStaPt) / len(tagtEndPt-tagtStaPt);
	Vector3f tagtMidPt = (tagtEndPt + tagtStaPt) / 2.0f;

	Cross *neighbor = new Cross(tagtType, CROSS_L);

	// Identify the edge shared by tagtCross and neighborCross
	// TODO: consider cross rotation here
	Vector3f transVec = Vector3f(0,0,0);
	bool reverseTheta;
	for (int i = 0; i < neighbor->poly2D.vers.size(); i++)
	{
		Vector3f staPt = neighbor->poly2D.vers[i];
		Vector3f endPt = neighbor->poly2D.vers[(i + 1) % neighbor->poly2D.vers.size()];

		Vector3f edgeVec = (endPt - staPt) / len(endPt - staPt);
		Vector3f edgeMidPt = (endPt + staPt) / 2.0f;

		float dotp = tagtVec DOT edgeVec;
		if ( fabs(dotp+1) < FLOAT_ERROR_LARGE )
		{
			transVec = tagtMidPt - edgeMidPt;

			float neighborTheta = neighbor->tiltAngles[i];

			if (tagtTheta *neighborTheta > 0)    reverseTheta = true;
			else                                 reverseTheta = false;

			break;
		}
	}

	// Translate the neighborCross
	neighbor->Translate(transVec);

	// Make sure the theta of tagtCross and neighborCross have different signs (but the same absolute value)
	if (reverseTheta)
	{
		for (int i = 0; i < neighbor->tiltAngles.size(); i++)
		{
			neighbor->tiltAngles[i] = -1.0f * neighbor->tiltAngles[i];
		}
	}

	return neighbor;
}

void Cross::Translate(Vector3f transVec)
{
	for (int i = 0; i < poly2D.vers.size(); i++)
	{
		poly2D.vers[i] += transVec;
	}

	poly2D.center += transVec;
}

bool Cross::IsEqual(Cross *cross)
{
	if (this->poly2D.vers.size() != cross->poly2D.vers.size())
		return false;

	float dist = len(this->poly2D.center - cross->poly2D.center);

	if (dist > FLOAT_ERROR_LARGE)
		return false;

	return true;
}




//**************************************************************************************//
//                                 Compute Tilt Normals
//**************************************************************************************//

void Cross::ComputeTiltNormals()
{
	oriPointList.clear();

	for (int i = 0; i < poly3D.vers.size(); i++)
	{
		// Compute surfNormal (should be (0,0,1) for planar surfaces)
		Vector3f surfNormal;
		if (neighbors.size() != 0 && neighbors[i] != NULL && neighbors[i]->isValid == true)
		{
			surfNormal = 0.5f * (poly3D.normal + neighbors[i]->poly3D.normal);
		}
		else
		{ 
			//printf("something wrong. \n");
			surfNormal = poly3D.normal;
		}

		// Get edge direction vector
		Vector3f staPt = poly3D.vers[i];
		Vector3f endPt = poly3D.vers[(i + 1) % poly3D.vers.size()];
		Vector3f edgeDir = (endPt - staPt) / len(endPt - staPt);

		// Compute tilt normal
		Vector3f tiltNormal = edgeDir CROSS surfNormal;
		//Vector3f tiltNormal = surfNormal;

		//printf("curr: [%.3f %.3f %.3f]   neibor: [%.3f %.3f %.3f] \n", poly3D.normal.x, poly3D.normal.y, poly3D.normal.z, neighbors[i]->poly3D.normal.x, neighbors[i]->poly3D.normal.y, neighbors[i]->poly3D.normal.z);
		//printf("i=%d surf: [%.3f %.3f %.3f]   tiltNormal: [%.3f %.3f %.3f] \n", i, surfNormal.x, surfNormal.y, surfNormal.z, tiltNormal.x, tiltNormal.y, tiltNormal.z);
		
		tiltNormal = RotateNormal(tiltNormal, edgeDir, tiltAngles[i]);


		OrientPoint oriPoint;
		oriPoint.point  = 0.5f * (staPt + endPt);
		oriPoint.normal = tiltNormal;

		oriPointList.push_back(oriPoint);
	}
}

Vector3f Cross::RotateNormal(Vector3f normal, Vector3f rotAxis, float rotAngle)
{
	double rotMat[16];
	GetRotationMatrix(rotAngle, rotAxis, rotMat);

	Vector3f outNormal;
	MultiplyNormal(normal, rotMat, outNormal);

	return outNormal;
}




//**************************************************************************************//
//                                       Rendering
//**************************************************************************************//
//
//void Cross::DrawCross3D()
//{
//	if (isValid == false || poly3D.vers.size() == 0 )
//		return;
//
//	// Draw the polygone and edge middle points
//	poly3D.Draw(2.0, Vector3f(0.2, 0.9, 0.9));
//
//	// Draw polygon center for debug
//	//glColor3f(0.2, 0.9, 0.2);
//	//glPointSize(id*3+3);
//	//glBegin(GL_POINTS);
//	//glVertex3f(poly3D.center.x, poly3D.center.y, poly3D.center.z);
//	//glEnd();
//	//glPointSize(1.0);
//}

//void Cross::DrawConnection()
//{
//	if (isValid == false || poly3D.vers.size() == 0)
//		return;
//
//	glDisable(GL_LIGHTING);
//	glLineWidth(2.0);
//	glPointSize(10.0);
//	glColor3f(0.9, 0.6, 0.1);
//
//	glBegin(GL_POINTS);
//	glVertex3f(poly3D.center.x, poly3D.center.y, poly3D.center.z);
//	glEnd();
//
//	for (int i = 0; i < neighbors.size(); i++)
//	{
//		if ( neighbors[i]->isValid == false )
//			continue;
//
//		Vector3f currPt = poly3D.center;
//		Vector3f neiborPt = neighbors[i]->poly3D.center;
//
//		glBegin(GL_LINES);
//		glVertex3f(currPt.x, currPt.y, currPt.z);
//		glVertex3f(neiborPt.x, neiborPt.y, neiborPt.z);
//		glEnd();
//	}
//
//	glEnable(GL_LIGHTING);
//	glLineWidth(1.0);
//}
