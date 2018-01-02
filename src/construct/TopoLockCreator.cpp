/////////////////////////////////////////////////////////////////
////
//// TopoLockCreator.cpp
////
////   Create Topological Interlocking Structure
////
//// by Peng SONG ( songpenghit@gmail.com )
////
//// 15/Nov/2017
////
/////////////////////////////////////////////////////////////////
//
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <math.h>
//#include <limits.h>
//
//#ifdef WIN32
//#include <windows.h>
//#include <float.h>
//#else
//#include <unistd.h>
//#endif
//
//#ifdef __linux
//#include <values.h>
//#endif
//
//#include <string.h>
//
//#if defined (__APPLE__) || defined(MACOSX) /* GLUT/ on Mac OS X */
//#include <OPENGL/gl.h>
//#include <GLUT/glut.h>                     /* for GLUT, GL, GLU */
//#else                                      /* GL/ on IRIX etc. */
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glut.h>
//#endif
//
//#include "HelpDefine.h"
//#include "gluiBuild.h"
//#include "gluiKey.h"
//#include "gluiMouse.h"
//#include "TopoLockCreator.h"
//#include "Controls.h"
//#include "math3D.h"
//#include "HelpFunc.h"
//#include "Part.h"
//#include "Struc.h"
//
//
//
/////////////////////////////////////////////////////////////////
//// Global Variables
/////////////////////////////////////////////////////////////////
//
//
////////////////////////////
//// INTERNAL
//
//// window stuff
//int  mainWindowID, winW, winH;
//char programName[_MAX_STR_SIZE],winTitle[_MAX_STR_SIZE],aboutStr[_MAX_STR_SIZE];
//int  backgndColor[3];
//
//// Show input model
//int showAxes;
//int showGround;
//int showSurf;
//int showSurfWire;
//int showSurfTex;
//
//// Show base mesh
//int showCross2D;
//int showCross3D;
//int showCrossDual;
//int showCrossInner;
//int showOriPoints;
//
//// Show TI construction
//int showGeomFace;
//int showGeomEdge;
//int showGeomVertex;
//
//// Show TI structure
//int showStruc;
//int showStrucWire;
//int showStrucGraph;
//
//// viewing
//float currFovy;
//float currAspect;
//int showStatusBar;
//
//// Surface model
//Struc myStruc;
//float texViewSize;
//float tiltTheta;
//int pickPartID;
//int surfaceID;
//int patternID;
//
//
//// Various modes
//int drawMode;
//
//// object material
//GLfloat materialAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
//GLfloat materialDiffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };
//GLfloat materialSpecular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
//GLfloat materialEmission[] = { 0.1f,0.1f,0.1f,1.0f };
//GLfloat materialShininess[] = { 76.8f };
//
////////////////////////////
//// EXTERNAL
//
//extern GLUI * glui, *gluiStatusBar   ;
//extern double mouseMotionSensitivity ;
//
//
//
/////////////////////////////////////////////////////////////////
//// Function Declarations
/////////////////////////////////////////////////////////////////
//
//void myInit();
//void glInit();
//
//void resetProj();
//void reshape(int w, int h);
//
//void display(void);
//
//
////**************************************************************************************//
////                            Initialization and Cleanup
////**************************************************************************************//
//
//void myInit()
//{
//    ////////////////////////////////////////////////////////////////////
//    // Note: No OpenGL initialization should be put here, glInit instead
//    ////////////////////////////////////////////////////////////////////
//
//
//    ////////////////////////////////////////////////////////////////////
//    // Setup the window
//
//    winW = WIN_W;
//    winH = WIN_H;
//
//    // window title
//    sprintf(programName,"%s %s.%s",PROGRAM_NAME,VERSION_MAJOR,VERSION_MINOR);
//    sprintf(winTitle,"%s",programName);
//
//    // about string
//    strcat(aboutStr, PROGRAM_ABOUT);
//
//
//    ////////////////////////////////////////////////////////////////////
//    // Initial Viewing
//
//    currFovy   = _DEFAULT_FOVY;
//    currAspect = (float) winW / (float) winH;
//
//
//    ////////////////////////////////////////////////////////////////////
//    // Status
//
//    showAxes         = _TRUE;
//	showGround       = _FALSE;
//
//	showSurf         = _TRUE;
//	showSurfWire     = _FALSE;
//	showSurfTex      = _TRUE;
//
//	showCross2D      = _FALSE;
//	showCross3D      = _TRUE;
//	showCrossDual    = _TRUE;
//	showCrossInner   = _FALSE;
//	showOriPoints    = _FALSE;
//
//	showGeomFace     = _FALSE;
//	showGeomEdge     = _FALSE;
//	showGeomVertex   = _FALSE;
//
//	showStruc        = _TRUE;
//	showStrucWire    = _TRUE;
//	showStrucGraph   = _FALSE;
//
//	patternID = PATTERN_HEX;
//
//	texViewSize = 5.0;
//
//	tiltTheta = 35.264389;     // theta = asin(sqrt(3.0) / 3.0) * (180 / M_PI)
//
//	//tiltTheta = 60;
//
//	pickPartID = NONE_PART;
//
//
//    ////////////////////////////////////////////////////////////////////
//    // Mouse motion sensitivity
//
//    mouseMotionSensitivity = 1.0;
//
//
//    ////////////////////////////////////////////////////////////////////
//    // Status bar
//
//    showStatusBar = _TRUE;
//    resetStatusBar("Ready.");
//
//
//    ////////////////////////////////////////////////////////////////////
//    // background color
//
//    backgndColor[0] = 255 ;
//    backgndColor[1] = 255 ;
//    backgndColor[2] = 255 ;
//
//
//    ///////////////////////////////////////
//    // no idle rotation or motion
//
//    stopIdleMotion();
//
//	///////////////////////////////////////
//	// Surface model related
//
//	drawMode = DRAW_FLAT;
//	//drawMode = DRAW_LINE;
//}
//
//
//void glInit()
//{
//    GLfloat ambient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
//    GLfloat diffuse[]  = { 0.7f, 0.7f, 0.7f, 1.0f };
//    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//    GLfloat position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
//
//
//    ////////////////////////////////////////////////////////////////////
//    // 1. Color and Lighting
//
//    glClearColor( backgndColor[0]/255.0f, backgndColor[1]/255.0f, backgndColor[2]/255.0f, 0.0f );
//
//
//    ////////////////////////////////////////////////////////////////////
//    // 2. various status
//
//    // may not work on all machines: improves TEXTURE specular shading
//    #ifdef GL_VERSION_1_2
//    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
//    #endif
//    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
//
//    glEnable(GL_DEPTH_TEST);
//    glClearDepth(1.0f);
//    glDepthFunc(GL_LESS);
//
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//
//    // Anti-aliasing
//    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
//    glEnable(GL_POINT_SMOOTH);
//    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
//    glEnable(GL_LINE_SMOOTH);
//
//    // Auto-Normalization
//    glEnable(GL_NORMALIZE);
//
//    // Cull the back face (speedup and transparency)
//    glCullFace( GL_BACK );
//
//
//    ////////////////////////////////////////////////////////////////////
//    // 3. Lighting
//
//    glMatrixMode(GL_MODELVIEW);
//    glPushMatrix();
//    glLoadIdentity();
//
//    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
//    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
//    glLightfv(GL_LIGHT0, GL_POSITION, position);
//
//    glEnable(GL_LIGHT0);
//
//    glPopMatrix();
//
//
//	////////////////////////////////////////////////////////////////////
//	// 4. Object material
//
//	float MatAmbientBack[]  = {1.0f, 0.0f, 0.0f, 1.0f};	// back material
//	glMaterialfv( GL_BACK,GL_AMBIENT,	  MatAmbientBack);
//	glMaterialfv( GL_FRONT, GL_AMBIENT,   materialAmbient);
//	glMaterialfv( GL_FRONT, GL_DIFFUSE,   materialDiffuse);
//	glMaterialfv( GL_FRONT, GL_SPECULAR,  materialSpecular);
//	glMaterialfv( GL_FRONT, GL_SHININESS, materialShininess);
//	glMaterialfv( GL_FRONT, GL_EMISSION,  materialEmission);
//
//
//	////////////////////////////////////////////////////////////////////
//	// 5. Object and axes matrices
//
//	myStruc.InitWorldMatrix(INIT_WORLD_POSITION, INIT_WORLD_ROT_AXIS, INIT_WORLD_ROT_ANGLE, INIT_WORLD_SCALE);
//	myStruc.InitWorldAxesMatrix(INIT_WORLD_AXES_POSITION, INIT_WORLD_AXES_ROT_AXIS, INIT_WORLD_AXES_ROT_ANGLE);
//}
//
//
//
//
////**************************************************************************************//
////                           Reset view / projection / Reshape
////**************************************************************************************//
//
//void resetProj()
//{
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(currFovy,currAspect,_Z_NEAR,_Z_FAR);
//    glMatrixMode(GL_MODELVIEW);
//}
//
//void reshape(int w, int h)
//{
//	int tx,ty,tw,th;
//
//	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
//	glViewport(tx,ty,tw,th);
//
//	winW = tw;
//	winH = th;
//
//	//    if (statusBar)
//	//	statusBar->set_w( winW-10 );
//
//	if (showStatusBar)
//		glViewport(0,25,tw,th);
//	else
//		glViewport(0,0,tw,th+25);
//
//	currAspect = (float) tw / (float) th;
//
//	resetProj();
//}
//
//
//
//
////**************************************************************************************//
////                                   Rendering Functions
////**************************************************************************************//
//
//void display(void)
//{
//	////////////////////////////////////////////////////////////////////
//	// (1) Set rendering mode
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	switch( drawMode )
//	{
//	case DRAW_POINT:
//		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
//		glShadeModel(GL_SMOOTH);
//		break;
//	case DRAW_LINE:
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		glShadeModel(GL_SMOOTH);
//		break;
//	case DRAW_FLAT:
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//		glShadeModel(GL_FLAT);
//		break;
//	//case DRAW_SMOOTH:
//	//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	//	glShadeModel(GL_SMOOTH);
//	//	break;
//	default:
//		break;
//	}
//
//
//	////////////////////////////////////////////////////////////////////
//	// (2) Draw various stuff
//
//	if (showAxes)          myStruc.DrawWorldAxes(winW, winH, currFovy);
//
//	if (showSurf)          myStruc.DrawSurface(showSurfTex);
//	if (showSurfWire)      myStruc.DrawSurfaceWire();
//
//	if (showCross2D)       myStruc.DrawCrossMesh2D();
//	if (showCross3D)       myStruc.DrawCrossMesh3D();
//	if (showCrossDual)     myStruc.DrawCrossMesh3D_Dual();
//	if (showCrossInner)    myStruc.DrawCrossMesh3D_Inner();
//	if (showOriPoints)     myStruc.DrawOriPoints();
//
//	if (showGeomFace)      myStruc.DrawGeomFaces();
//	if (showGeomEdge)      myStruc.DrawGeomEdges();
//	if (showGeomVertex)    myStruc.DrawGeomVertices();
//
//	if (showStruc)         myStruc.DrawStructure(GL_RENDER);
//	if (showStrucWire)     myStruc.DrawStructureWire();
//	if (showStruc)         myStruc.DrawPickPart(pickPartID);
//	if (showStrucGraph)    myStruc.DrawStructureGraph();
//
//	//if (showGround)        myStruc.DrawGround();
//
//
//	// Refresh
//	glutSwapBuffers();
//	glFlush();
//}
//
//void CreatePolygon_Regular(int edgeNum, float radius)
//{
//	for (int i = edgeNum-1; i >=0; i--)
//	{
//		float angle = (i /(float) edgeNum) * 2*M_PI;
//		float x = radius * cos(angle);
//		float z = radius * sin(angle);
//
//		printf("v %6.3f  %6.3f  %6.3f\n", x, 0.0, z);
//	}
//}
//
//
////**************************************************************************************//
////                                    Main Program
////**************************************************************************************//
//
//void usage(char *cmd)
//{
//    fprintf(stderr,"Usage : %s\n\n",cmd);
//}
//
//
//int main(int argc, char **argv)
//{
//    ////////////////////////////////////////////////////////////////////
//    // 1. Initialization
//
//    // initialize various non-GL stuff
//    myInit();
//
//	//myStruc.Function_Test();
//
//	if      (patternID == PATTERN_HEX)	    myStruc.InitCrossMesh2D(CROSS_HEX, 16);
//	else if (patternID == PATTERN_QUAD)     myStruc.InitCrossMesh2D(CROSS_QUAD, 20);
//
//
//	//CreatePolygon_Regular(30, 1.0);
//
//	//float angle = acos(0.01) * 180 / M_PI;
//	//printf("angle: %.3f \n", angle);
//
//    ////////////////////////////////////////////////////////////////////
//    // 2. GLUT Initialization
//
//    // initialization
//    glutInit(&argc,argv);
//    glutInitWindowSize(winW, winH);
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
//
//    // create a new window and give a title to it
//    mainWindowID = glutCreateWindow(winTitle);
//
//    // initialize OpenGL stuff
//    glInit();
//
//
//    ////////////////////////////////////////////////////////////////////
//    // 3. GLUI Creation
//
//    initGLUI();
//
//
//    ////////////////////////////////////////////////////////////////////
//    // 4. GLUT Callbacks
//
//    glutDisplayFunc(display);
//
//    GLUI_Master.set_glutReshapeFunc(reshape);
//    GLUI_Master.set_glutMouseFunc(mouse);
//    GLUI_Master.set_glutKeyboardFunc(keyboard);
//    GLUI_Master.set_glutSpecialFunc(specKey);
//
//    GLUI_Master.set_glutIdleFunc(idle);
//
//    glutMotionFunc(motion);
//
//    glutMainLoop();
//
//    return(EXIT_SUCCESS);
//}
//
//
//// Note:
//// -----
//// If we are running in Windows non-console mode, the starting point for
//// for program is WinMain instead of main().
//
//#if (!defined(_CONSOLE)) && defined(WIN32)
//
//int WINAPI
//WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, PSTR cmd, int showmode)
//{
//    char  *args[1024];			/* arbitrary limit, but should suffice */
//    char  *p, *q, **argv = args;
//    int    argc = 0;
//
//    argv[argc++] = programName;
//    p = cmd;
//    for (;;) {
//	if (*p == ' ')
//	    while (*++p == ' ')
//		;
//	/* now p points at the first non-space after some spaces */
//	if (*p == '\0')
//	    break;    /* nothing after the spaces:  done */
//	argv[argc++] = q = p;
//	while (*q && *q != ' ')
//	    ++q;
//	/* now q points at a space or the end of the string */
//	if (*q == '\0')
//	    break;    /* last argv already terminated; quit */
//	*q = '\0';    /* change space to terminator */
//	p = q + 1;
//    }
//    argv[argc] = NULL;   /* terminate the argv array itself */
//
//    return main(argc,argv);
//}
//
//#endif
