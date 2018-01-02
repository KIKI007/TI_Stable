///////////////////////////////////////////////////////////////
//
// Part.cpp
//
//   Part Model Class
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 16/Dec/2017
//
///////////////////////////////////////////////////////////////

#include "Controls.h"
#include "HelpDefine.h"
#include "HelpStruct.h"
#include "Cross.h"
#include "Part.h"
#include "PartGeom.h"
#include "PolyMesh.h"
//#include <GL/glut.h>

//extern Vector3f colorTable[18];


//**************************************************************************************//
//                                   Initialization
//**************************************************************************************//

Part::Part(vector<OrientPoint> _oriPointList)
{
	oriPointList = _oriPointList;

	for (int i = 0; i < oriPointList.size(); i++)
	{
		oriPointList[i].normal = oriPointList[i].normal / len(oriPointList[i].normal);
	}

	touchGround = false;

	atBoundary = false;

	isRemove = false;

	polyMesh = NULL;

	partGeom = NULL;

	assoCrossID = NONE_CROSS;
}

Part::~Part()
{
	Clear();
}

void Part::Clear()
{
	if ( polyMesh != NULL )
	{
		delete polyMesh;
		polyMesh = NULL;
	}

	if (partGeom != NULL)
	{
		delete partGeom;
		partGeom = NULL;
	}

	initNeighbors.clear();
}

void Part::PrintPart()
{
	printf("partID: %2d   ", partID);

	printf("%d neighbors: ", initNeighbors.size());
	for (int i = 0; i < initNeighbors.size(); i++)
	{
		if (initNeighbors[i] == NULL)
		{
			printf(" %3d ", -1);
		}
		else
		{
			printf(" %3d ", initNeighbors[i]->partID);
		}
	}

	printf("\n");
}




//**************************************************************************************//
//                                Compute Part Geometry 
//**************************************************************************************//

void Part::ComputePartGeometry()
{
	if ( partGeom != NULL )
	{
		delete partGeom;
		partGeom = NULL;
	}

	if (polyMesh != NULL)
	{
		delete polyMesh;
		polyMesh = NULL;
	}

	partGeom = new PartGeom(oriPointList);

	polyMesh = partGeom->ComputePartGeometry();

	polyMesh->ComputeCentroid();
	polyMesh->ComputeBBox();

	//printf("ver num: %d \n", polyMesh->verList.size());

	//for (int i = 0; i < polyMesh->verList.size(); i++)
	//{
	//	Vector3f ver = polyMesh->verList[i];

	//	printf("i=%d  v: [%.3f %.3f %.3f] \n", i, ver.x, ver.y, ver.z);
	//}
}


void Part::WriteOBJModel(char *objFileName)
{
	if (polyMesh == NULL)
		return;

	polyMesh->WriteOBJModel(objFileName);
}




//**************************************************************************************//
//                                      Part Operations
//**************************************************************************************//

int Part::GetNeighborIndex(Part *neiborPart)
{
	for (int i = 0; i < initNeighbors.size(); i++)
	{
		if ( initNeighbors[i] == NULL )
			continue;

		if ( initNeighbors[i]->partID == neiborPart->partID)
		{
			return i;
		}
	}

	return NONE_PART;
}




//**************************************************************************************//
//                                         Draw Part 
//**************************************************************************************//
//
//void Part::DrawPart()
//{
//	if (polyMesh == NULL)
//		return;
//
//	Vector3f color;
//	//if (touchGround)    color = Vector3f(0.2, 0.2, 0.2);
//	if (atBoundary)     color = Vector3f(0.2, 0.2, 0.2);
//	else                color = Vector3f(0.9, 0.9, 0.9);
//
//	float mtlDiffuse[4] = { color.x, color.y, color.z, 1.0 };
//	float mtlSpecular[4] = { color.x, color.y, color.z, 1.0 };
//
//	glPushAttrib(GL_LIGHTING_BIT);
//
//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mtlDiffuse);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mtlSpecular);
//
//	polyMesh->DrawMesh();
//
//	glPopAttrib();
//}
//
//void Part::DrawPartWire(float lineWidth, Vector3f color)
//{
//	if (polyMesh == NULL)
//		return;
//
//	polyMesh->DrawMeshWire(lineWidth, color);
//}



//
////**************************************************************************************//
////                                  Draw Part Construction
////**************************************************************************************//
//
//void Part::DrawInnerPolygon()
//{
//	if (partGeom == NULL)
//		return;
//
//	partGeom->DrawInnerPolygon();
//}
//
//void Part::DrawOriPoints()
//{
//	if (partGeom == NULL)
//		return;
//
//	partGeom->DrawOriPoints();
//}
//
//void Part::DrawGeomFaces()
//{
//	if (partGeom == NULL)
//		return;
//
//	partGeom->DrawFaces();
//}
//
//void Part::DrawGeomEdges()
//{
//	if (partGeom == NULL)
//		return;
//
//	partGeom->DrawEdges();
//}
//
//void Part::DrawGeomVertices()
//{
//	if (partGeom == NULL)
//		return;
//
//	partGeom->DrawVertices();
//}