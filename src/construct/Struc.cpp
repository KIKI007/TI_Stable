///////////////////////////////////////////////////////////////
//
// Struc.cpp
//
//   3D Geometric Structure Class
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 21/Nov/2017
//
///////////////////////////////////////////////////////////////

#include "Controls.h"
#include "HelpDefine.h"
#include "HelpStruct.h"
#include "HelpFunc.h"
#include "math3D.h"
#include "Surface.h"
#include "Cross.h"
#include "CrossMesh.h"
#include "Part.h"
#include "PolyMesh.h"
#include "Struc.h"


//**************************************************************************************//
//                                   Initialization
//**************************************************************************************//

Struc::Struc()
{
	identityd(worldAxesMat);

	surface = NULL;

	crossMesh = NULL;
}

Struc::~Struc()
{
	Clear();
}

void Struc::Clear()
{
	if (surface != NULL)
	{
		delete surface;
		surface = NULL;
	}

	if (crossMesh != NULL)
	{
		delete crossMesh;
		crossMesh = NULL;
	}

	for (int i = 0; i < partList.size(); i++)
	{
		delete partList[i];
	}
	partList.clear();
}




//**************************************************************************************//
//                             Struc and Axes Transform
//**************************************************************************************//

void Struc::InitWorldMatrix(Vector3f position, Vector3f rotAxis, float rotAngle, float scale)
{
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(position.x, position.y, position.z);
	glRotatef(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
	glScalef(scale, scale, scale);
	glGetDoublev(GL_MODELVIEW_MATRIX, worldMatrix);
	glPopMatrix();
}

void Struc::InitWorldAxesMatrix(Vector3f position, Vector3f rotAxis, float rotAngle)
{
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(position.x, position.y, position.z);
	glRotatef(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
	glGetDoublev(GL_MODELVIEW_MATRIX, worldAxesMat);
	glPopMatrix();
}

void Struc::ResetModel()
{
	InitWorldMatrix(INIT_WORLD_POSITION, INIT_WORLD_ROT_AXIS, INIT_WORLD_ROT_ANGLE, INIT_WORLD_SCALE);
	InitWorldAxesMatrix(INIT_WORLD_AXES_POSITION, INIT_WORLD_AXES_ROT_AXIS, INIT_WORLD_AXES_ROT_ANGLE);
}



//**************************************************************************************//
//                                  Compute Structure (Object Model)
//**************************************************************************************//

/*void Struc::LoadModel(char * objFileName)
{
	if (model != NULL)
	{
		delete model;
		model = NULL;
	}

	model = new Model();

	model->ReadOBJFile_Poly(objFileName);
	model->ComputeProperties();

	model->BuildHalfEdgeMesh();
}

void Struc::CreateStructure_Obj(float tiltTheta)
{
	if (model == NULL)
		return;

	if ( crossMesh != NULL )
	{
		delete crossMesh;
		crossMesh = NULL;
	}

	for (int i = 0; i < partList.size(); i++)
	{
		delete partList[i];
	}
	partList.clear();

	crossMesh = new CrossMesh();

	// Specify vertices
	for (int i = 0; i < model->faceList.size(); i++)
	{
		Face_ *face = model->faceList[i];

		Cross *cross = new Cross();

		cross->isValid = true;
		cross->id = i;

		for (int j = 0; j < face->vertices.size(); j++)
		{
			cross->poly3D.vers.push_back(face->vertices[j]->pos);

			float angle;
			if (j % 2 == 0)    angle =  tiltTheta;
			else               angle = -tiltTheta;
		
			cross->tiltAngles.push_back(angle);
		}

		//if ( i == 1 )
		//{
		//	for (int j = 0; j < cross->tiltAngles.size(); j++)
		//	{
		//		cross->tiltAngles[j] = -cross->tiltAngles[j];
		//	}
		//}

		//cross->poly3D.ComputeCenter();
		cross->poly3D.ComputeNormal();
		cross->poly3D.ComputeMidPoints();

		crossMesh->crossList.push_back(cross);
	}

	if (crossMesh->crossList.size() > 1)
	{
		// Specify neighbors 
		for (int i = 0; i < model->faceList.size(); i++)
		{
			Face_ *face = model->faceList[i];
			Cross *cross = crossMesh->crossList[i];

			for (int j = 0; j < face->edges.size(); j++)
			{
				//Edge_ *edge = face->edges[j];

				if (face->edges[j]->twin == NULL)
				{
					cross->neighbors.push_back(NULL);
					continue;
				}
				

				int neiborFaceID = face->edges[j]->twin->face->id;

				if (neiborFaceID < 0 || neiborFaceID >= crossMesh->crossList.size())
				{
					printf("Warning: neiborFaceID = %d is invalid \n", neiborFaceID);
					continue;
				}

				//printf("i=%d   j=%d   neiborFaceID: %d \n", i, j, neiborFaceID);

				Cross* neiborCross = crossMesh->crossList[neiborFaceID];

				cross->neighbors.push_back(neiborCross);
			}
		}

		// TODO: remove this hack later
		if (crossMesh->crossList.size() == 2 )
		{
			Cross * cross = crossMesh->crossList[1];
			for (int j = 0; j < cross->tiltAngles.size(); j++)
			{
				cross->tiltAngles[j] = -cross->tiltAngles[j];
			}
		}
		else
		{
			SpecifyTiltAngles(tiltTheta);
		}
	}

	for (int i = 0; i < crossMesh->crossList.size(); i++)
	{
		crossMesh->crossList[i]->ComputeTiltNormals();
	}

	for (int i = 0; i < crossMesh->crossList.size(); i++)
	{
		Cross *cross = crossMesh->crossList[i];

		Part *part = new Part(cross->oriPointList);

		part->ComputePartGeometry();

		partList.push_back(part);
	}

	//printf("step_4 \n");

	//for (int i = 0; i < partList.size(); i++)
	//{
	//	printf("i=%d  verNum: %d \n", i, partList[i]->polyMesh->verList.size());
	//}
}


void Struc::SpecifyTiltAngles(float tiltTheta)
{
	vector<Cross*> bfsQueue;

	// Find a root for Breadth-First Search Traversal 
	Cross *root = crossMesh->crossList[0];

	root->isVisited = true;

	bfsQueue.push_back(root);

	// Start Breadth-First Search Traversal
	while (true)
	//for (int k = 0; k < 1; k++)
	{
		Cross *currVisit = bfsQueue[0];
		bfsQueue.erase(bfsQueue.begin());

		//printf("currVisit: %d \n", currVisit->id);

		int crossID = currVisit->id;
		Face_ *face = model->faceList[crossID];

		bool isAllVisited = true;
		for (int i = 0; i < crossMesh->crossList.size(); i++)
		{
			Cross *cross = crossMesh->crossList[i];

			if (cross->isVisited == false)
			{
				isAllVisited = false;
				break;
			}
		}


		if ( isAllVisited )
		{
			break;
		}


		for (int i = 0; i < face->edges.size(); i++)
		{
			Cross *nextVisit = currVisit->neighbors[i];

			bfsQueue.push_back(nextVisit);

			//printf("k=%d  i=%d faceID: %d neiborID: %d  visited: %d \n", k, i, face->id, face->edges[i]->twin->face->id, nextVisit->isVisited);

			if (nextVisit->isVisited)
				continue;

			vector<int> faceEdgeIDs;
			for (int j = 0; j < face->edges[i]->twin->face->edges.size(); j++)
			{
				faceEdgeIDs.push_back(face->edges[i]->twin->face->edges[j]->id);
			}

			int edgeIndex = GetElementIndexInList(face->edges[i]->twin->id, faceEdgeIDs);

			//printf("before: i=%d  index: %d  %.2f %.2f \n", i, edgeIndex, currVisit->tiltAngles[i], nextVisit->tiltAngles[edgeIndex]);

			//if (face->edges[i]->twin->face->id != 4)
			{
				if (currVisit->tiltAngles[i] * nextVisit->tiltAngles[edgeIndex] > 0)
				{
					for (int j = 0; j < nextVisit->tiltAngles.size(); j++)
					{
						nextVisit->tiltAngles[j] = -nextVisit->tiltAngles[j];
					}
				}
			}
			//else
			//{
			//	printf("==========================================\n");
			//	printf("nextVist: %d \n", nextVisit->id);
			//}

			nextVisit->isVisited = true;		
		}
	}

	for (int i = 0; i < crossMesh->crossList.size(); i++)
	{
		Cross *cross = crossMesh->crossList[i];

		//if ( i == 4 )
		//{
		//	for (int j = 0; j < cross->tiltAngles.size(); j++)
		//	{
		//		cross->tiltAngles[j] = -cross->tiltAngles[j];
		//	}
		//}

		printf("cross: %d ", cross->id);
		for (int j = 0; j < cross->tiltAngles.size(); j++)
		{
			printf(" %5.2f ", cross->tiltAngles[j]);
		}
		printf(" \n");
	}
}*/



//**************************************************************************************//
//                                  Initialize Structure
//**************************************************************************************//

void Struc::InitCrossMesh2D(int crossType, int crossRingNum)
{
	if (crossMesh != NULL)
	{
		delete crossMesh;
		crossMesh = NULL;
	}

	crossMesh = new CrossMesh();
	crossMesh->CreateCrossMesh2D(crossType, crossRingNum);
}


void Struc::LoadSurface(char * objFileName)
{
	if (surface != NULL)
	{
		delete surface;
		surface = NULL;
	}

	surface = new Surface();

	surface->LoadModel(objFileName);
	surface->ComputeBoundingBox();
	surface->ComputeFootMatrix();
}

void Struc::BindTexture(float viewSize)
{
	if (surface == NULL)
		return;

	//float viewW = 4.0;
	//float viewH = 4.0;

	float viewW = viewSize;
	float viewH = viewSize;

	GLint iWidth = 250;
	GLint iHeight = 250;

//	GLubyte *pBytes = crossMesh->RenderOffline2D(viewW, viewH, iWidth, iHeight);

//	surface->BindTextureImage(pBytes, iWidth, iHeight);
}




//**************************************************************************************//
//                                  Create Structure 
//**************************************************************************************//

void Struc::CreateStructure_Arch(float viewSize, float tiltTheta)
{
	if (surface == NULL || crossMesh == NULL )
		return;

	for (int i = 0; i < partList.size(); i++)
	{
		delete partList[i];
	}
	partList.clear();

	crossMesh->ResetCrossMesh();


	////////////////////////////////////////////////////////////////
	// 1. Specify cross tilt angles

	for (int i = 0; i < crossMesh->crossList.size(); i++)
	{
		Cross *cross = crossMesh->crossList[i];

		for (int j = 0; j < cross->tiltAngles.size(); j++)
		{
			if (cross->tiltAngles[j] > 0)
				cross->tiltAngles[j] = tiltTheta;
			else
				cross->tiltAngles[j] = -tiltTheta;
		}
	}


	////////////////////////////////////////////////////////////////
	// 2. Validate each cross and compute corresponding 3D polygon

	double footInverMat[16];
	memcpy(footInverMat, surface->footMatrix, sizeof(double) * 16);
	if (invertMat(footInverMat, 4) == 1)  printf("Inverse Matrix Error \n");



	ComputeBaseMesh(viewSize, footInverMat);


	////////////////////////////////////////////////////////////////
	// 3. Remove dangling cross (repeat it 3 times)

	for (int i = 0; i < 3; i++)
	{
		RemoveDanglingCross();
	}


	////////////////////////////////////////////////////////////////
	// 4. Construct a part (i.e., convex polyhedron) from each 3D polygon

	for (int i = 0; i < crossMesh->crossList.size(); i++)
	{
		Cross *cross = crossMesh->crossList[i];

		if (cross->isValid)
		{
			cross->ComputeTiltNormals();

			Part *part = new Part(cross->oriPointList);

			part->ComputePartGeometry();

			//part->ComputePartMobility();

			part->partID = partList.size();

			part->assoCrossID = cross->crossID;  // Link the cross with the part
			cross->assoPartID = part->partID;

			partList.push_back(part);
		}
	}
}

void Struc::ComputeBaseMesh(float viewSize, double footInverMat[16])
{
	for (int i = 0; i < crossMesh->crossList.size(); i++)
	{
		Cross *cross = crossMesh->crossList[i];

		// Validate each 2D cross
		cross->isValid = true;
		vector<Vector3f> vers3D;
		for (int j = 0; j < cross->poly2D.vers.size(); j++)
		{
			Vector3f ver = cross->poly2D.vers[j];

			Vector3f texCoord = GetTextureCoord(ver, viewSize);

			printf("ver: [%.3f %.3f %.3f]  tex: [%.3f %.3f %.3f] \n", ver.x, ver.y, ver.z, texCoord.x, texCoord.y, texCoord.z);
			MultiplyPoint(texCoord, footInverMat, texCoord);

			Vector3f ver3D;
			bool isSuccess = surface->ComputeSurfaceCoord(texCoord, ver3D);

			vers3D.push_back(ver3D);

			if (isSuccess == false)
			{
				cross->isValid = false;
				break;
			}

			//printf("new ver: [%.3f %.3f %.3f] \n", ver3D.x, ver3D.y, ver3D.z);
		}

		// Compute 3D polygon for each valid cross
		if (cross->isValid)
		{
			cross->poly3D.vers = vers3D;
			cross->poly3D.ComputeCenter();
			cross->poly3D.ComputeNormal();
			cross->poly3D.ComputeMidPoints();
		}
	}
}

Vector3f Struc::GetTextureCoord(Vector3f point, float viewSize)
{
	Vector3f texCoord;

	texCoord.x = (point.x + 0.5f*viewSize) / viewSize;
	texCoord.y = (point.y + 0.5f*viewSize) / viewSize;
	texCoord.z = 0;

	return texCoord;
}

void Struc::RemoveDanglingCross()
{
	for (int i = 0; i < crossMesh->crossList.size(); i++)
	{
		Cross *cross = crossMesh->crossList[i];

		int totalNeiborNum = cross->neighbors.size();

		int validNeiborNum = 0;
		for (int j = 0; j < cross->neighbors.size(); j++)
		{
			if (cross->neighbors[j]->isValid)
			{
				validNeiborNum++;
			}
		}

		// A valid cross: more or equal to half of its neighbors are valid 
		int thresNeiborNum = totalNeiborNum / 2;

		//printf("valid: %d   thres: %d \n", validNeiborNum, thresNeiborNum);

		if (validNeiborNum == 0 || validNeiborNum < thresNeiborNum)
		{
			cross->isValid = false;
		}
	}
}




//**************************************************************************************//
//                             Build Parts Graph (Arch Model)
//**************************************************************************************//

void Struc::BuildPartsGraph_Arch()
{
	for (int i = 0; i < partList.size(); i++)
	{
		Part *part = partList[i];

		int partID = part->partID;

		int assoCrossID = GetAssoCrossID(partID);

		//printf("partID: %d   assoCrossID: %d \n", partID, assoCrossID);

		Cross *cross = crossMesh->crossList[assoCrossID];

		for (int j = 0; j < cross->neighbors.size(); j++)
		{
			Cross *neiborCross = cross->neighbors[j];

			int assoPartID = GetAssoPartID(neiborCross->crossID);

			//printf("j=%d crossID: %d  assoPartID: %d \n", j, neiborCross->crossID, assoPartID);

			if (neiborCross->isValid == false || assoPartID == NONE_PART )
			{
				part->initNeighbors.push_back(NULL);
			}
			else
			{
				part->initNeighbors.push_back(partList[assoPartID]);
			}
		}
	}

	for (int i = 0; i < partList.size(); i++)
	{
		Part *part = partList[i];

		part->currNeighbors = part->initNeighbors;
	}

	//printf("\n");
	//for (int i = 0; i < partList.size(); i++)
	//{
	//	Part *part = partList[i];

	//	part->PrintPart();
	//}
}


void Struc::IdentifyBoundaryParts()
{
	for (int i = 0; i < partList.size(); i++)
	{
		Part *part = partList[i];

		int emptyNeiborNum = 0;
		for (int j = 0; j < part->initNeighbors.size(); j++)
		{
			if (part->initNeighbors[j] == NULL)
			{
				emptyNeiborNum++;
			}
		}

		if ( emptyNeiborNum > 0 )
		{
			part->atBoundary = true;
		}
	}

	int boundaryPartNum = 0;
	for (int i = 0; i < partList.size(); i++)
	{
		Part *part = partList[i];
		if ( part->atBoundary )
		{
			boundaryPartNum++;
		}
	}

	printf("Total part: %d   boundary part: %d \n", partList.size(), boundaryPartNum);
}

int Struc::GetAssoCrossID(int partID)
{
	for (int i = 0; i < crossMesh->crossList.size(); i++)
	{
		Cross *cross = crossMesh->crossList[i];

		if ( cross->assoPartID == partID )
		{
			return cross->crossID;
		}
	}

	return NONE_CROSS;
}

int Struc::GetAssoPartID(int crossID)
{
	for (int i = 0; i < partList.size(); i++)
	{
		Part *part = partList[i];

		if (part->assoCrossID == crossID )
		{
			return part->partID;
		}
	}

	return NONE_PART;
}




//**************************************************************************************//
//                                   Save OBJ Models
//**************************************************************************************//

void Struc::WriteStructure(char *folderPath)
{
	////////////////////////////////////////////////////////
	// 1. Save all part OBJ models

	for (int i = 0; i < partList.size(); i++)
	{
		char objFileName[FILE_NAME_LENGTH];
		sprintf(objFileName, "%s\\Part_%02d.obj", folderPath, i);
		partList[i]->WriteOBJModel(objFileName);
	}


	////////////////////////////////////////////////////////
	// 2. Save part graph
	WritePartGraph(folderPath);
}

void Struc::WritePartGraph(char *folderPath)
{
	char graphFileName[512];
	sprintf(graphFileName, "%s\\PartGraph.txt", folderPath);

	FILE *fp;
	if ((fp = fopen(graphFileName, "w+")) == NULL)
	{
		printf("Error: file not exists! \n");
		return;
	}
	else
	{
		////////////////////////////////////////////////////////
		// Write piece OBJ model name and number

		//fprintf(fp, "# Part ID starts from 0 \n");
		fprintf(fp, "Part Num %d\n", partList.size());

		for (int i = 0; i < partList.size(); i++)
		{
			Part *part = partList[i];
			//char partFileName[FILE_NAME_LENGTH];
			//sprintf(partFileName, "%2d %d", i, part);

			fprintf(fp, "%2d    supp: %d    neighbor %d: [", part->partID, part->atBoundary, part->initNeighbors.size());

			for (int j = 0; j < part->initNeighbors.size(); j++)
			{
				Part *neibor = part->initNeighbors[j];

				if ( neibor== NULL )    fprintf(fp, " %2d ", -1);
				else                    fprintf(fp, " %2d ", neibor->partID);
			}

			fprintf(fp, " ] \n");
		}

		fprintf(fp, "\n");

		fclose(fp);
	}
}




//**************************************************************************************//
//                                   Draw Input Model
//**************************************************************************************//

void Struc::DrawSurface(int showSurfTex)
{
	if (surface == NULL)
		return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixd(worldMatrix);

	surface->DrawSurface(showSurfTex);

	glPopMatrix();
}

void Struc::DrawSurfaceWire()
{
	if (surface == NULL)
		return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixd(worldMatrix);

	surface->DrawSurfaceWire();

	glPopMatrix();
}




//**************************************************************************************//
//                                    Draw Base Mesh
//**************************************************************************************//
//
//void Struc::DrawCrossMesh2D()
//{
//	if (crossMesh == NULL)
//		return;
//
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldMatrix);
//
//	crossMesh->DrawCrossMesh2D();
//
//
//	glPopMatrix();
//}
//
//void Struc::DrawCrossMesh3D()
//{
//	if (crossMesh == NULL)
//		return;
//
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldMatrix);
//
//	crossMesh->DrawCrossMesh3D();
//
//	glPopMatrix();
//}
//
//void Struc::DrawCrossMesh3D_Dual()
//{
//	if (crossMesh == NULL)
//		return;
//
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldMatrix);
//
//	crossMesh->DrawCrossMesh3D_Dual();
//
//	glPopMatrix();
//}
//
//void Struc::DrawCrossMesh3D_Inner()
//{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldMatrix);
//
//	for (int i = 0; i < partList.size(); i++)
//	{
//		partList[i]->DrawInnerPolygon();
//	}
//
//	glPopMatrix();
//}
//
//void Struc::DrawOriPoints()
//{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldMatrix);
//
//	for (int i = 0; i < partList.size(); i++)
//	{
//		partList[i]->DrawOriPoints();
//	}
//
//	glPopMatrix();
//}
//
//
//
//
////**************************************************************************************//
////                                 Draw TI Construction
////**************************************************************************************//
//
//void Struc::DrawGeomFaces()
//{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldMatrix);
//
//	//int i = 0;
//	for (int i = 0; i < partList.size(); i++)
//	{
//		partList[i]->DrawGeomFaces();
//	}
//
//	glPopMatrix();
//}
//
//void Struc::DrawGeomEdges()
//{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldMatrix);
//
//	for (int i = 0; i < partList.size(); i++)
//	{
//		partList[i]->DrawGeomEdges();
//	}
//
//	glPopMatrix();
//}
//
//void Struc::DrawGeomVertices()
//{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldMatrix);
//
//	for (int i = 0; i < partList.size(); i++)
//	{
//		partList[i]->DrawGeomVertices();
//	}
//
//	glPopMatrix();
//}
//
//
//
//
////**************************************************************************************//
////                                   Draw TI Structure
////**************************************************************************************//
//
//void Struc::DrawStructure(int mode)
//{
//	if (partList.size() == 0)
//		return;
//
//	//int i = 0;
//	for (int i = 0; i < partList.size(); i++)
//	{
//		Part *part = partList[i];
//
//		if ( part->isRemove )
//			continue;
//
//		if (mode == GL_SELECT)
//		{
//			glLoadName(i);
//			glPushName(i);
//		}
//
//		glMatrixMode(GL_MODELVIEW);
//		glPushMatrix();
//		glLoadMatrixd(worldMatrix);
//
//		part->DrawPart();
//
//		glPopMatrix();
//
//		if (mode == GL_SELECT)
//		{
//			glPopName();
//		}
//	}
//}
//
//void Struc::DrawStructureWire()
//{
//	if (partList.size() == 0)
//		return;
//
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldMatrix);
//
//	//int i = 0;
//	for (int i = 0; i < partList.size(); i++)
//	{
//		Part *part = partList[i];
//
//		if (part->isRemove)
//			continue;
//
//		part->DrawPartWire(2.0, Vector3f(0.2, 0.2, 0.2));
//	}
//
//	glPopMatrix();
//}
//
//void Struc::DrawStructureGraph()
//{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldMatrix);
//
//	glDisable(GL_LIGHTING);
//	glLineWidth(3.0);
//	glColor3f(0.9, 0.2, 0.9);
//
//	for (int i = 0; i < partList.size(); i++)
//	{
//		Part *part = partList[i];
//
//		if ( part->isRemove )
//			continue;
//
//		Vector3f partCen = part->polyMesh->centroid;
//
//		for (int j = 0; j < part->currNeighbors.size(); j++)
//		{
//			Part *neibor = part->currNeighbors[j];
//
//			if (neibor == NULL)
//				continue;
//
//			Vector3f neiborCen = neibor->polyMesh->centroid;
//
//			glBegin(GL_LINES);
//			glVertex3f(partCen.x, partCen.y, partCen.z);
//			glVertex3f(neiborCen.x, neiborCen.y, neiborCen.z);
//			glEnd();
//		}
//	}
//
//	glEnable(GL_LIGHTING);
//	glLineWidth(1.0);
//
//	glPopMatrix();
//}
//
//void Struc::DrawPickPart(int pickPartID)
//{
//	if (pickPartID < 0 || pickPartID >= partList.size())
//		return;
//
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldMatrix);
//
//	Part *pickPart = partList[pickPartID];
//	pickPart->DrawPartWire(5.0, Vector3f(0.2, 0.9, 0.2));
//
//	glPopMatrix();
//}
//
//
//
//
////**************************************************************************************//
////                              Draw World Axes, Ground
////**************************************************************************************//
//
//void Struc::DrawWorldAxes(int winW, int winH, float currFovy)
//{
//	glDisable(GL_LIGHTING);
//	glLineWidth(2.0);
//
//	int viewW = 0.3 * winW;
//	int viewH = 0.3 * winH;
//	glViewport(0, 0, viewW, viewH);
//
//	glMatrixMode(GL_PROJECTION);
//	glPushMatrix();
//	glLoadIdentity();
//	gluPerspective(currFovy, viewW / ((double)viewH), 1.0, 5000.0);
//
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldAxesMat);
//	glBegin(GL_LINES);
//	// light red x axis arrow
//	glColor3f(1.f, 0.5f, .5f);
//	glVertex3f(0.0f, 0.0f, 0.0f);
//	glVertex3f(1.0f, 0.0f, 0.0f);
//	// light green y axis arrow
//	glColor3f(.5f, 1.f, 0.5f);
//	glVertex3f(0.0f, 0.0f, 0.0f);
//	glVertex3f(0.0f, 1.0f, 0.0f);
//	// light blue z axis arrow
//	glColor3f(.5f, .5f, 1.f);
//	glVertex3f(0.0f, 0.0f, 0.0f);
//	glVertex3f(0.0f, 0.0f, 1.0f);
//	glEnd();
//	glBegin(GL_LINES);
//	// x letter & arrowhead
//	glColor3f(1.f, 0.5f, .5f);
//	glVertex3f(1.1f, 0.1f, 0.0f);
//	glVertex3f(1.3f, -0.1f, 0.0f);
//	glVertex3f(1.3f, 0.1f, 0.0f);
//	glVertex3f(1.1f, -0.1f, 0.0f);
//	glVertex3f(1.0f, 0.0f, 0.0f);
//	glVertex3f(0.9f, 0.1f, 0.0f);
//	glVertex3f(1.0f, 0.0f, 0.0f);
//	glVertex3f(0.9f, -0.1f, 0.0f);
//	// y letter & arrowhead
//	glColor3f(.5f, 1.f, 0.5f);
//	glVertex3f(-0.1f, 1.3f, 0.0f);
//	glVertex3f(0.f, 1.2f, 0.0f);
//	glVertex3f(0.1f, 1.3f, 0.0f);
//	glVertex3f(0.f, 1.2f, 0.0f);
//	glVertex3f(0.f, 1.2f, 0.0f);
//	glVertex3f(0.f, 1.1f, 0.0f);
//	glVertex3f(0.0f, 1.0f, 0.0f);
//	glVertex3f(0.1f, 0.9f, 0.0f);
//	glVertex3f(0.0f, 1.0f, 0.0f);
//	glVertex3f(-0.1f, 0.9f, 0.0f);
//	// z letter & arrowhead
//	glColor3f(.5f, .5f, 1.f);
//	glVertex3f(0.0f, -0.1f, 1.3f);
//	glVertex3f(0.0f, 0.1f, 1.3f);
//	glVertex3f(0.0f, 0.1f, 1.3f);
//	glVertex3f(0.0f, -0.1f, 1.1f);
//	glVertex3f(0.0f, -0.1f, 1.1f);
//	glVertex3f(0.0f, 0.1f, 1.1f);
//	glVertex3f(0.0f, 0.0f, 1.0f);
//	glVertex3f(0.0f, 0.1f, 0.9f);
//	glVertex3f(0.0f, 0.0f, 1.0f);
//	glVertex3f(0.0f, -0.1f, 0.9f);
//	glEnd();
//	glMatrixMode(GL_MODELVIEW);
//	glPopMatrix();
//
//	glMatrixMode(GL_PROJECTION);
//	glPopMatrix();
//
//	glEnable(GL_LIGHTING);
//	glLineWidth(1.0);
//	glViewport(0, 25, winW, winH);
//}
//
//void Struc::RotateWorldAxes(Vector3f rotAxis, float rotAngle)
//{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadIdentity();
//	glTranslatef(worldAxesMat[12], worldAxesMat[13], worldAxesMat[14]);
//	glRotatef(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
//	glTranslatef(-worldAxesMat[12], -worldAxesMat[13], -worldAxesMat[14]);
//	glMultMatrixd(worldAxesMat);
//	glGetDoublev(GL_MODELVIEW_MATRIX, worldAxesMat);
//	glPopMatrix();
//}
//
//
//
//
////**************************************************************************************//
////                                      Draw Ground
////**************************************************************************************//
//
////void Struc::DrawGround()
////{
////	float dense = 10;
////	float dimen = 6.0;
////
////	const float offset = 0.0001;
////
////	glDisable(GL_LIGHTING);
////	glLineStipple(1, 0x3333);
////	glLineWidth(1.5);
////	glEnable(GL_LINE_STIPPLE);
////
////	glMatrixMode(GL_MODELVIEW);
////	glPushMatrix();
////	glLoadMatrixd(worldMatrix);
////
////	// Draw the ground wireframe grid
////	glColor3f(0.5f, 0.5f, 0.5f);
////	//DrawGridLines(dense, dimen, 0.0001);
////	DrawGridLines(dense, dimen, groundY + offset);
////	// Draw the solid ground plane
////	glColor3f(0.9f, 0.9f, 0.9f);
////	glPushAttrib(GL_POLYGON_BIT);
////	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
////	//DrawGrid(dense, dimen, 0.00);
////	DrawGrid(dense, dimen, groundY);
////	glPopAttrib();
////
////	glPopMatrix();
////
////	glDisable(GL_LINE_STIPPLE);
////	glEnable(GL_LIGHTING);
////	glLineWidth(1.0);
////}
//
//void Struc::DrawGround()
//{
//	const float dense = 10;
//	const float dimen = 4.0;
//	const float offset = 0.0001;
//
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadMatrixd(worldMatrix);
//
//	DrawGrid(dense, dimen, groundY);
//	DrawGridLines(dense, dimen, groundY + offset);
//
//	glPopMatrix();
//}
//
//void Struc::DrawGridLines(float dense, float dimen, float gridY)
//{
//	glDisable(GL_LIGHTING);
//	glLineStipple(1, 0x3333);
//	glLineWidth(1.5);
//	glEnable(GL_LINE_STIPPLE);
//
//	glColor3f(0.5f, 0.5f, 0.5f);
//
//	glBegin(GL_LINES);
//	for (int i = -dense; i <= dense; i++)
//	{
//		double loc = (i / (double)dense)  * dimen;
//		glVertex3d(loc, gridY, -dimen);
//		glVertex3d(loc, gridY, dimen);
//		glVertex3d(-dimen, gridY, loc);
//		glVertex3d(dimen, gridY, loc);
//	}
//	glEnd();
//
//	glDisable(GL_LINE_STIPPLE);
//	glEnable(GL_LIGHTING);
//	glLineWidth(1.0);
//}
//
//void Struc::DrawGrid(float dense, float dimen, float gridY)
//{
//	Vector3f color = Vector3f(0.5,0.5,0.5);
//
//	float mtlAmbient[4] = { color.x, color.y, color.z, 0.4 };
//	float mtlDiffuse[4] = { color.x, color.y, color.z, 0.4 };
//	float mtlSpecular[4] = { color.x, color.y, color.z, 0.4 };
//	float mtlEmission[4] = { color.x, color.y, color.z, 0.4 };
//
//	glPushAttrib(GL_LIGHTING_BIT);
//
//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mtlAmbient);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mtlDiffuse);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mtlSpecular);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mtlEmission);
//
//	glBegin(GL_QUADS);
//	for (int i = -dense; i <= dense; i++)
//	{
//		for (int j = -dense; j <= dense; j++)
//		{
//			float x0 = (i / (float)dense) * dimen;
//			float x1 = ((i + 1) / (float)dense) * dimen;
//			float y0 = (j / (float)dense) * dimen;
//			float y1 = ((j + 1) / (float)dense) * dimen;
//
//			glVertex3f(x0, gridY, y0);
//			glVertex3f(x1, gridY, y0);
//			glVertex3f(x1, gridY, y1);
//			glVertex3f(x0, gridY, y1);
//		}
//	}
//	glEnd();
//
//	glPopAttrib();
//}
//
//
//
//
////**************************************************************************************//
////                                  Transform World
////**************************************************************************************//
//
//void Struc::TranslateObject(Vector3f transVec)
//{
//	worldMatrix[12] += transVec[0];
//	worldMatrix[13] += transVec[1];
//	worldMatrix[14] += transVec[2];
//}
//
//void Struc::RotateObject(Vector3f rotAxis, float rotAngle)
//{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadIdentity();
//	glTranslatef(worldMatrix[12], worldMatrix[13], worldMatrix[14]);
//	glRotatef(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
//	glTranslatef(-worldMatrix[12], -worldMatrix[13], -worldMatrix[14]);
//	glMultMatrixd(worldMatrix);
//	glGetDoublev(GL_MODELVIEW_MATRIX, worldMatrix);
//	glPopMatrix();
//}
//
//void Struc::ScaleObject(Vector3f scaleVec)
//{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadIdentity();
//	glTranslatef(worldMatrix[12], worldMatrix[13], worldMatrix[14]);
//	glScalef(scaleVec.x, scaleVec.y, scaleVec.z);
//	glTranslatef(-worldMatrix[12], -worldMatrix[13], -worldMatrix[14]);
//	glMultMatrixd(worldMatrix);
//	glGetDoublev(GL_MODELVIEW_MATRIX, worldMatrix);
//	glPopMatrix();
//}


