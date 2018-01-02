/////////////////////////////////////////////////////////////////
////
//// gluiBuild.h
////
//// - building the GLUI interface
////
//// by Peng SONG ( songpenghit@gmail.com )
////
//// 03/Aug/2016
////
////
/////////////////////////////////////////////////////////////////
//
//
//#ifndef _GLUI_BUILD
//#define _GLUI_BUILD
//
//
//#include "glui.h"
//
//#define _MAX_ROLLOUT	32
//
//// ID for GLUI callback
//#define ID_QUIT					11
//#define ID_HELP					12
//#define ID_ABOUT				13
//#define ID_ROTATE_PANELS		14
//#define ID_CLOSE_PANELS			15
//
//// ID for basic panel (edit boxes)
//#define ID_PATTERN_ID           20
//#define ID_PATTERN_SIZE         21
//
//// ID for basic panel (buttons)
//#define ID_READ_SURFACE         30
//#define ID_CREATE_STRUC         31
//#define ID_READ_MODEL           32
//#define ID_READ_POLYGON         33
//#define ID_CREATE_STRUC_2       34
//#define ID_FUNC_TEST            35
//#define ID_RESET_STRUC          36
//#define ID_SAVE_STRUC           37
//#define ID_SAVE_DISASSEM        38
//
//// ID for show input model
//#define ID_SHOW_AXES		    40
//#define ID_SHOW_GROUND          41
//#define ID_SHOW_SURF            42
//#define ID_SHOW_SURF_WIRE       43
//#define ID_SHOW_SURF_TEX        44
//#define ID_SHOW_MODEL           45
//#define ID_SHOW_MODEL_WIRE      46
//
//// ID for show base mesh
//#define ID_SHOW_CROSS_2D        50
//#define ID_SHOW_CROSS_3D        51
//#define ID_SHOW_CROSS_DUAL      52
//#define ID_SHOW_CROSS_INNER     53
//#define ID_SHOW_ORI_POINTS      54
//
//// ID for show TI construction
//#define ID_SHOW_GEOM_FACE       60
//#define ID_SHOW_GEOM_EDGE       61
//#define ID_SHOW_GEOM_VERTEX     62
//
//// ID for show TI mobility
//#define ID_SHOW_MOBILI_FACE     64
//#define ID_SHOW_MOBILI_EDGE     65
//#define ID_SHOW_MOBILI_VERTEX   66
//#define ID_SHOW_MOBILI_RAY      67
//#define ID_SHOW_MOBILI_VECTOR   68
//#define ID_SHOW_MOBILI_MESH     69
//
//// ID for show TI structure
//#define ID_SHOW_STRUC           70
//#define ID_SHOW_STRUC_WIRE      71
//#define ID_SHOW_STRUC_NORMAL    72
//#define ID_SHOW_STRUC_GRAPH     73
//
//
//#define ID_DRAW_MODE            80
//
//
//// Build the GLUI interface
//extern void initGLUI();
//
//// rotate the rollout panels in main panel
//extern void closeAllRollouts();
//extern void rotatePanelsOffset(int value);
//extern int  rotatePanelsTo(char key);
//
//// Build the GLUI interface
//extern void initGLUI();
//static void buildInterface();
//
//// Add Panels
//static void addBasicPanel(GLUI_Panel *panel);
//static void addRenderPanel(GLUI_Panel *panel);
//
//// Callback function
//extern void callbackGLUI(int id);
//
//// Open and Save File
//void ReaSurfaceFile_OBJ();
////void ReaModelFile_OBJ();
//void SaveStructureFiles();
////void SaveDisassemFiles();
////void ReaPolygonFile_DAT();
//
//void GetSurfaceID(char *objFileName);
////void SetSurfaceParameters();
//
//bool GetOpenFileName(HWND hWnd, LPSTR szFile, int StringSize);
//bool GetSaveFileName(HWND hWnd, LPSTR szFile, int StringSize);
//
//// Auxiliary functions
//void myExit(int exitCode);
//void myHelp();
//void myAbout();
//extern void resetStatusBar(const char * fmt, ... );
//
//
//#endif
