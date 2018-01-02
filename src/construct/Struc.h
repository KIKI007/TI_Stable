///////////////////////////////////////////////////////////////
//
// Struc.h
//
//   3D Geometric Structure Class
//
// by Peng SONG  ( songpenghit@gmail.com )
// 
// 21/Nov/2017
//
///////////////////////////////////////////////////////////////

#include "vec.h"
#include <vector>
#include "Controls.h"

using namespace std;

class Surface;
class CrossMesh;
class Part;
class OrientPoint;



class Struc 
{
private:
	double worldMatrix[16];            // Object matrix
	double worldAxesMat[16];           // World axes matrix

public:
	Surface *surface;                  // Reference surface (Arch model)
	CrossMesh *crossMesh;              // Cross-section surface
	 
	vector<Part*> partList;            // Parts in the structure

	float avgPartSize;                 // Average part size
	float groundY;                     // Z-coordinate of the ground
	

public:
	Struc();
	~Struc();
	void Clear();

	// Model and Axes Transform
	void InitWorldMatrix(Vector3f position, Vector3f rotAxis, float rotAngle, float scale);
	void InitWorldAxesMatrix(Vector3f position, Vector3f rotAxis, float rotAngle);
	void ResetModel();

	// Initialize Structure
	void InitCrossMesh2D(int crossType, int crossRingNum);
	void LoadSurface(char * objFileName);
	void BindTexture(float viewSize);

	// Compute Structure (Arch Model)
	void CreateStructure_Arch(float viewSize, float tiltTheta);
	void ComputeBaseMesh(float viewSize, double footInverMat[16]);
	Vector3f GetTextureCoord(Vector3f point, float viewSize);
	void RemoveDanglingCross();

	// Build Parts Graph (Arch Model)
	void BuildPartsGraph_Arch();
	void IdentifyBoundaryParts();
	int GetAssoCrossID(int partID);
	int GetAssoPartID(int crossID);

	// Save Output Models
	void WriteStructure(char *folderPath);
	void WritePartGraph(char *folderPath);

	// Draw Input Model
	void DrawSurface(int showSurfTex);              
	void DrawSurfaceWire();

	// Draw Base Mesh
	void DrawCrossMesh3D();      
	void DrawCrossMesh2D();
	void DrawCrossMesh3D_Dual();
	void DrawCrossMesh3D_Inner();
	void DrawOriPoints();

	// Draw TI Construction
	void DrawGeomFaces();
	void DrawGeomEdges();
	void DrawGeomVertices();

	// Draw TI Structure
	void DrawStructure(int mode);
	void DrawStructureWire();
	void DrawStructureGraph();
	void DrawPickPart(int pickPartID);

	// Draw World Axes
	void DrawWorldAxes(int winW, int winH, float currFovy);
	void RotateWorldAxes(Vector3f rotAxis, float rotAngle);

	// Draw Ground
	void DrawGround();
	void DrawGridLines(float dense, float dimen, float gridY);
	void DrawGrid(float dense, float dimen, float gridY);

	// Transform Object
	void TranslateObject(Vector3f transVec);
	void RotateObject(Vector3f rotAxis, float rotAngle);
	void ScaleObject(Vector3f scaleVec);
};