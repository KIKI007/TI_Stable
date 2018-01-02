/////////////////////////////////////////////////////////////////
////
//// gluiKey.cpp
////
//// - handle keyboard event
////
//// by Peng SONG ( song0083@ntu.edu.sg )
////
//// 29/Oct/2012
////
////
/////////////////////////////////////////////////////////////////
//
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//
//#ifdef WIN32
//#include <windows.h>
//#endif
//
//#ifdef WIN32
//#include <windows.h>
//#include <GL/gl.h>
//#include <GL/glut.h>
//#endif
//
//#ifdef __APPLE__
//#include <OPENGL/gl.h>
//#include <GLUT/glut.h>
//#endif // __APPLE__
//
//#include "HelpDefine.h"
//#include "gluiBuild.h"
//#include <vector>
//#include "vec.h"
//#include "Struc.h"
//
//using namespace std;
//
/////////////////////////////////////////////////////////////////
//// Global Variables
/////////////////////////////////////////////////////////////////
//
//extern int mainWindowID ;
//extern int showAxes ;
//
//extern Struc myStruc;
//
//
/////////////////////////////////////////////////////////////////
//// Internal Functions
/////////////////////////////////////////////////////////////////
//
//
//
//
////**************************************************************************************//
////                                  Keyboard Callbacks
////**************************************************************************************//
//
//void keyboard ( unsigned char key, int x, int y )
//{
//    static int waitForSecondKey = _FALSE;
//    static char str[_MAX_STR_SIZE];
//    int winID,r,i,l;
//
//    // rollout list
//    extern int  nRolloutList;
//    extern char rolloutChar[_MAX_ROLLOUT];
//
//    // external function
//    void myExit ( int ) ;
//    void myHelp (     ) ;
//
//
//    ///////////////////////////////////////////
//    // Make sure pointing to the mainWindow
//
//    winID = glutGetWindow();
//    if (winID != mainWindowID)
//	glutSetWindow(mainWindowID);
//
//
//    ///////////////////////////////////////////
//    // For second key
//
//    if (waitForSecondKey)
//	{
//		// rotate the panels
//		if ('1' <= key && key <= '1'+nRolloutList-1)
//			r = rotatePanelsTo(key-'1');
//		else
//			r = rotatePanelsTo(key);
//
//		waitForSecondKey = _FALSE;
//
//		// if success, just return
//		if (r)
//		{
//			resetStatusBar("control panel rearranged.");
//			return;
//		}
//    }
//
//
//    switch (key)
//	{
//
//
//	////////////////////////////////////////////////////////////////////
//	// Quit (27 -> ESC)
//
//	case 'q' : case 'Q' : case 27 :
//		myExit(EXIT_SUCCESS);
//		break;
//
//    case 'h' : case 'H' :
//		myHelp();
//		break;
//
//    case 'm' :
//		if (strncmp(str,"Please",6))
//		{
//			// init. str once only
//			sprintf(str,"Rearrange subpanels by keys: %c",rolloutChar[0]);
//			l = strlen(str);
//			for (i=1; i<nRolloutList; i++) {
//			str[l++] = ',';
//			str[l++] = ' ';
//			str[l++] = rolloutChar[i];
//			}
//			str[l++] = '.';
//			str[l]   = '\0';
//			strcat(str, " ( M - close all subpanels )");
//		}
//		resetStatusBar(str);
//		waitForSecondKey = _TRUE;	// wait for the next key
//		break;
//
//    case 'M' :
//		closeAllRollouts();
//		resetStatusBar("all panels closed.");
//		break;
//
//
//    ////////////////////////////////////////////////////////////////////
//    // status
//
//    case 'a' : case 'A' :
//		showAxes = 1 - showAxes;
//		callbackGLUI( ID_SHOW_AXES );
//		break;
//
//    default:
//		break;
//    }
//
//
//    if (winID != mainWindowID)
//	glutSetWindow(winID);
//}
//
//
//void specKey ( int key, int x, int y )
//{
//}
