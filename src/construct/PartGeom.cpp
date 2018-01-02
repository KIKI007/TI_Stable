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
//
// Input:  a set of oriented points (oriPointList)
// Output: geometry of a convex polyhedron (polyMesh)
//
///////////////////////////////////////////////////////////////

#include "Controls.h"
#include "HelpDefine.h"
#include "HelpFunc.h"
#include "math3D.h"
#include "Cross.h"
#include "PartGeom.h"
#include "QuickHull.h"
#include "HEdgeMesh.h"
#include "PolyMesh.h"


extern Vector3f colorTable[18];


//**************************************************************************************//
//                                   Initialization
//**************************************************************************************//

PartGeom::PartGeom(vector<OrientPoint> _oriPointList)
{
	oriPointList = _oriPointList;

	for (int i = 0; i < oriPointList.size(); i++)
	{
		oriPointList[i].normal = oriPointList[i].normal / len(oriPointList[i].normal);
	}
}

PartGeom::~PartGeom()
{
	Clear();
}

void PartGeom::Clear()
{
	for (int i = 0; i < faceList.size(); i++)
	{
		delete faceList[i];
	}
	faceList.clear();

	for (int i = 0; i < edgeList.size(); i++)
	{
		delete edgeList[i];
	}
	edgeList.clear();

	for (int i = 0; i < verList.size(); i++)
	{
		delete verList[i];
	}
	verList.clear();
}




//**************************************************************************************//
//                                 Compute Part Geometry 
//**************************************************************************************//

PolyMesh* PartGeom::ComputePartGeometry()
{
	if (oriPointList.size() == 0)
		return NULL;

	///////////////////////////////////////////////////////////
	// 1. Compute all validate vertices

	ComputeFaces();
	ComputeEdges(faceList);
	ComputeVertices(faceList, edgeList);
	vector<Vector3f> pointList = ValidateVertices();


	///////////////////////////////////////////////////////////
	// 2. Compute convex hull of all vertices

	QuickHull *quickHull = new QuickHull();

	double transMat[16];
	identityd(transMat);

	quickHull->Init(pointList, transMat);
	quickHull->ComputeConvexHull();


	///////////////////////////////////////////////////////////
	// 3. Convert triangular mesh into polygonal mesh

	PolyMesh *polyMesh = new PolyMesh();

	polyMesh->Convert2PolyMesh(quickHull->verList, quickHull->triList);

	delete quickHull;

	return polyMesh;
}

void PartGeom::ComputePartGeometry_Matrix()
{
	// TODO:
	// 1. compute all possible vertices by solving 3x3 matrices
	// 2. check valid vertices by testing Nx3 inequalities
	// 3. compute convex hull of all valid vertices
}




//**************************************************************************************//
//                                Compute Faces and Edges
//**************************************************************************************//

void PartGeom::ComputeFaces()
{
	if (oriPointList.size() < 3)
		return;

	float radius = _MAX(5.0 * len(oriPointList[1].point - oriPointList[0].point), 2.0);

	for (int i = 0; i < oriPointList.size(); i++)
	{
		HypPlane *face = new HypPlane();

		face->point = oriPointList[i].point;
		face->normal = oriPointList[i].normal;

		face->radius = radius;
		face->planeID = i;

		faceList.push_back(face);
	}
}

void PartGeom::ComputeEdges(vector<HypPlane*> _faceList)
{
	for (int i = 0; i < _faceList.size() - 1; i++)
	{
		for (int j = i + 1; j < _faceList.size(); j++)
		{
			Vector3f normalA = _faceList[i]->normal;
			Vector3f normalB = _faceList[j]->normal;

			float dotp = normalA DOT normalB;

			//printf("dop: %.6f \n", dotp);

			if ( fabs(fabs(dotp) - 1.0) < FLOAT_ERROR_LARGE )
				continue;

			HypEdge *edge = PlanePlaneIntersect(_faceList[i], _faceList[j]);

			edge->planeIDs[0] = _faceList[i]->planeID;
			edge->planeIDs[1] = _faceList[j]->planeID;

			edge->edgeID = edgeList.size();

			edgeList.push_back(edge);
		}
	}

	//printf(" edgeNum: %d \n", edgeList.size());
}

HypEdge* PartGeom::PlanePlaneIntersect(HypPlane *faceA, HypPlane *faceB)
{
	Vector3f edgeDir= faceA->normal CROSS faceB->normal;
	edgeDir = edgeDir / len(edgeDir);

	Vector3f tempEdgeDir = faceA->normal CROSS edgeDir;
	tempEdgeDir = tempEdgeDir / len(tempEdgeDir);

	Vector3f hitPoint;
	bool isHit = EdgeHitFace(faceA->point, tempEdgeDir, faceB, hitPoint);
	if ( isHit == false )
	{
		printf("Warning: The line is parallel with the plane without intersection point. \n");
	}

	HypEdge *edge = new HypEdge();
	edge->point = hitPoint;
	edge->normal = edgeDir;

	//printf("faceA: [%.2f %.2f %.2f] \n", faceA->normal.x, faceA->normal.y, faceA->normal.z);
	//printf("faceB: [%.2f %.2f %.2f] \n", faceB->normal.x, faceB->normal.y, faceB->normal.z);

	//printf("normal: [%.2f %.2f %.2f] \n", edgeDir.x, edgeDir.y, edgeDir.z);
	//printf("point:  [%.2f %.2f %.2f] \n", hitPoint.x, hitPoint.y, hitPoint.z);

	//printf("\n");

	return edge;
}




//**************************************************************************************//
//             Compute Intersection Points between Edges and Tilt Planes
//**************************************************************************************//

void PartGeom::ComputeVertices(vector<HypPlane*> _faceList, vector<HypEdge*> _edgeList)
{
	for (int i = 0; i < _faceList.size(); i++)
	{
		for (int j = 0; j < _edgeList.size(); j++)
		{
			HypPlane *face = _faceList[i];
			HypEdge *edge = _edgeList[j];

			// Avoid intersection test between The edge and its two component faces
			if (face->planeID == edge->planeIDs[0] ||
				face->planeID == edge->planeIDs[1])
			{
				continue;
			}

			float dotp = face->normal DOT edge->normal;
			if ( fabs(dotp) < FLOAT_ERROR_SMALL )
			{
				continue;
			}

			Vector3f hitPt;
			bool isHit = EdgeHitFace(edge->point, edge->normal, face, hitPt);
			if ( isHit == false )
			{
				continue;
			}

			if (IsPointInList(hitPt, verList) == true )
			{
				continue;
			}

			HypVertex *vertex = new HypVertex();

			vertex->point = hitPt;
			vertex->verID = verList.size();

			verList.push_back(vertex);
		}
	}

	//printf("VerNum: %d \n", verList.size());
}

bool PartGeom::EdgeHitFace(Vector3f edgePt, Vector3f edgeDir, HypPlane *face, Vector3f &hitPt)
{
	float dotp = face->normal DOT edgeDir;
	if (fabs(dotp) < FLOAT_ERROR_SMALL)
	{
		return false;
	}

	Vector3f temp = face->point - edgePt;
	float m = (face->normal DOT temp) / (face->normal DOT edgeDir);

	hitPt[0] = edgePt[0] + m * edgeDir[0];
	hitPt[1] = edgePt[1] + m * edgeDir[1];
	hitPt[2] = edgePt[2] + m * edgeDir[2];

	return true;
}

bool PartGeom::IsPointInList(Vector3f tagtPoint, vector<HypVertex*> _verList)
{
	for (int i = 0; i < _verList.size(); i++)
	{
		float dist = len(tagtPoint - _verList[i]->point);

		// Note: this threshold depends on the scale of elements
		if (dist < FLOAT_ERROR_LARGE)
		{
			return true;
		}
	}

	return false;
}




//**************************************************************************************//
//                                 Validate Vertices
//**************************************************************************************//

vector<Vector3f> PartGeom::ValidateVertices()
{
	//////////////////////////////////////////////////////////////////
	// 1. Save only valid (intersection) vertices

	vector<Vector3f> pointList;
	for (int i = 0; i < verList.size(); i++)
	{
		verList[i]->isValid = IsValidVertex(verList[i]->point);

		if (verList[i]->isValid == true)
		{
			pointList.push_back(verList[i]->point);
		}
	}

	return pointList;
}

bool PartGeom::IsValidVertex(Vector3f verPt)
{
	for (int i = 0; i < faceList.size(); i++)
	{
		Vector3f tempPt = verPt - faceList[i]->point;
		float dotp = tempPt DOT faceList[i]->normal;

		// A vertex is invalid if it is not within the hyperplane of any face
		if (dotp > FLOAT_ERROR_LARGE)
		{
			return false;
		}
	}

	return true;
}




//**************************************************************************************//
//                                    Draw Base Polygon 
//**************************************************************************************//
//
//void PartGeom::DrawInnerPolygon()
//{
//	glColor3f(0.3, 0.9, 0.3);
//
//	glDisable(GL_LIGHTING);
//	glLineWidth(3.0);
//	glPointSize(10.0);
//
//	glBegin(GL_LINE_LOOP);
//	for (int i = 0; i < oriPointList.size(); i++)
//	{
//		Vector3f point = oriPointList[i].point;
//		glVertex3f(point.x, point.y, point.z);
//	}
//	glEnd();
//
//	glEnable(GL_LIGHTING);
//	glLineWidth(1.0);
//	glPointSize(1.0);
//}
//
//void PartGeom::DrawOriPoints()
//{
//	//Vector3f color = Vector3f(0.9, 0.2, 0.9);
//	//if (id % 2 == 0)   color = Vector3f(0.9, 0.2, 0.9);
//	//else              color = Vector3f(0.9, 0.5, 0.2);
//
//	//glColor3f(color.x, color.y, color.z);
//
//	glColor3f(0.9, 0.3, 0.9);
//
//	glDisable(GL_LIGHTING);
//	glLineWidth(3.0);
//	//glPointSize(16.0);
//	glPointSize(8.0);
//
//	float norLen = 0.8* len(oriPointList[1].point - oriPointList[0].point);
//
//	for (int i = 0; i < oriPointList.size(); i++)
//	{
//		Vector3f point = oriPointList[i].point;
//		Vector3f normal = oriPointList[i].normal;
//
//		Vector3f endPt = point + normal * norLen;
//
//		//glColor3f(0.9, 0.2, 0.9);
//		glBegin(GL_POINTS);
//		glVertex3f(point.x, point.y, point.z);
//		glEnd();
//
//		//glColor3f(0.3, 0.9, 0.3);
//		glBegin(GL_LINES);
//		glVertex3f(point.x, point.y, point.z);
//		glVertex3f(endPt.x, endPt.y, endPt.z);
//		glEnd();
//	}
//
//	glEnable(GL_LIGHTING);
//	glLineWidth(1.0);
//	glPointSize(1.0);
//}




//**************************************************************************************//
//                                  Draw Part Construction
//**************************************************************************************//

//void PartGeom::DrawFaces()
//{
//	for (int i = 0; i < faceList.size(); i++)
//	{
//		DrawFace(faceList[i]);
//	}
//}
//
//void PartGeom::DrawFace(HypPlane *face)
//{
//	Vector3f point = face->point;
//	Vector3f normal = face->normal;
//	float radius = face->radius;
//	Vector3f color = colorTable[face->planeID];
//
//	DrawPlane(point, normal, radius, color);
//}
//
//void PartGeom::DrawEdges()
//{
//	float length = 8.0;
//
//	for (int i = 0; i < edgeList.size(); i++)
//	{
//		DrawEdge(edgeList[i], length);
//	}
//}
//
//void PartGeom::DrawEdge(HypEdge *edge, float length)
//{
//	glDisable(GL_LIGHTING);
//	glPointSize(15.0);
//	glLineWidth(4.0);
//
//	//glColor3f(0.2, 0.2, 0.9);
//	//glBegin(GL_POINTS);
//	//glVertex3f(edge->point.x, edge->point.y, edge->point.z);
//	//glEnd();
//
//	Vector3f endPt1 = edge->point - 0.5f *length * edge->normal;
//	Vector3f endPt2 = edge->point + 0.5f *length * edge->normal;
//
//	glColor3f(0.9, 0.2, 0.2);
//	glBegin(GL_LINES);
//	glVertex3f(endPt1.x, endPt1.y, endPt1.z);
//	glVertex3f(endPt2.x, endPt2.y, endPt2.z);
//	glEnd();
//
//	glEnable(GL_LIGHTING);
//	glPointSize(1.0);
//	glLineWidth(1.0);
//}
//
//void PartGeom::DrawVertices()
//{
//	for (int i = 0; i < verList.size(); i++)
//	{
//		DrawVertex(verList[i]);
//	}
//}
//
//void PartGeom::DrawVertex(HypVertex *vertex)
//{
//	glDisable(GL_LIGHTING);
//	glPointSize(18.0);
//
//	Vector3f color;
//	if (vertex->isValid)   color = Vector3f(0.2, 0.9, 0.9);
//	else                   color = Vector3f(0.7, 0.7, 0.7);
//
//	glColor3f(color.x, color.y, color.z);
//	glBegin(GL_POINTS);
//	glVertex3f(vertex->point.x, vertex->point.y, vertex->point.z);
//	glEnd();
//
//	glEnable(GL_LIGHTING);
//	glPointSize(1.0);
//}