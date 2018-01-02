///////////////////////////////////////////////////////////////
//
// glmimg_png.cpp
//
//  - Read PNG image pixel data
//
// by Song Peng ( song0083@ntu.edu.sg )
// 
// 02/Dec/2012
//
//
///////////////////////////////////////////////////////////////

#include "glm.h"
#include "glmint.h"
#include "lodepng.h"
#include <vector>


GLubyte* glmReadPNG(const char* filename, GLboolean alpha, int* imgWidth, int* imgHeight, int* imgType)
{
	// Read png image size and pixel data
	unsigned imgW, imgH;
	std::vector<unsigned char> pngPixelData;
	unsigned error = lodepng::decode(pngPixelData, imgW, imgH, filename);

	// Copy the image size and type
	*imgWidth  = imgW;
	*imgHeight = imgH;
	*imgType   = GL_RGBA; // Note: png data should have RGBA channel

	// Copy the image pixel data
	unsigned char *buffer;
	if ( pngPixelData.size() == 0 )
	{
		buffer = NULL;
	}
	else
	{
		buffer = (unsigned char*) malloc(pngPixelData.size());

		for (int i=0; i<pngPixelData.size(); i++)
			buffer[i] = pngPixelData[i];
	}
	
	return buffer;
}