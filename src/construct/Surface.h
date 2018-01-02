///////////////////////////////////////////////////////////////
//
// Surface.h
//
//  3D Reference Surface
//
// by Peng SONG  ( songpenghit@gmail.com )
// 
// 17/Nov/2017
//
///////////////////////////////////////////////////////////////

#include "vec.h"
#include "glm/glm.h"
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#include <vector>
#include "HelpStruct.h"

using namespace std;


class Surface 
{
public:
	GLMmodel *model;             // Surface model
	Box bbox;                    // 3D bounding box	

	double footMatrix[16];       // Transform the footprint to be inside a 1.0x1.0 window
	int textureID;               // Texture ID of cross pattern

public:
	Surface();
	~Surface();
	void Clean();

	// Load Surface Model
	void LoadModel(char * fileName);
	void ComputeBoundingBox();
	void ComputeFootMatrix();
	void ComputeTextureBBox(Box &texBBox, float &footScale);

	// Bind Texture to Surface
//	void BindTextureImage(GLubyte *pBytes, GLint iWidth, GLint iHeight);
	bool ComputeSurfaceCoord(Vector3f ptTexCoord, Vector3f &ptSurfCoord);

	// Render 3D Model and Footprint in Texture Domain
	void DrawSurface(int showSurfTex);
	void DrawSurfaceWire();
};


