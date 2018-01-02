///////////////////////////////////////////////////////////////
//
// HEdgeMesh.cpp
//
//   Half-Edge Mesh Structure Class
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 08/Nov/2017
//
///////////////////////////////////////////////////////////////

#include "HEdgeMesh.h"
#include "HelpStruct.h"
#include "HelpFunc.h"
//#include "GL\glut.h"
#include "math3D.h"
#include "time.h"


//**************************************************************************************//
//                                   Initialization
//**************************************************************************************//

HEdgeMesh::HEdgeMesh()
{
	identityd(prevMVMatrix);
}

HEdgeMesh::~HEdgeMesh()
{
	for (int i=0; i<vertexList.size(); i++)
	{
		delete vertexList[i];
	}
	vertexList.clear();

	for (int i=0; i<edgeList.size(); i++)
	{
		delete edgeList[i];
	}
	edgeList.clear();

	for (int i=0; i<faceList.size(); i++)
	{
		delete faceList[i];
	}
	faceList.clear();
}

void HEdgeMesh::InitMesh(vector<Vector3f> _vertices, vector<Vector3i> _triangles)
{
	vertices  = _vertices;
	triangles = _triangles;
}




//**************************************************************************************//
//                             Build Half Edge Structure
//**************************************************************************************//

void HEdgeMesh::BuildMesh()
{
	//clock_t beginTime = clock();

	GenerateVertices();
	GenerateFaces();
	GenerateEdges();

	UpdateVertices();
	CreateLightFaces();
	UpdateEdges();
	
	//clock_t endTime = clock();
	//float endElapsed = ((float) (endTime - beginTime)) / (CLOCKS_PER_SEC); // Time unit: second
	//printf("Time taken for building HalfEdge structure is %.3f  seconds \n\n", endElapsed);
}

void HEdgeMesh::GenerateVertices()
{
	vertexList.clear();

	for (int i=0; i<vertices.size(); i++)
	{
		Vertex *vertex = new Vertex();

		vertex->id  = i;
		vertex->point = vertices[i];

		vertexList.push_back( vertex );
	}
}

void HEdgeMesh::GenerateFaces()
{
	faceList.clear();

	for (int i=0; i<triangles.size(); i++)
	{
		Face *face = new Face();

		face->id = i;
		for (int j=0; j<3; j++)
		{
			int verID = triangles[i][j];
			face->vertices.push_back( vertexList[verID] );	
		}
		ComputeFaceNormal( face );

		faceList.push_back( face );
	}
}

void HEdgeMesh::GenerateEdges()
{
	edgeList.clear();

	for (int i=0; i<triangles.size(); i++)
	{
		for (int j=0; j<3; j++)
		{
			int staVerID = triangles[i][j];
			int endVerID = triangles[i][(j+1)%3];
			
			//if ( j == 2 ) endVerID = triangles[i][0];
			//else          endVerID = triangles[i][j+1];

			Edge *edge = new Edge();

			edge->id = i*3+j;
			edge->staVer = vertexList[staVerID];
			edge->endVer = vertexList[endVerID];
			edge->face = faceList[i];

			edgeList.push_back( edge );
		}
	}
}

void HEdgeMesh::UpdateVertices()
{
	for (int i=0; i<edgeList.size(); i++)
	{
		edgeList[i]->staVer->outEdges.push_back( edgeList[i] );
	}

	for (int i=0; i<vertexList.size(); i++)
	{
		ComputeVertexNormal( vertexList[i] );
	}
}

void HEdgeMesh::CreateLightFaces()
{
	for (int i=0; i<faceList.size();i ++)
	{
		int edgeID = 3*i;
		faceList[i]->startEdge = edgeList[edgeID];
	}
}

void HEdgeMesh::UpdateEdges()
{
	////////////////////////////////////////////////////////
	// 1. Get next and previous edges

	for (int i=0; i<edgeList.size(); i++)
	{
		int nextID = GetNextEdgeID( i );
		int prevID = GetPrevEdgeID( i );

		edgeList[i]->next = edgeList[nextID];
		edgeList[i]->prev = edgeList[prevID];
	}


	////////////////////////////////////////////////////////
	// 2. Get twin/opposite edge (need to speed it up for large mesh)
	// TODO: (1) put triangles into corresponding buckets to speed-up
	//       (2) only compare with neighboring triangles

	for (int i=0; i<edgeList.size(); i++)
	{
		if ( edgeList[i]->twin != NULL )
		{
			continue;
		}

		for (int j=0; j<edgeList.size(); j++)
		{
			if( IsTwinEdges(edgeList[i], edgeList[j]) )
			{
				edgeList[i]->twin = edgeList[j];
				edgeList[j]->twin = edgeList[i];
				break;
			}
		}
	}


	////////////////////////////////////////////////////////
	// 3. Warning for empty twin edges, probably due to incorrect input (i.e., triangles )

	for (int i = 0; i < edgeList.size(); i++)
	{
		if ( edgeList[i]->twin == NULL )
		{
			printf("Warning: some twin edges are empty. \n");
		}
	}
}




//**************************************************************************************//
//                                Utility Functions
//**************************************************************************************//

int HEdgeMesh::GetNextEdgeID(int edgeID)
{
	// Note: this is only applicable to triangle mesh
	int nextEdgeID;

	if ( edgeID%3 == 0 )         nextEdgeID = edgeID + 1;
	else if ( edgeID%3 == 1 )    nextEdgeID = edgeID + 1;
	else if ( edgeID%3 == 2 )    nextEdgeID = edgeID - 2;

	return nextEdgeID;
}

int HEdgeMesh::GetPrevEdgeID(int edgeID)
{
	// Note: this is only applicable to triangle mesh
	int prevEdgeID;

	if ( edgeID%3 == 0 )         prevEdgeID = edgeID + 2;
	else if ( edgeID%3 == 1 )    prevEdgeID = edgeID - 1;
	else if ( edgeID%3 == 2 )    prevEdgeID = edgeID - 1;

	return prevEdgeID;
}

bool HEdgeMesh::IsTwinEdges(Edge *edgeA, Edge *edgeB)
{
	if ( edgeA->staVer->id == edgeB->endVer->id && 
		 edgeA->endVer->id == edgeB->staVer->id )
	{
		return true;
	}

	return false;
}

void HEdgeMesh::ComputeFaceNormal(Face *face)
{
	Vector3f lineA = face->vertices[1]->point - face->vertices[0]->point;
	Vector3f lineB = face->vertices[2]->point - face->vertices[0]->point;
	Vector3f normal = lineA CROSS lineB;

	face->normal = normal / len(normal);
}

void HEdgeMesh::ComputeVertexNormal(Vertex *vertex)
{
	vector<Face*> neiborFaces = GetNeighborFaces(vertex);

	Vector3f normalSum = Vector3f(0,0,0);
	for (int i=0; i<neiborFaces.size(); i++)
	{
		normalSum += neiborFaces[i]->normal;
	}

	vertex->normal = normalSum / len(normalSum);
}




//**************************************************************************************//
//                                 Get Neighbors
//**************************************************************************************//

vector<Vertex*> HEdgeMesh::GetNeighborVertices(Vertex *vertex)
{
	vector<Vertex*> neiborVertices;

	for (int i=0; i<vertex->outEdges.size(); i++)
	{
		Vertex* neiborVer = vertex->outEdges[i]->endVer;
		neiborVertices.push_back( neiborVer );
	}

	return neiborVertices;
}

vector<Face*> HEdgeMesh::GetNeighborFaces(Vertex *vertex)
{
	vector<Face*> neiborFaces;

	for (int i=0; i<vertex->outEdges.size(); i++)
	{
		Face* neiborFace = vertex->outEdges[i]->face;
		neiborFaces.push_back( neiborFace );
	}

	return neiborFaces;
}

vector<int> HEdgeMesh::GetNeighborFaces(int faceID)
{
	vector<int> neiborFaceIDs;

	Face *face = faceList[faceID];
	Edge *startEdge = face->startEdge;
	Edge *currEdge  = startEdge;

	while ( true )
	{
		// Push back a neighboring face
		Face *neiborFace = currEdge->twin->face;

		neiborFaceIDs.push_back( neiborFace->id );

		// Move to the next edge
		currEdge = currEdge->next;

		// Stop if the next edge is the same as the start edge
		if ( currEdge->IsEqual( startEdge ) )
		{
			break;
		}
	}

	return neiborFaceIDs;
}

// Note: this function is only applicable to triangular mesh
vector<Edge*> HEdgeMesh::GetNonSharedEdges(vector<int> faceIDs)
{
	vector<Edge*> outputEdges;

	for (int i=0; i<faceIDs.size(); i++)
	{
		int faceID = faceIDs[i];
		Face *face = faceList[faceID];

		// Get 3 neighboring faces (only for triangles)
		Face *face1 = face->startEdge->twin->face;
		Face *face2 = face->startEdge->next->twin->face;
		Face *face3 = face->startEdge->prev->twin->face;

		// Push back corresponding edge if cannot find the neighboring face in faceIDs
		if (GetElementIndexInList(face1->id, faceIDs) == ELEMENT_OUT_LIST)    outputEdges.push_back(face->startEdge);
		if (GetElementIndexInList(face2->id, faceIDs) == ELEMENT_OUT_LIST)    outputEdges.push_back(face->startEdge->next);
		if (GetElementIndexInList(face3->id, faceIDs) == ELEMENT_OUT_LIST)    outputEdges.push_back(face->startEdge->prev);
	}

	//printf("Edge Num: %d \n", outputEdges.size());
	//for (int i=0; i<outputEdges.size(); i++)
	//{
	//	printf("[%d %d] \n", outputEdges[i]->staVer->id, outputEdges[i]->endVer->id);
	//}

	return outputEdges;
}




//**************************************************************************************//
//                                   Draw Mesh
//**************************************************************************************//

// Draw 3D model with material
//void HEdgeMesh::DrawMesh(float mtlAmbient[], float mtlDiffuse[], float mtlSpecular[], float mtlEmission[])
//{
//	if ( faceList.size() == 0 )
//		return;
//
//	glEnable(GL_LIGHTING);
//
//	glPushAttrib( GL_LIGHTING_BIT );
//
//	GLfloat mtlShininess[] = { 76.8f };
//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mtlAmbient);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mtlDiffuse);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mtlSpecular);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mtlShininess);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  mtlEmission);
//
//	// Draw the triangles
//	for (int i=0; i<faceList.size(); i++)
//	{
//		glBegin(GL_TRIANGLES);
//		Vector3f faceNor = faceList[i]->normal;
//		glNormal3f(faceNor.x, faceNor.y, faceNor.z);
//		for (int j=0; j<faceList[i]->vertices.size(); j++)
//		{
//			Vector3f vertexPos = faceList[i]->vertices[j]->point;
//			glVertex3f(vertexPos.x, vertexPos.y, vertexPos.z);
//		}
//		glEnd();
//	}
//
//	glPopAttrib();
//}
//
//
//// Draw 3D model with transparency
//// TODO: there exists rendering issue with transparency
//void HEdgeMesh::DrawMesh(float mtlAmbient[], float mtlDiffuse[], float mtlSpecular[], float mtlEmission[], bool updateZOrder)
//{
//	if ( faceList.size() == 0 )
//		return;
//
//
//	glEnable(GL_LIGHTING);
//
//	glPushAttrib( GL_LIGHTING_BIT );
//
//	GLfloat mtlShininess[] = { 76.8f };
//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mtlAmbient);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mtlDiffuse);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mtlSpecular);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mtlShininess);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  mtlEmission);
//
//	// Compare model view matrix here
//	if ( updateZOrder )
//	{
//		SortZBufferTriangle();
//
//		//glEnable(GL_BLEND);
//		//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//	}
//
//	if ( sortedTriIDs.size() == 0 )
//		printf("Warning: sortedTriIDs should not be empty. \n");
//
//	for (int i=0; i<sortedTriIDs.size(); i++)
//	{
//		int faceID = sortedTriIDs[i];
//
//		glBegin(GL_TRIANGLES);
//		Vector3f faceNor = faceList[faceID]->normal;
//		glNormal3f(faceNor.x, faceNor.y, faceNor.z);
//		for (int j=0; j<faceList[faceID]->vertices.size(); j++)
//		{
//			Vector3f vertexPos = faceList[faceID]->vertices[j]->point;
//			glVertex3f(vertexPos.x, vertexPos.y, vertexPos.z);
//		}
//		glEnd();
//	}
//
//	glPopAttrib();
//
//	//if ( updateZOrder )
//	//{
//	//	glDisable(GL_BLEND);
//	//}
//}
//
//void HEdgeMesh::SortZBufferTriangle()
//{
//	double currMVMatrix[16] ;
//	glGetDoublev(GL_MODELVIEW_MATRIX, currMVMatrix);
//
//	//PrintMatrix( currMVMatrix );
//
//
//	if ( EqualMatrix( currMVMatrix, prevMVMatrix ) )
//		return;
//
//	vector<float> faceCenterZList;
//	for (int i=0; i<faceList.size(); i++)
//	{
//		// TODO: sort triangles according to the center may not be enough
//		Vector3f origCenter = Vector3f(0,0,0);
//		for (int j=0; j<faceList[i]->vertices.size(); j++)
//		{
//			origCenter += faceList[i]->vertices[j]->point;
//		}
//		origCenter /= 3.0f;
//
//		Vector3f currCenter;
//		MultiplyPoint(origCenter, currMVMatrix, currCenter);
//
//		faceCenterZList.push_back( currCenter.z );
//	}
//
//	sortedTriIDs = BubbleSort(faceCenterZList, true);
//
//	//printf("sortedTriIDs: ");
//	//for (int i=0; i<sortedTriIDs.size(); i++)
//	//{
//	//	printf(" %d ", sortedTriIDs[i] );
//	//}
//	//printf("\n");
//}

//
//void HEdgeMesh::DrawMeshWire(Vector3f color, float width)
//{
//	// Note: This epsilon depends on model scale and camera fovy
//	float epsilon = 1e-5;
//	float projMat[16];
//	glGetFloatv(GL_PROJECTION_MATRIX, projMat);
//
//	glDisable( GL_LIGHTING );
//	glColor3f(color.x, color.y, color.z);
//	glLineWidth( width );
//
//	// Move the mesh toward the eye a bit to avoid z-fighting issue
//	glMatrixMode(GL_PROJECTION);
//	glPushMatrix();
//	glLoadIdentity();
//	glTranslatef(0.0f,0.0f,-epsilon);
//	glMultMatrixf(projMat);
//
//	// Draw the mesh wireframe
//	for (int i=0; i<faceList.size(); i++)
//	{
//		glBegin(GL_LINE_LOOP);
//		for (int j=0; j<faceList[i]->vertices.size(); j++)
//		{
//			Vector3f vertexPos = faceList[i]->vertices[j]->point;
//			glVertex3f(vertexPos.x, vertexPos.y, vertexPos.z);
//		}
//		glEnd();
//	}
//
//	glPopMatrix();
//	glMatrixMode(GL_MODELVIEW);
//
//	glLineWidth( 1.0 );
//	glEnable(GL_LIGHTING);
//}
//
//void HEdgeMesh::DrawMeshVertices(Vector3f color, float size)
//{
//	if ( vertexList.size() == 0 )
//		return;
//
//	glDisable(GL_LIGHTING);
//	glColor3f(color.x, color.y, color.z);
//	glPointSize(size);
//
//	glBegin(GL_POINTS);
//	for (int i=0; i<vertexList.size(); i++)
//	{
//		Vector3f point = vertexList[i]->point;
//		glVertex3f(point.x, point.y, point.z);
//	}
//	glEnd();
//
//	glPointSize(1.0);
//	glEnable(GL_LIGHTING);
//}
//
//void HEdgeMesh::DrawMeshEdges(Vector3f color, float width)
//{
//	if ( edgeList.size() == 0 )
//		return;
//
//	glDisable(GL_LIGHTING);
//	glColor3f(color.x, color.y, color.z);
//	glLineWidth(width);
//
//	glBegin(GL_LINES);
//	for (int i=0; i<edgeList.size(); i++)
//	{
//		Vector3f staPt = edgeList[i]->staVer->point;
//		Vector3f endPt = edgeList[i]->endVer->point;
//		glVertex3f(staPt.x, staPt.y, staPt.z);
//		glVertex3f(endPt.x, endPt.y, endPt.z);
//	}
//	glEnd();
//
//	glLineWidth(1.0);
//	glEnable(GL_LIGHTING);
//}




//**************************************************************************************//
//                                    Draw Debug
//**************************************************************************************//
//
//void HEdgeMesh::DrawNeighborEdges(int edgeID)
//{
//	if ( edgeList.size() == 0 )
//		return;
//
//	glDisable(GL_LIGHTING);
//	glLineWidth(5.0);
//
//	// Draw current edge
//	DrawEdge(edgeList[edgeID], Vector3f(0.9,0.9,0.1));
//
//	// Draw next edge
//	DrawEdge(edgeList[edgeID]->next, Vector3f(0.9,0.1,0.1));
//
//	// Draw previous edge
//	DrawEdge(edgeList[edgeID]->prev, Vector3f(0.1,0.1,0.9));
//
//	glLineWidth(1.0);
//	glEnable(GL_LIGHTING);
//}
//
//void HEdgeMesh::DrawEdge(Edge *edge, Vector3f color)
//{
//	Vector3f staPt = edge->staVer->point;
//	Vector3f endPt = edge->endVer->point;
//
//	glColor3f(color.x, color.y, color.z);
//	glBegin(GL_LINES);
//	glVertex3f(staPt.x, staPt.y, staPt.z);
//	glVertex3f(endPt.x, endPt.y, endPt.z);
//	glEnd();
//}