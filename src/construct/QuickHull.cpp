///////////////////////////////////////////////////////////////
//
// QuickHull.cpp
//
//   Find Convex Hull of a Set of 3D Points using Quick Hull Algorithm:
//   1. http://thomasdiewald.com/blog/?p=1888
//   2. C. Barber et al. "The Quickhull Algorithm for Convex Hulls"
//      ACM Transactions on Mathematical Software, 2015
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 08/Nov/2017
//
//
///////////////////////////////////////////////////////////////


#include "QuickHull.h"
#include "HelpDefine.h"
#include "HelpFunc.h"
#include "HEdgeMesh.h"


//**************************************************************************************//
//                                   Initialization
//**************************************************************************************//

QuickHull::QuickHull()
{
	hedgeMesh = NULL;

#ifdef DEBUG_QUICK_HULL
	iter = 0;
#endif 
}

QuickHull::~QuickHull()
{

}

void QuickHull::Clear()
{
#ifdef DEBUG_QUICK_HULL
	iter = 0;
#endif

	verList.clear();
	triList.clear();

	for (int i=0; i<triangles.size(); i++)
	{
		delete triangles[i];
	}
	triangles.clear();

	if ( hedgeMesh != NULL )
	{
		delete hedgeMesh;
		hedgeMesh = NULL;
	}
	faceList.clear();
}

void QuickHull::Init(vector<Vector3f> _pointList, double _transMat[16])
{
	pointList = _pointList;

	memcpy(transMat, _transMat, sizeof(double) * 16);
}




//**************************************************************************************//
//                        Compute Convex Hull using Quick Hull
//**************************************************************************************//

void QuickHull::ComputeConvexHull()
{
	if (pointList.size() <= 3)
	{
		//printf("Warning: The number of input points should be larger than 3. \n");
		return;
	}

	// Clear memory
	Clear();

	// Create initial tetrahedron
	verList = CreateTetrahedron();

	// Build half-edge structure 
	hedgeMesh = new HEdgeMesh();
	hedgeMesh->InitMesh(verList, triList);
	hedgeMesh->BuildMesh();

#ifdef DEBUG_QUICK_HULL
	char fileName[FILE_NAME_LENGTH];
	sprintf(fileName, "C:\\_SongPeng\\poly_%d.obj", iter);
	WriteOBJModel(fileName, verList, triList);
	iter++;
#endif

	// Keep expanding the convex hull until all input points are included
	while ( true )
	{
		bool isTerminate = IsTerminateExpand();
		if (isTerminate == true)
			break;
					
		ExpandConvexHull(true);

#ifdef DEBUG_QUICK_HULL
		sprintf(fileName, "C:\\_SongPeng\\poly_%d.obj", iter);
		WriteOBJModel(fileName, verList, triList);
		iter++;
#endif
	}
}

vector<Vector3f> QuickHull::CreateTetrahedron()
{
	/////////////////////////////////////////////////////////////////
	// 1. Find 6 extreme points (EPs) along -x/+x, -y/+y, -z/+z axes
	//    Note: there exist duplicated points in 6 EPs

	Vector3f minXPt = Vector3f( MAX_FLOAT, 0,         0         );
	Vector3f minYPt = Vector3f( 0,         MAX_FLOAT, 0         );
	Vector3f minZPt = Vector3f( 0,         0,         MAX_FLOAT );
	Vector3f maxXPt = Vector3f( MIN_FLOAT, 0,         0         );
	Vector3f maxYPt = Vector3f( 0,         MIN_FLOAT, 0         );
	Vector3f maxZPt = Vector3f( 0,         0,         MIN_FLOAT );

	for (int i=0; i<pointList.size(); i++)
	{
		if ( pointList[i].x < minXPt.x )    minXPt = pointList[i];
		if ( pointList[i].x > maxXPt.x )    maxXPt = pointList[i];

		if ( pointList[i].y < minYPt.y )    minYPt = pointList[i];
		if ( pointList[i].y > maxYPt.y )    maxYPt = pointList[i];

		if ( pointList[i].z < minZPt.z )    minZPt = pointList[i];
		if ( pointList[i].z > maxZPt.z )    maxZPt = pointList[i];
	}

	vector<Vector3f> EPList;
	EPList.push_back( minXPt );
	EPList.push_back( minYPt );
	EPList.push_back( minZPt );
	EPList.push_back( maxXPt );
	EPList.push_back( maxYPt );
	EPList.push_back( maxZPt );


	/////////////////////////////////////////////////////////////////
	// 2. Find 2 most distant EPs to build base-line of the base-triangle

	Vector3f ep1, ep2;
	float maxDist1 = MIN_FLOAT;

	for (int i=0; i<EPList.size()-1; i++)
	{
		for (int j=i+1; j<EPList.size(); j++)
		{
			float dist = len(EPList[j]-EPList[i]);

			if ( dist > maxDist1)
			{
				maxDist1 = dist;
				ep1 = EPList[i];
				ep2 = EPList[j];
			}
		}
	}


	/////////////////////////////////////////////////////////////////
	// 3. Find most distant EP to the base-line to construct the base-triangle

	Vector3f ep3;
	float maxDist2 = MIN_FLOAT;
	for (int i=0; i<EPList.size(); i++)
	{
		if ( EPList[i] == ep1 || EPList[i] == ep2 )
			continue;

		float dist = PointLineDistance(EPList[i], ep1, ep2);

		if ( dist > maxDist2 )
		{
			maxDist2 = dist;
			ep3 = EPList[i];
		}
	}

	// Construct the base-triangle
	Triangle *baseTri = new Triangle();
	baseTri->v[0] = ep1;
	baseTri->v[1] = ep2;
	baseTri->v[2] = ep3;
	baseTri->ComputeNormal();


	/////////////////////////////////////////////////////////////////
	// 4. Find most distant EP to the base-triangle as 4th vertex

	Vector3f ep4;
	float maxDist3 = MIN_FLOAT;
	//for (int i=0; i<EPList.size(); i++)
	for (int i=0; i<pointList.size(); i++)
	{
		//Vector3f pt = EPList[i];
		Vector3f pt = pointList[i];

		if ( pt == ep1 || pt == ep2 || pt == ep3 )
			continue;

		float dist = PointPlaneDistance(pt, baseTri->v[0], baseTri->normal);

		if ( dist > maxDist3 )
		{
			maxDist3 = dist;
			ep4 = pt;
		}
	}

	verList.clear();
	verList.push_back( ep1 );
	verList.push_back( ep2 );
	verList.push_back( ep3 );
	verList.push_back( ep4 );


	/////////////////////////////////////////////////////////////////
	// 5. Construct the tetrahedron

	Triangle *tri1 = new Triangle();
	Triangle *tri2 = new Triangle();
	Triangle *tri3 = new Triangle();

	tri1->v[0] = ep1;  	tri1->v[1] = ep2;  tri1->v[2] = ep4;  
	tri2->v[0] = ep1;  	tri2->v[1] = ep3;  tri2->v[2] = ep4;  
	tri3->v[0] = ep2;  	tri3->v[1] = ep3;  tri3->v[2] = ep4;  

	triangles.push_back( baseTri );
	triangles.push_back( tri1 );
	triangles.push_back( tri2 );
	triangles.push_back( tri3 );

	triList.clear();
	triList.push_back( Vector3i(0, 1, 2) );
	triList.push_back( Vector3i(0, 1, 3) );
	triList.push_back( Vector3i(0, 2, 3) );
	triList.push_back( Vector3i(1, 2, 3) );

	//printf("tri num: %d \n", triList.size());


	/////////////////////////////////////////////////////////////////
	// 6. Correct normal for each face of the tetrahedron

	CorrectTriangleNormal();

	return verList;
}


void QuickHull::CorrectTriangleNormal()
{
	// Compute initial normal of each triangle
	for (int i = 0; i < triangles.size(); i++)
	{
		triangles[i]->ComputeCenter();
		triangles[i]->ComputeNormal();
	}

	// Correct normal for each triangle
	for (int i = 0; i < triangles.size(); i++)
	{
		Vector3f rayOrg = triangles[i]->center;
		Vector3f rayNor = triangles[i]->normal;

		// The normal should not face any other triangles (since the polyhedron is convex)
		for (int j = 0; j < verList.size(); j++)
		{
			Vector3f tempVec = verList[j] - rayOrg;
			float dist = tempVec DOT rayNor;

			if (dist > FLOAT_ERROR_LARGE)
			{
				// Reverse triangle's normal
				triangles[i]->CorrectNormal(-1.0f*rayNor);

				// Swap vertexIDs of the triangle
				int tempVerID = triList[i][2];
				triList[i][2] = triList[i][1];
				triList[i][1] = tempVerID;
				break;
			}
		}
	}
}




//**************************************************************************************//
//                             Check If Terminate Expanding
//**************************************************************************************//

bool QuickHull::IsTerminateExpand()
{
	// 1. Create light-faces (i.e., save light-points for each face)
	CreateLightFaces(verList);

	// 2. Check if all light-points are empty
	bool isTerminate = true;
	for (int i = 0; i < faceList.size(); i++)
	{
		//printf("i: %d   lightNum: %d \n", i, faceList[i].lightPts.size());

		if (faceList[i].lightPts.size() != 0)
		{
			isTerminate = false;
		}
	}

	return isTerminate;
}

void QuickHull::CreateLightFaces(vector<Vector3f> vertices)
{
	// 1. Find all remaining input points
	vector<Vector3f> remaPts;
	for (int i = 0; i < pointList.size(); i++)
	{
		if (IsPointInList(pointList[i], vertices) == false)
		{
			remaPts.push_back(pointList[i]);
		}
	}

	// 2. Create initial light-faces
	faceList.clear();
	for (int i = 0; i < triangles.size(); i++)
	{
		TriFace face;
		face.tri = triangles[i];

		faceList.push_back(face);
	}

	// 3. Push back light-points for each face
	for (int i = 0; i < remaPts.size(); i++)
	{
		for (int j = 0; j < faceList.size(); j++)
		{
			if (IsPointSeeTriangle(remaPts[i], faceList[j].tri) == true)
			{
				faceList[j].lightPts.push_back(remaPts[i]);
				break;
			}
		}
	}
}




//**************************************************************************************//
//                                 Expand Convex Hull
//**************************************************************************************//

void QuickHull::ExpandConvexHull(bool isAdd)
{
	// 1. Find a remaining input point (if exist) as a new vertex and 
	//    get also its corresponding light-face
	int currTriID;
	Vector3f newVertex;
	if ( FindNewVertex(currTriID, newVertex) == false )
		return;

	// 2. Find all faces that can be seen from the point
	vector<int> lightFaceIDs = GetLightFaces(currTriID, newVertex); 

	// 3. Add new triangles for the light-faces
	if ( isAdd )
	{
		AddNewTriangles(newVertex, lightFaceIDs);
	}

	// 4. Build half-edge structure for the updated convex hull
	if ( hedgeMesh != NULL )
	{
		delete hedgeMesh;
		hedgeMesh = NULL;
	}
	hedgeMesh = new HEdgeMesh();
	hedgeMesh->InitMesh(verList, triList);
	hedgeMesh->BuildMesh();
}

bool QuickHull::FindNewVertex(int &triID, Vector3f &vertex)
{
	bool isFindVertex = false;

	for (int i=0; i<faceList.size(); i++)
	{
		int lightPtNum = faceList[i].lightPts.size();
		if ( lightPtNum == 0 )
			continue;

		//printf("faceID %d  ptNum: %d \n", i, lightPtNum);

		// Get a target light-face
		Triangle *tri = faceList[i].tri;
		int currTriID = i;
		Vector3f maxDistPt;
		float maxDist = MIN_FLOAT;

		// Get most distant point of the face's point-set (as a new vertex)
		for (int j=0; j<faceList[i].lightPts.size(); j++)
		{
			Vector3f pt = faceList[i].lightPts[j];
			float dist = PointPlaneDistance(pt, tri->v[0], tri->normal);
			//printf("[%.2f %.2f %.2f] \n", pt.x, pt.y, pt.z);

			if ( dist > maxDist )
			{
				maxDist = dist;
				maxDistPt = pt;
			}
		}
		//printf("maxDistPt: [%.2f %.2f %.2f] \n", maxDistPt.x, maxDistPt.y, maxDistPt.z);

		// Update the triangle ID and the new vertex
		triID = currTriID;
		vertex = maxDistPt;
		isFindVertex = true;

		break;
	}

	return isFindVertex;
}

vector<int> QuickHull::GetLightFaces(int triID, Vector3f vertex)
{
	// Find all faces that can be seen from that point
	vector<int> lightFaces;
	lightFaces.push_back( triID );

	// The candidate faces must be the neighbors of the current face
	vector<int> neighborFaces = hedgeMesh->GetNeighborFaces( triID );

	for (int i=0; i<neighborFaces.size(); i++)
	{
		int faceID = neighborFaces[i];

		if ( IsPointSeeTriangle(vertex, triangles[faceID]) == true )
		{
			lightFaces.push_back( faceID );
		}
	}

	//printf("Neighbor Faces: ");
	//for (int i=0; i<neighborFaces.size(); i++)
	//{
	//	printf(" %d ", neighborFaces[i]);
	//}
	//printf("\n");

	//printf("Light FaceIDs: ");
	//for (int i=0; i<lightFaces.size(); i++)
	//{
	//	printf(" %d ", lightFaces[i]);
	//	int faceID = lightFaces[i];
	//}
	//printf("\n");

	return lightFaces;
}

void QuickHull::AddNewTriangles(Vector3f vertex, vector<int> lightFaceIDs)
{
	// 1. Push back the new vertex
	verList.push_back( vertex );

	// 2. Remove light-faces from triList and triangles
	vector<Vector3i> oldTriList = triList;
	vector<Triangle*> oldTriangles = triangles;
	vector<Triangle*> remvTriangles;

	triList.clear();
	triangles.clear();
	for (int i=0; i<oldTriList.size(); i++)
	{
		if (GetElementIndexInList(i, lightFaceIDs) == ELEMENT_OUT_LIST)
		{
			triList.push_back( oldTriList[i] );
			triangles.push_back( oldTriangles[i] );
		}
		else
		{
			remvTriangles.push_back(oldTriangles[i]);
		}
	}

	// 3. Release the memory of all light-faces
	for (int i = 0; i < remvTriangles.size(); i++)
	{
		delete remvTriangles[i];
	}

	// 4. Extract horizon edges of light-faces (i.e., edges that are not shared by light-faces)
	vector<Edge*> edges = hedgeMesh->GetNonSharedEdges( lightFaceIDs );

	// 5. Construct a new triangle for each horizon edge
	for (int i=0; i<edges.size(); i++)
	{
		int staPtID = edges[i]->staVer->id;
		int endPtID = edges[i]->endVer->id;
		
		Vector3i newTri = Vector3i(staPtID, endPtID, verList.size()-1);

		Triangle *tri = new Triangle();
		for (int j=0; j<3; j++)
		{
			int index = newTri[j];
			tri->v[j] = verList[index];
		}

		CorrectTriangleNormal(newTri, tri, triList);

		tri->ComputeCenter();
		tri->ComputeNormal();

		triList.push_back( newTri );
		triangles.push_back( tri );
	}
}

void QuickHull::CorrectTriangleNormal(Vector3i &tri, Triangle *triangle, const vector<Vector3i> triList)
{
	for (int i = 0; i < 3; i++)
	{
		int staVerID = tri[i];
		int endVerID = tri[(i + 1) % 3];

		// Correct triangle normal if any directed edge in the triangle is found also in the triList
		for (int j = 0; j < triList.size(); j++)
		{
			for (int k = 0; k < 3; k++)
			{
				int tempStaVerID = triList[j][k];
				int tempEndVerID = triList[j][(k+1)%3];

				if ( staVerID == tempStaVerID && endVerID == tempEndVerID )
				{
					Vector3f tempVer;
					tempVer = triangle->v[2];
					triangle->v[2] = triangle->v[1];
					triangle->v[1] = tempVer;

					// Swap also the order of vertex indices 
					float tempVerID = tri[2];
					tri[2] = tri[1];
					tri[1] = tempVerID;

					return;
				}
			}
		}
	}	
}




//**************************************************************************************//
//                                Utility Functions 
//**************************************************************************************//

bool QuickHull::IsPointInList(Vector3f tagtPt, vector<Vector3f> points)
{
	for (int i=0; i<points.size(); i++)
	{
		//if ( points[i] == tagtPt )
		if (len(points[i] - tagtPt) < FLOAT_ERROR_SMALL)
		{
			return true;
		}
	}

	return false;
}

bool QuickHull::IsPointSeeTriangle(Vector3f point, Triangle *tri)
{
	Vector3f triCen = (tri->v[0] + tri->v[1] + tri->v[2]) / 3.0f;
	Vector3f tagtvec = point - triCen;

	// Point is on the back side of the triangle
	float dotP = tagtvec DOT tri->normal;

	// Note: this threshold is important
	//if ( dotP <= 0 )
	if (dotP < FLOAT_ERROR_SMALL)
		return false;

	return true;
}