/////////////////////////////////////////////////////////////////
////
//// ChainHull.cpp
////
////   Compute 2D Convex Hull for a set of co-planar points
////
//// by Peng SONG ( songpenghit@gmail.com )
////
//// 17/May/2017
////
////
/////////////////////////////////////////////////////////////////
//
//#include "ChainHull.h"
//
//
//
////**************************************************************************************//
////                              Chain Huall Algorithm
////**************************************************************************************//
//
//// Link: http://geomalgorithms.com/a10-_hull-1.html
//// chainHull_2D(): Andrew's monotone chain 2D convex hull algorithm
////     Input:  P[] = an array of 2D points
////                  presorted by increasing x and y-coordinates
////             n =  the number of points in P[]
////     Output: H[] = an array of the convex hull vertices (max is n)
////     Return: the number of points in H[]
//
//int chainHull_2D(vector<Vector3f> P, int n, vector<Vector3f> &H)
//{
//	printf("step_1 \n");
//	// Pre-process input points (add by Peng SONG)
//	HeapSort(P, true);
//	H = P;
//
//	printf("step_2 \n");
//
//	// the output array H[] will be used as the stack
//	int    bot = 0, top = (-1);   // indices for bottom and top of the stack
//	int    i;                 // array scan index
//
//	// Get the indices of points with min x-coord and min|max y-coord
//	int minmin = 0, minmax;
//	float xmin = P[0].x;
//	for (i = 1; i < n; i++)
//		if (P[i].x != xmin) break;
//	minmax = i - 1;
//	if (minmax == n - 1) {       // degenerate case: all x-coords == xmin
//		H[++top] = P[minmin];
//		if (P[minmax].y != P[minmin].y) // a  nontrivial segment
//			H[++top] = P[minmax];
//		H[++top] = P[minmin];            // add polygon endpoint
//		return top + 1;
//	}
//
//	printf("step_3 \n");
//
//	// Get the indices of points with max x-coord and min|max y-coord
//	int maxmin, maxmax = n - 1;
//	float xmax = P[n - 1].x;
//	for (i = n - 2; i >= 0; i--)
//		if (P[i].x != xmax) break;
//	maxmin = i + 1;
//
//	printf("step_4 \n");
//
//	// Compute the lower hull on the stack H
//	H[++top] = P[minmin];      // push  minmin point onto stack
//	i = minmax;
//	while (++i <= maxmin)
//	{
//		// the lower line joins P[minmin]  with P[maxmin]
//		if (isLeft(P[minmin], P[maxmin], P[i]) >= 0 && i < maxmin)
//			continue;           // ignore P[i] above or on the lower line
//
//		while (top > 0)         // there are at least 2 points on the stack
//		{
//			// test if  P[i] is left of the line at the stack top
//			if (isLeft(H[top - 1], H[top], P[i]) > 0)
//				break;         // P[i] is a new hull  vertex
//			else
//				top--;         // pop top point off  stack
//		}
//		H[++top] = P[i];        // push P[i] onto stack
//	}
//
//	printf("step_5 \n");
//
//	// Next, compute the upper hull on the stack H above  the bottom hull
//	if (maxmax != maxmin)      // if  distinct xmax points
//		H[++top] = P[maxmax];  // push maxmax point onto stack
//	bot = top;                  // the bottom point of the upper hull stack
//	i = maxmin;
//	while (--i >= minmax)
//	{
//		// the upper line joins P[maxmax]  with P[minmax]
//		if (isLeft(P[maxmax], P[minmax], P[i]) >= 0 && i > minmax)
//			continue;           // ignore P[i] below or on the upper line
//
//		while (top > bot)     // at least 2 points on the upper stack
//		{
//			// test if  P[i] is left of the line at the stack top
//			if (isLeft(H[top - 1], H[top], P[i]) > 0)
//				break;         // P[i] is a new hull  vertex
//			else
//				top--;         // pop top point off  stack
//		}
//		H[++top] = P[i];        // push P[i] onto stack
//	}
//	if (minmax != minmin)
//		H[++top] = P[minmin];  // push  joining endpoint onto stack
//
//	return top + 1;
//}
//
//// Assume that a class is already given for the object:
////    Point with coordinates {float x, y;}
////===================================================================
//
//
//// isLeft(): tests if a point is Left|On|Right of an infinite line.
////    Input:  three points P0, P1, and P2
////    Return: >0 for P2 left of the line through P0 and P1
////            =0 for P2 on the line
////            <0 for P2 right of the line
////    See: Algorithm 1 on Area of Triangles
//inline float isLeft(Vector3f P0, Vector3f P1, Vector3f P2)
//{
//	return (P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y);
//}
////===================================================================
//
//
//
//
////**************************************************************************************//
////                       Sort Points Based on X- and Y-coordinate
////**************************************************************************************//
//
//vector<int> HeapSort(vector<Vector3f> &Array, bool isAscend)
//{
//	int count = Array.size();
//	int start, end;
//
//	// Initialize the index array
//	vector<int> Indices;
//	for (int i = 0; i < count; i++)
//		Indices.push_back(i);
//
//	/* heapify */
//	for (start = (count - 2) / 2; start >= 0; start--)
//	{
//		siftDown(Array, Indices, start, count, isAscend);
//	}
//
//	for (end = count - 1; end > 0; end--)
//	{
//		swap(Array[end], Array[0]);
//		swap(Indices[end], Indices[0]);
//
//		siftDown(Array, Indices, 0, end, isAscend);
//	}
//
//	return Indices;
//}
//
//bool IS_LESS(Vector3f a, Vector3f b)
//{
//	if (a.x < b.x || (a.x == b.x && a.y < b.y))
//		return true;
//	else
//		return false;
//}
//
//bool IS_LARGE(Vector3f a, Vector3f b)
//{
//	if (a.x < b.x || (a.x == b.x && a.y < b.y))
//		return false;
//	else
//		return true;
//}
//
//void siftDown(vector<Vector3f> &Array, vector<int> &Indices, int start, int end, bool isAscend)
//{
//	int root = start;
//
//	while (root * 2 + 1 < end)
//	{
//		int child = 2 * root + 1;
//
//		if (child == Array.size() - 1)
//			return;
//
//		if ((isAscend == true && IS_LESS(Array[child], Array[child + 1])) ||
//			(isAscend == false && IS_LARGE(Array[child], Array[child + 1])))
//		{
//			if (child + 1 < end)
//			{
//				child += 1;
//			}
//		}
//
//		if ((isAscend == true && IS_LESS(Array[root], Array[child])) ||
//			(isAscend == false && IS_LARGE(Array[root], Array[child])))
//		{
//			swap(Array[child], Array[root]);
//			swap(Indices[child], Indices[root]);
//
//			root = child;
//		}
//		else
//			return;
//	}
//}
