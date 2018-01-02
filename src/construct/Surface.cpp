///////////////////////////////////////////////////////////////
//
// Surface.cpp
//
//  3D Reference Surace
//
// by Song Peng ( song0083@ntu.edu.sg )
// 
// 21/Jan/2013
//
///////////////////////////////////////////////////////////////


#include "Surface.h"
#include "math3D.h"
#include "HelpDefine.h"
#include "HelpFunc.h"
#include "vec.h"
//#include "glm/glmint.h"



//**************************************************************************************//
//                                   Initialization
//**************************************************************************************//

Surface::Surface()
{
	model = NULL;
}

Surface::~Surface()
{
	if ( model != NULL )
	{ 
		glmDelete( model );
		model = NULL;
	}
}

void Surface::Clean()
{
	if (model != NULL)
	{
		glmDelete(model);
		model = NULL;
	}
}




//**************************************************************************************//
//                                   Load Surface Model
//**************************************************************************************//

void Surface::LoadModel(char * fileName)
{
	Clean();

	model = glmReadOBJ(fileName);

	if (!model) 
	{
		printf("Failed opening obj with name=%s.\n", fileName);
		exit(0);
	}
	else
	{
		//glmUnitize(model);
		//glmWriteOBJ(model, "J:\\SurfaceTiling\\SourceCode\\SurfaceCreator\\Model\\temp.obj", GLM_SMOOTH);
	}
}

void Surface::ComputeBoundingBox()
{
	float minX, minY, minZ, maxX, maxY, maxZ;
	minX = maxX = model->vertices[3];
	minY = maxY = model->vertices[4];
	minZ = maxZ = model->vertices[5];

	for (int i=2; i<model->numvertices; i++) 
	{
		if (maxX < model->vertices[3 * i + 0])    maxX = model->vertices[3 * i + 0];
		if (minX > model->vertices[3 * i + 0])    minX = model->vertices[3 * i + 0];

		if (maxY < model->vertices[3 * i + 1])    maxY = model->vertices[3 * i + 1];
		if (minY > model->vertices[3 * i + 1])    minY = model->vertices[3 * i + 1];

		if (maxZ < model->vertices[3 * i + 2])    maxZ = model->vertices[3 * i + 2];
		if (minZ > model->vertices[3 * i + 2])    minZ = model->vertices[3 * i + 2];
	}
	bbox.minPt = Vector3f(minX, minY, minZ);
	bbox.maxPt = Vector3f(maxX, maxY, maxZ);

	printf("BBox min:    %f %f %f\n", bbox.minPt[0], bbox.minPt[1], bbox.minPt[2]);
	printf("BBox max:    %f %f %f\n", bbox.maxPt[0], bbox.maxPt[1], bbox.maxPt[2]);
}


void Surface::ComputeFootMatrix()
{
	// Compute 2D bounding box of the parameterized surface mesh
	Box texBBox;                 
	float footScale;
	ComputeTextureBBox(texBBox, footScale);

	// Compute foot matrix that transform the 2D bbox into a unit 2D box [minPt(0,0), maxPt(1,1)] 
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();
	glTranslatef(0.5, 0.5, 0.0);
	glScalef(footScale, footScale, 1.0);
	glTranslatef(-0.5f*(texBBox.minPt.x + texBBox.maxPt.x), -0.5f*(texBBox.minPt.y + texBBox.maxPt.y), 0.0);

    double translate_0[16];
    identityd(translate_0);
    translate_0[12] = 0.5; translate_0[13] = 0.5;

    double scale_0[16];
    identityd(scale_0);
    scale_0[0] = footScale;
    scale_0[5] = footScale;

    double translate_1[16];
    identityd(translate_1);
    translate_1[12] = -0.5f*(texBBox.minPt.x + texBBox.maxPt.x);
    translate_1[13] = -0.5f*(texBBox.minPt.y + texBBox.maxPt.y);

    MultiplyMatrix(translate_1, scale_0, footMatrix);
    MultiplyMatrix(footMatrix, translate_0, footMatrix);

    PrintMatrix(footMatrix);

    return;

}

void Surface::ComputeTextureBBox(Box &texBBox, float &footScale)
{
	if ( model->numtexcoords <=0 )
		return;

	float minU, minV, maxU, maxV;
	minU = maxU = model->texcoords[2];
	minV = maxV = model->texcoords[3];

	for (int i=2; i<model->numtexcoords; i++) 
	{
		if (maxU < model->texcoords[2 * i + 0])    maxU = model->texcoords[2 * i + 0];
		if (minU > model->texcoords[2 * i + 0])    minU = model->texcoords[2 * i + 0];

		if (maxV < model->texcoords[2 * i + 1])    maxV = model->texcoords[2 * i + 1];
		if (minV > model->texcoords[2 * i + 1])    minV = model->texcoords[2 * i + 1];
	}

	texBBox.minPt = Vector3f(minU, minV, 0);
	texBBox.maxPt = Vector3f(maxU, maxV, 0);

	printf("Texture BBox min:  %f %f %f\n", texBBox.minPt[0], texBBox.minPt[1], texBBox.minPt[2]);
	printf("Texture BBox max:  %f %f %f\n", texBBox.maxPt[0], texBBox.maxPt[1], texBBox.maxPt[2]);

	footScale = 1 / max(maxU-minU, maxV-minV);
}




//**************************************************************************************//
//                                Bind Texture to Surface
//**************************************************************************************//
//
//void Surface::BindTextureImage(GLubyte *pBytes, GLint iWidth, GLint iHeight)
//{
//	if (textureID != -1)
//	{
//		glDeleteTextures(1, (GLuint*)&textureID);
//	}
//
//	// Black background
//	glClearDepth(1.0f);
//
//	// Textures applied as decals, no lighting or coloring effects
//	glEnable(GL_TEXTURE_2D);
//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//
//	// Load textures
//	glGenTextures(1, (GLuint*)&textureID);
//
//	// Bind to next texture object
//	glBindTexture(GL_TEXTURE_2D, textureID);
//
//	// Load texture, set filter and wrap modes
//	bool isAlpha = true;
//	int imgType;
//	//GLint iWidth, iHeight;
//	//GLubyte *pBytes = glmReadPNG(texFileName, isAlpha, &iWidth, &iHeight, &imgType);
//	GLenum eFormat = GL_RGBA;
//	GLint iComponents = GL_RGBA8;
//
//	gluBuild2DMipmaps(GL_TEXTURE_2D, iComponents, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE, pBytes);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	glDisable(GL_TEXTURE_2D);
//
//	// Don't need original texture data any more
//	free(pBytes);
//}

bool Surface::ComputeSurfaceCoord(Vector3f ptTexCoord, Vector3f &ptSurfCoord)
{
	if (model == NULL)
		return false;

	for (int i = 0; i < model->numtriangles; i++)
	{
		//printf(" i=%d \n", i);
		Triangle texTri;
		for (int j = 0; j < 3; j++)
		{
			int texIndex = model->triangles[i].tindices[j];
			texTri.v[j].x = model->texcoords[2 * texIndex];
			texTri.v[j].y = model->texcoords[2 * texIndex + 1];
			texTri.v[j].z = 0;
		}

		// Get the triangle in geometrical space
		Triangle geoTri;
		for (int j = 0; j < 3; j++)
		{
			int vertexIndex = model->triangles[i].vindices[j];
			geoTri.v[j].x = model->vertices[3 * vertexIndex];
			geoTri.v[j].y = model->vertices[3 * vertexIndex + 1];
			geoTri.v[j].z = model->vertices[3 * vertexIndex + 2];
		}

		// Skip the triangle if the point is out of its bounding box        
		Vector3f triMinPt = texTri.GetBBoxMinPt();
		Vector3f triMaxPt = texTri.GetBBoxMaxPt();
		if (ptTexCoord.x < triMinPt.x || ptTexCoord.x > triMaxPt.x ||
			ptTexCoord.y < triMinPt.y || ptTexCoord.y > triMaxPt.y)
		{
			continue;
		}

		// Barycentric interpolation if point is inside the triangle
		if (IsPointInsideTriangle(ptTexCoord, texTri, false))
		{
			// Compute total and partial triangle area 
			float totalArea = GetTriangleArea(texTri.v[0], texTri.v[1], texTri.v[2]);
			float subArea0 = GetTriangleArea(ptTexCoord, texTri.v[1], texTri.v[2]);
			float subArea1 = GetTriangleArea(texTri.v[0], ptTexCoord, texTri.v[2]);
			float subArea2 = GetTriangleArea(texTri.v[0], texTri.v[1], ptTexCoord);

			ptSurfCoord = (subArea0 / totalArea)*geoTri.v[0] +
				(subArea1 / totalArea)*geoTri.v[1] +
				(subArea2 / totalArea)*geoTri.v[2];

			return true;
		}
	}

	return false;
}




//**************************************************************************************//
//                           Render 3D Model and Surface Fan
//**************************************************************************************//

void Surface::DrawSurface(int showSurfTex)
{
	if ( model == NULL )
		return;
	
	// Enable/disable 2D texture
	if ( showSurfTex )
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	// Load texture matrix
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMultMatrixd(footMatrix);
	
	// Draw textured/smooth model
	glmFacetNormals(model);
	glmVertexNormals(model, 60.0, false);
	if ( showSurfTex )
		glmDraw(model, GLM_TEXTURE | GLM_SMOOTH);
	else
		glmDraw(model, GLM_SMOOTH);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	if (showSurfTex)
		glDisable(GL_TEXTURE_2D);
}


void Surface::DrawSurfaceWire()
{
	if ( model == NULL )
		return;

	glDisable(GL_LIGHTING);
	glLineWidth(1.2);
	glColor3f(0.3, 0.3, 0.3);

	// Push back original polygon mode
	glPushAttrib( GL_POLYGON_BIT );
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Shift the wire frame from the model a bit 
	glmDraw(model, GLM_SMOOTH);
	glEnable(GL_LIGHTING);

	// Pop up original polygon mode
	glPopAttrib();

	glEnable(GL_LIGHTING);
	glLineWidth(1.0);
}