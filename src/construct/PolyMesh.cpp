///////////////////////////////////////////////////////////////
//
// PolyMesh.cpp
//
//   Convert a Triangular Mesh into a Polygonal Mesh
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 20/Dec/2017
//
//
///////////////////////////////////////////////////////////////


#include "Controls.h"
#include "HelpDefine.h"
#include "HelpFunc.h"
#include "math3D.h"
#include "Polygon.h"
#include "PolyMesh.h"
#include "ConvexHull_2D.h"


//**************************************************************************************//
//                                   Initialization
//**************************************************************************************//

PolyMesh::PolyMesh()
{

}

PolyMesh::~PolyMesh()
{
	for (int i = 0; i < polyList.size(); i++)
	{
		delete polyList[i];
	}
	polyList.clear();
}




//**************************************************************************************//
//                                  Mesh Operations
//**************************************************************************************//

void PolyMesh::ComputeCentroid()
{
	centroid = Vector3f(0, 0, 0);

	for (int i = 0; i < verList.size(); i++)
	{
		centroid += verList[i];
	}

	centroid /= (float)verList.size();
}

void PolyMesh::ComputeBBox()
{
	float minX, minY, minZ, maxX, maxY, maxZ;
	minX = minY = minZ = MAX_FLOAT;
	maxX = maxY = maxZ = MIN_FLOAT;

	for (int i = 0; i < verList.size(); i++)
	{
		Vector3f ver = verList[i];

		if (maxX < ver.x)  maxX = ver.x;
		if (minX > ver.x)  minX = ver.x;

		if (maxY < ver.y)  maxY = ver.y;
		if (minY > ver.y)  minY = ver.y;

		if (maxZ < ver.z)  maxZ = ver.z;
		if (minZ > ver.z)  minZ = ver.z;
	}

	bbox.minPt = Vector3f(minX, minY, minZ);
	bbox.maxPt = Vector3f(maxX, maxY, maxZ);
	bbox.cenPt = 0.5f*(bbox.minPt + bbox.maxPt);
}

void PolyMesh::ComputePolygons()
{
	for (int i = 0; i < polyList.size(); i++)
	{
		Polygon *poly = polyList[i];

		if (poly->vers.size() != 0)
		{
			printf("Warning: vertices of poly %d are already there. \n", i);
			continue;;
		}

		for (int j = 0; j < poly->verIDs.size(); j++)
		{
			int verID = poly->verIDs[j];

			Vector3f ver = verList[verID];

			poly->vers.push_back(ver);
		}

		poly->ComputeCenter();
		poly->ComputeNormal();
	}
}




//**************************************************************************************//
//                      Convert Triangular Mesh to Polygonal Mesh
//**************************************************************************************//

void PolyMesh::Convert2PolyMesh(vector<Vector3f> inVerList, vector<Vector3i> inTriList)
{
	// Initialize each face as a triangle
	InitPolyMesh(inVerList, inTriList);

	//return;

	// Keep merging neighboring faces that are co-planar
	while ( true )
	{
		bool isFinish = true;

		for (int i = 0; i < polyList.size(); i++)
		{
			for (int j = 0; j < polyList.size(); j++)
			{
				if (j == i)
					continue;

				Polygon *polyA = polyList[i];
				Polygon *polyB = polyList[j];

				//printf("i=%d  j=%d \n", i, j);

				if (IsCoplanar(polyA, polyB) == true)
				{
					isFinish = false;

					Polygon *newPoly = MergePolygons(polyA, polyB);
					UpdatePolyList(polyA, i, polyB, j, polyList);

					break;			
				}
			}

			if ( isFinish == false )
			{
				break;
			}
		}

		if ( isFinish == true )
		{
			break;
		}
	}
}

void PolyMesh::InitPolyMesh(vector<Vector3f> inVerList, vector<Vector3i> inTriList)
{
	// Initialize vertices
	verList = inVerList;

	// Initialize faces
	for (int i = 0; i < inTriList.size(); i++)
	{
		Polygon *poly = new Polygon();

		for (int j = 0; j < 3; j++)
		{
			int verID = inTriList[i][j];
			Vector3f ver = inVerList[verID];

			poly->verIDs.push_back(verID);
			poly->vers.push_back(ver);
		}

		poly->ComputeNormal();
		//poly->ComputeCenter();

		polyList.push_back(poly);
	}
}


bool PolyMesh::IsCoplanar(Polygon *polyA, Polygon *polyB)
{
	Vector3f cenDiffVec = polyB->center - polyA->center;
	cenDiffVec = cenDiffVec / len(cenDiffVec);

	float dotpA = polyA->normal DOT cenDiffVec;
	float dotpB = polyB->normal DOT cenDiffVec;

	//printf("dotpA: %.6f    dotpB: %.6f \n", dotpA, dotpB);

	if (fabs(dotpA) < CO_PLANAR_THRES && fabs(dotpB) < CO_PLANAR_THRES)
	{
		return true;
	}
	else
	{
		return false;
	}
}


Polygon* PolyMesh::MergePolygons(Polygon *polyA, Polygon *polyB)
{
	///////////////////////////////////////////////////////
	// 1. Unique vertices of ployA and polyB

	vector<Vector3f> polyVers;
	polyVers = polyA->vers;

	for (int i = 0; i < polyB->vers.size(); i++)
	{
		Vector3f ver = polyB->vers[i];

		if (GetPointIndexInList(ver, polyVers) == ELEMENT_OUT_LIST)
		{
			polyVers.push_back(ver);
		}
	}


	///////////////////////////////////////////////////////
	// 2. Compute transform matrix for the vertex projection

	double transMat[16];
	Vector3f planeVec = polyB->center - polyA->center;
	GetPlaneTransformMatrix(polyA->normal, planeVec, transMat);

	double inveTransMat[16];
	memcpy(inveTransMat, transMat, sizeof(double) * 16);
	if (invert4by4(inveTransMat) == 0)  printf("Inverse Matrix Error \n");


	///////////////////////////////////////////////////////
	// 3. Transform the vertices such that their z-coordinates are the same

	vector<Vector3f> projPtList;
	for (int i = 0; i < polyVers.size(); i++)
	{
		Vector3f projPt;
		MultiplyPoint(polyVers[i], transMat, projPt);

		projPtList.push_back(projPt);
	}


	///////////////////////////////////////////////////////
	// 4. Compute 2D convex hull of the projected vertices

	vector<Vector3f> hullPtList = convex_hull(projPtList);


	///////////////////////////////////////////////////////
	// 5. Project back the hull vertices (note: order is important)

	vector<Vector3f> newPolyVers;
	for (int i = 0; i < hullPtList.size(); i++)
	{
		Vector3f newVer;
		MultiplyPoint(hullPtList[i], inveTransMat, newVer);

		newPolyVers.push_back(newVer);
	}

	vector<int> newPolyVerIDs;
	for (int i = 0; i < newPolyVers.size(); i++)
	{
		Vector3f newVer = newPolyVers[i];

		int verID = GetPointIndexInList(newVer, verList);
		if (verID == ELEMENT_OUT_LIST)
		{
			printf("Warning: newVer should be inside verList. \n");
			break;
		}

		//printf("i=%d  id: %d \n", i, verID);

		newPolyVerIDs.push_back(verID);
		newPolyVers[i] = verList[verID];  // Avoid modifying vertices during the projection
	}


	///////////////////////////////////////////////////////
	// 6. Construct the merged polygon

	Polygon *newPoly = new Polygon();

	newPoly->vers = newPolyVers;
	newPoly->verIDs = newPolyVerIDs;

	//newPoly->ComputeCenter();
	newPoly->ComputeNormal();

	return newPoly;
}

void PolyMesh::UpdatePolyList(Polygon *polyA, int polyAID, Polygon *polyB, int polyBID, vector<Polygon*> &polyList)
{
	Polygon *newPoly = MergePolygons(polyA, polyB);

	vector<Polygon*> newPolyList;
	for (int i = 0; i < polyList.size(); i++)
	{
		if ( i==polyAID || i== polyBID )
		{
			continue;
		}

		newPolyList.push_back(polyList[i]);
	}

	delete polyA;
	delete polyB;

	newPolyList.push_back(newPoly);

	polyList = newPolyList;
}




//**************************************************************************************//
//                                   Create Mesh
//**************************************************************************************//

void PolyMesh::CreateMesh_Fan(Vector3f verA, Vector3f verB, Vector3f verM)
{
	Polygon *poly = new Polygon();
	poly->CreatePolygon_Fan(verA, verB, verM);

	verList = poly->vers;
	polyList.push_back(poly);
}




//**************************************************************************************//
//                                   Save OBJ File
//**************************************************************************************//

void PolyMesh::WriteOBJModel(char *objFileName)
{
	FILE *fp;
	if ((fp = fopen(objFileName, "w+")) == NULL)
	{
		printf("Error: file not exists! \n");
		return;
	}
	else
	{
		////////////////////////////////////////////////////////
		// Write the vertex info of piece OBJ Part

		//fprintf(fp, "# Wavefront OBJ generated by Peng SONG \n\n");

		fprintf(fp, "# %d vertices \n", verList.size());

		for (int i = 0; i < verList.size(); i++)
		{
			fprintf(fp, "v %f %f %f \n", verList[i].x, verList[i].y, verList[i].z);
		}
		fprintf(fp, "\n");


		////////////////////////////////////////////////////////
		// Write the quad info of piece OBJ Part

		fprintf(fp, "# %d faces \n", polyList.size());

		for (int i = 0; i < polyList.size(); i++)
		{
			Polygon *poly = polyList[i];

			fprintf(fp, "f ");
			for (int j = 0; j < poly->verIDs.size(); j++)
			{
				//Since the index in OBJ file starting from 1 instead of 0, we need to add 1 to each index
				fprintf(fp, " %d", poly->verIDs[j] + 1);
			}
			fprintf(fp, "\n");

		}
		fprintf(fp, "\n");

		fclose(fp);
	}
}




//**************************************************************************************//
//                                     Rendering
//**************************************************************************************//
//
//void PolyMesh::DrawMesh()
//{
//	// Draw the polygons
//	for (int i = 0; i < polyList.size(); i++)
//	{
//		glBegin(GL_POLYGON);
//
//		Vector3f faceNor = polyList[i]->normal;
//		glNormal3f(faceNor.x, faceNor.y, faceNor.z);
//
//		for (int j = 0; j < polyList[i]->vers.size(); j++)
//		{
//			Vector3f vertexPos = polyList[i]->vers[j];
//			glVertex3f(vertexPos.x, vertexPos.y, vertexPos.z);
//		}
//		glEnd();
//	}
//}
//
//void PolyMesh::DrawMeshWire(float width, Vector3f color)
//{
//	float epsilon = 1e-6;
//	float projMat[16];
//	glGetFloatv(GL_PROJECTION_MATRIX, projMat);
//
//	glDisable(GL_LIGHTING);
//	glColor3f(color.x, color.y, color.z);
//	glLineWidth(width);
//
//	// Move the mesh toward the eye a bit to avoid z-fighting issue
//	glMatrixMode(GL_PROJECTION);
//	glPushMatrix();
//	glLoadIdentity();
//	glTranslatef(0.0f, 0.0f, -epsilon);
//	glMultMatrixf(projMat);
//
//	// Draw the triangles
//	for (int i = 0; i < polyList.size(); i++)
//	{
//		glBegin(GL_LINE_LOOP);
//		for (int j = 0; j < polyList[i]->vers.size(); j++)
//		{
//			Vector3f vertexPos = polyList[i]->vers[j];
//			glVertex3f(vertexPos.x, vertexPos.y, vertexPos.z);
//		}
//		glEnd();
//	}
//
//	glPopMatrix();
//	glMatrixMode(GL_MODELVIEW);
//
//	glLineWidth(1.0);
//	glEnable(GL_LIGHTING);
//
//	//DrawWireCuboid(bbox.minPt, bbox.maxPt, 2.0, Vector3f(0, 0.9, 0));
//}
//
//
//void PolyMesh::DrawFaceNormals(Vector3f color)
//{
//	const float normalLen = 0.6f;
//
//	glDisable(GL_LIGHTING);
//	glPointSize(12.0);
//	glLineWidth(2.0);
//	glColor3f(color.x, color.y, color.z);
//
//	for (int i = 0; i < polyList.size(); i++)
//	{
//		Vector3f point = polyList[i]->center;
//		Vector3f normal = polyList[i]->normal;
//		Vector3f endPt = point + normalLen * normal;
//
//		glBegin(GL_POINTS);
//		glVertex3f(point.x, point.y, point.z);
//		glEnd();
//
//		glBegin(GL_LINES);
//		glVertex3f(point.x, point.y, point.z);
//		glVertex3f(endPt.x, endPt.y, endPt.z);
//		glEnd();
//	}
//
//	glPointSize(1.0);
//	glLineWidth(1.0);
//	glEnable(GL_LIGHTING);
//}