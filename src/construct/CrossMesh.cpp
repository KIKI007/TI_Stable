///////////////////////////////////////////////////////////////
//
// CrossMesh.cpp
//
//   Cross Mesh
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 17/Nov/2017
//
///////////////////////////////////////////////////////////////


#include "Controls.h"
#include "HelpDefine.h"
#include "HelpFunc.h"
#include "Cross.h"
#include "CrossMesh.h"


#define CROSS_OUT_LIST       -1


//**************************************************************************************//
//                                   Initialization
//**************************************************************************************//

CrossMesh::CrossMesh()
{

}

CrossMesh::~CrossMesh()
{
	for (int i = 0; i < crossList.size(); i++)
	{
		delete crossList[i];
	}
	crossList.clear();
}

void CrossMesh::ResetCrossMesh()
{
	for (int i = 0; i < crossList.size(); i++)
	{
		Cross *cross = crossList[i];

		cross->assoPartID = NONE_PART;
	}
}




//**************************************************************************************//
//                                  Construct Cross Mesh
//**************************************************************************************//

void CrossMesh::CreateCrossMesh2D(int crossType, int maxDist)
{
	Cross *cross = new Cross(crossType, CROSS_L);

	cross->crossID = crossList.size();

	crossList.push_back(cross);

	vector<Cross*> bfsQueue;

	// Find a root for Breadth-First Search Traversal 
	Cross *root = crossList[0];
	root->dist = 0;
	bfsQueue.push_back(root);

	// Start Breadth-First Search Traversal
	while (true)
	{
		Cross *currVisit = bfsQueue[0];
		bfsQueue.erase(bfsQueue.begin());

		if (currVisit->dist >= maxDist)
		{
			break;
		}

		currVisit->ComputeNeighbors();

		for (int i = 0; i < currVisit->neighbors.size(); i++)
		{
			Cross *nextVisit = currVisit->neighbors[i];

			nextVisit->dist = currVisit->dist + 1;

			int index = GetCrossIndexInList(nextVisit, crossList);
			if (index == CROSS_OUT_LIST)
			{
				nextVisit->crossID = crossList.size();

				crossList.push_back(nextVisit);
				bfsQueue.push_back(nextVisit);
			}

			else
			{
				delete nextVisit;

				currVisit->neighbors[i] = crossList[index];
			}
		}

		//if (crossList.size() >= crossNum)
		//{
		//	break;
		//}
	}
}

int CrossMesh::GetCrossIndexInList(Cross *tagtCross, vector<Cross*> crossList)
{
	for (int i = 0; i < crossList.size(); i++)
	{
		if (crossList[i]->IsEqual(tagtCross) == true)
		{
			return i;
		}
	}

	return CROSS_OUT_LIST;
}




//**************************************************************************************//
//                                Render Cross Mesh Offline
//**************************************************************************************//
//
//GLubyte* CrossMesh::RenderOffline2D(float viewW, float viewH, GLint iWidth, GLint iHeight)
//{
//	GLubyte *imgBytes = new GLubyte[iWidth*iHeight * 4];
//
//	for (int i = 0; i < iWidth*iHeight * 4; i++)
//	{
//		imgBytes[i] = 255;
//	}
//
//	for (int i = 0; i < crossList.size(); i++)
//	{
//		Cross *cross = crossList[i];
//
//		for (int j = 0; j < cross->poly2D.vers.size(); j++)
//		{
//			Vector3f staPt = cross->poly2D.vers[j];
//			Vector3f endPt = cross->poly2D.vers[(j + 1) % cross->poly2D.vers.size()];
//
//			Vector3i staPixelPos;
//			Vector3i endPixelPos;
//
//			bool getStaPixelPos = GetPixelLocation(staPt, viewW, viewH, iWidth, iHeight, staPixelPos);
//			bool getEndPixelPos = GetPixelLocation(endPt, viewW, viewH, iWidth, iHeight, endPixelPos);
//
//			if ( getStaPixelPos == false || getEndPixelPos == false )
//			{
//				break;
//			}
//
//			vector<Vector3i> linePts = Rasterize2DLine(staPixelPos, endPixelPos);
//
//			for (int k = 0; k < linePts.size(); k++)
//			{
//				//int index = linePts[k].x * iHeight + linePts[k].y;
//				int index = linePts[k].y * iWidth + linePts[k].x;
//
//				if (index < 0 || index > iWidth*iHeight)
//				{
//					printf("index = %d  \n", index);
//					break;
//				}
//
//				imgBytes[4 * index] = 255;
//				imgBytes[4 * index + 1] = 0;
//				imgBytes[4 * index + 2] = 0;
//			}
//		}
//	}
//
//	return imgBytes;
//}
//
//vector<Vector3i> CrossMesh::Rasterize2DLine(Vector3i staPixelPos, Vector3i endPixelPos)
//{
//	int x0 = staPixelPos.x;
//	int y0 = staPixelPos.y;
//
//	int x1 = endPixelPos.x;
//	int y1 = endPixelPos.y;
//
//	int dx, dy, steps, k;
//	//int **line_point;
//	float xIncrement, yIncrement, x, y;
//
//	dx = x1 - x0;   dy = y1 - y0;
//	x = (float)x0;  y = (float)y0;
//	if (fabs((float)dx) > fabs((float)dy))   steps = (int)fabs((float)dx);
//	else                                     steps = (int)fabs((float)dy);
//
//	vector<Vector3i> linePts;
//
//	Vector3i pt0;
//	pt0.x = (int)(x0 + 0.5);
//	pt0.y = (int)(y0 + 0.5);
//	pt0.z = 0;
//
//	linePts.push_back(pt0);
//
//	xIncrement = float(dx) / float(steps);
//	yIncrement = float(dy) / float(steps);
//
//
//	for (k = 0; k < steps; k++)
//	{
//		x = x + xIncrement;
//		y = y + yIncrement;
//
//		Vector3i pt;
//		pt.x = (int)(x + 0.5);
//		pt.y = (int)(y + 0.5);
//		pt.z = 0;
//
//		linePts.push_back(pt);
//	}
//
//	return linePts;
//}
//
//bool CrossMesh::GetPixelLocation(Vector3f crossPt, float viewW, float viewH, GLint iWidth, GLint iHeight, Vector3i &pixelPos)
//{
//	if ( crossPt.x >= 0.5f*viewW || crossPt.x <= -0.5f*viewW ||
//		 crossPt.y >= 0.5f*viewH || crossPt.y <= -0.5f*viewH )
//	{
//		return false;
//	}
//
//	int u = ((crossPt.x + 0.5f*viewW) / viewW) * (iWidth - 1);
//	int v = ((crossPt.y + 0.5f*viewH) / viewH) * (iHeight - 1);
//
//	pixelPos = Vector3i(u, v, 0);
//
//	return true;
//}




//**************************************************************************************//
//                                       Rendering
//**************************************************************************************//
//
//void CrossMesh::DrawCrossMesh2D()
//{
//	for (int i = 0; i < crossList.size(); i++)
//	{
//		crossList[i]->poly2D.Draw(2.0, Vector3f(0.2, 0.9, 0.9));
//	}
//}
//
//void CrossMesh::DrawCrossMesh3D()
//{
//	for (int i = 0; i < crossList.size(); i++)
//	{
//		crossList[i]->DrawCross3D();
//	}
//}
//
//void CrossMesh::DrawCrossMesh3D_Dual()
//{
//	for (int i = 0; i < crossList.size(); i++)
//	{
//		crossList[i]->DrawConnection();
//	}
//}