/////////////////////////////////////////////////////////////////
////
//// gluiBuild.cpp
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
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdarg.h>
//
//#ifdef WIN32
//#include <windows.h>
//#include <float.h>
//#else
//#include <limits.h>
//#endif
//
//
//#include "gluiBuild.h"
//#include "TopoLockCreator.h"
//#include "HelpDefine.h"
//#include "HelpFunc.h"
//#include "Controls.h"
//#include "Struc.h"
//
//
//#define  RIGHT_PANEL_WIDTH   250
//
//////////////////////////////////////////////////////////
//// (1) Global Variables
//
//
//#define _UNKNOWN	"---"
//
//
////////////////////////////////////////////////
//// (A) External from driver program
//
//// window
//extern int winW,winH;
//extern int mainWindowID;
//extern int backgndColor[3];
//extern char programName[_MAX_STR_SIZE];
//extern char winTitle[_MAX_STR_SIZE];
//extern char aboutStr[_MAX_STR_SIZE];
//extern int  preMouseX;
//extern int  preMouseY;
//
//// Show input model
//extern int showAxes;
//extern int showGround;
//extern int showSurf;
//extern int showSurfWire;
//extern int showSurfTex;
//extern int showModel;
//extern int showModelWire;
//
//// Show base mesh
//extern int showCross2D;
//extern int showCross3D;
//extern int showCrossDual;
//extern int showCrossInner;
//extern int showOriPoints;
//
//// Show TI construction
//extern int showGeomFace;
//extern int showGeomEdge;
//extern int showGeomVertex;
//
//// Show TI structure
//extern int showStruc;
//extern int showStrucWire;
//extern int showStrucGraph;
//
//// Surface model
//extern Struc myStruc;
//extern float texViewSize;
//extern float tiltTheta;
//extern int surfaceID;
//extern int patternID;
//
//// Various mode
//extern int   drawMode;
//
//
////////////////////////////////////////////////
//// (B) Internal
//
//Vector3f scaleVec = Vector3f(1,1,1);
//float rotAngle = 0.0;
//Vector3f rotAxis = Vector3f(1,0,0);
//
//// using GLUI
//GLUI *glui          = NULL;
//GLUI *gluiStatusBar = NULL;
//
//// list of rollout
//GLUI_Rollout *rolloutList[_MAX_ROLLOUT];
//int  rolloutStart;
//int  nRolloutList  = 0;
//int  rolloutOffset = 0;
//char rolloutChar[_MAX_ROLLOUT];
//
//// Status Bar
//GLUI_StaticText *statusBar;
//char             statusStr[_MAX_STR_SIZE];
//
//
//
//
////**************************************************************************************//
////                            Rotating the RollOuts in Main Panel
////**************************************************************************************//
//
//void closeAllRollouts()
//{
//    int i;
//
//    for (i=0; i<nRolloutList; i++)
//	rolloutList[i]->close();
//}
//
//
//static void resetPanels()
//{
//    int offset,i;
//
//    // remove all the rollouts
//    while ( glui->remove_control(glui->get_main_panel(),rolloutStart,false) ) ;
//
//    // current starting point
//    offset = rolloutOffset;
//
//    // add all the rollouts
//    for (i=0; i<nRolloutList; i++) {
//
//	glui->add_separator(false,0);
//
//	glui->add_control(glui->get_main_panel(),rolloutList[offset],true);
//	offset = (offset+1) % nRolloutList;
//    }
//}
//
//
//void rotatePanelsOffset(int value)
//{
//    // any rollout?
//    if (!nRolloutList)
//	return;
//
//    // rollout offset
//    rolloutOffset += value;
//
//    // make sure it is positive
//    if (rolloutOffset >= nRolloutList)
//        rolloutOffset -= ((rolloutOffset-nRolloutList) / nRolloutList + 1) * nRolloutList;
//    if (rolloutOffset < 0)
//        rolloutOffset += ((-rolloutOffset) / nRolloutList + 1) * nRolloutList;
//
//    // rotate the rollout panels
//    resetPanels();
//}
//
//
//int rotatePanelsTo(char key)
//{
//    GLUI_Rollout *tmpControl,*tmp2Control;
//
//    int index,i,from,tmpCH,tmp2CH;
//
//
//    // any rollout?
//    if (!nRolloutList)
//	return _FALSE;
//
//
//    // 1. find the index
//
//    // to lowercase
//    if ('A' <= key && key <= 'Z')
//	key = key-'A'+'a';
//
//    // index to the selected stuff
//    index = -1;
//    for (i=0; i<nRolloutList; i++)
//	if (rolloutChar[i] == key)
//	    index = i;
//
//    // cannot find the key?
//    if (index == -1)
//	return _FALSE;
//
//
//    // 2. close the rollout if it is currently open
//
//    if (rolloutList[index]->is_open) {
//	rolloutList[index]->close();
//	return _TRUE;
//    }
//
//
//    // 3. rearrange rolloutList and rolloutChar consistently
//
//    tmpControl = rolloutList[index];
//    tmpCH      = rolloutChar[index];
//    from       = rolloutOffset;
//
//    if (from != index) {
//
//	from--;
//
//	do {
//
//	    from = (from+1) % nRolloutList;
//
//	    // swap(tmpControl,rolloutList[from]);
//	    tmp2Control       = tmpControl;
//	    tmpControl        = rolloutList[from];
//	    rolloutList[from] = tmp2Control;
//
//	    // swap(tmpCH,rolloutList[from]);
//	    tmp2CH            = tmpCH;
//	    tmpCH             = rolloutChar[from];
//	    rolloutChar[from] = tmp2CH;
//
//	} while (from != index);
//    }
//
//
//    // 4. reset the panel
//
//    resetPanels();
//
//
//    // 5. open this panel
//
//    rolloutList[rolloutOffset]->open();
//
//
//    return _TRUE;
//}
//
//
//
//
////**************************************************************************************//
////                              Main Interface Builder
////**************************************************************************************//
//
//void initGLUI()
//{
//	int tx,ty,tw,th;
//
//	// create the GLUI panel
//	glui = GLUI_Master.create_glui_subwindow(mainWindowID,GLUI_SUBWINDOW_RIGHT);
//	glui->bkgd_color.set(GLUI_BGCOLOR);
//	glui->fogd_color.set(GLUI_FGCOLOR);
//
//	// create the status bar
//	gluiStatusBar = GLUI_Master.create_glui_subwindow(mainWindowID,GLUI_SUBWINDOW_BOTTOM);
//	gluiStatusBar->bkgd_color.set(STATUSBAR_BGCOLOR);
//	gluiStatusBar->fogd_color.set(STATUSBAR_FGCOLOR);
//
//	// create the controls/widgets
//	buildInterface();
//
//	// who is the main window
//	glui->set_main_gfx_window( mainWindowID );
//	gluiStatusBar->set_main_gfx_window( mainWindowID );
//
//	// reset the window size
//	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
//	//winW += (winW - tw);
//	glutReshapeWindow(winW*2-tw,winH*2-th);
//}
//
//
//static void buildInterface()
//{
//	///////////////////////////////////////////
//	// (1) This is a control panel
//
//	glui->add_separator(false,2);
//	glui->add_separator();
//
//	GLUI_StaticText *infoText
//		= glui->add_statictext( "CONTROL PANEL" );
//	infoText->set_alignment( GLUI_ALIGN_CENTER );
//
//	glui->add_separator();
//
//
//	// Quit / Help / About
//
//	glui->add_separator(false,-5);
//
//	GLUI_Panel *lastPanel = glui->add_panel( "", GLUI_PANEL_NONE );
//	lastPanel->set_w(RIGHT_PANEL_WIDTH, false);
//
//	glui->add_column_to_panel(lastPanel,false);
//	GLUI_Button *quitButton = glui->add_button_to_panel(lastPanel,"Quit",ID_QUIT,callbackGLUI);
//	glui->add_column_to_panel(lastPanel,false);
//	GLUI_Button *helpButton = glui->add_button_to_panel(lastPanel,"Help",ID_HELP,callbackGLUI);
//	glui->add_column_to_panel(lastPanel,false);
//	GLUI_Button *aboutButton = glui->add_button_to_panel(lastPanel,"About",ID_ABOUT,callbackGLUI);
//	glui->add_column_to_panel(lastPanel,false);
//	GLUI_Button *menuButton_down = glui->add_button_to_panel(lastPanel,"-",ID_CLOSE_PANELS,callbackGLUI);
//	glui->add_column_to_panel(lastPanel,false);
//	GLUI_Button *menuButton_up = glui->add_button_to_panel(lastPanel,"m",ID_ROTATE_PANELS,callbackGLUI);
//
//	quitButton->set_w(42,42);
//	helpButton->set_w(42,42);
//	aboutButton->set_w(42,42);
//	menuButton_up->set_w(12,12);
//	menuButton_down->set_w(12,12);
//
//	quitButton->set_alignment(GLUI_ALIGN_CENTER);
//	helpButton->set_alignment(GLUI_ALIGN_CENTER);
//	aboutButton->set_alignment(GLUI_ALIGN_CENTER);
//	menuButton_up->set_alignment(GLUI_ALIGN_CENTER);
//	menuButton_down->set_alignment(GLUI_ALIGN_CENTER);
//
//	glui->add_separator(false,-14);
//
//	glui->add_separator(false,0);
//
//	rolloutStart = (glui->get_main_panel())->get_num_childs();
//
//
//	///////////////////////////////////////////
//	// (2) Basic Panel
//
//	glui->add_separator(false,0);
//
//	GLUI_Rollout *rollout1 = glui->add_rollout("Basic Control", true);
//	rollout1->set_w(RIGHT_PANEL_WIDTH, true);
//
//	addBasicPanel(rollout1);
//
//	// add to rollout list
//	rolloutChar[nRolloutList] = 'b';
//	rolloutList[nRolloutList++] = (GLUI_Rollout *) rollout1;
//
//
//	///////////////////////////////////////////
//	// (3) Application Panel
//
//	glui->add_separator(false,0);
//
//	GLUI_Rollout *rollout2 = glui->add_rollout("Render Control", true);
//	rollout2->set_w(RIGHT_PANEL_WIDTH, true);
//
//	addRenderPanel(rollout2);
//
//	// add to rollout list
//	rolloutChar[nRolloutList] = 'a';
//	rolloutList[nRolloutList++] = (GLUI_Rollout *) rollout2;
//
//
//	///////////////////////////////////////////
//	// (4) Status Bar
//
//	statusBar = gluiStatusBar->add_statictext(statusStr);
//
//	statusBar->set_w( winW-10 );
//	statusBar->set_alignment(GLUI_ALIGN_LEFT);
//}
//
//
//
//
////**************************************************************************************//
////                                    Add Panels
////**************************************************************************************//
//
//static void addBasicPanel(GLUI_Panel *panel)
//{
//	glui->add_separator_to_panel(panel, false, 6);
//
//	GLUI_Spinner *patternIDSpinner = glui->add_spinner_to_panel(panel, "Pattern ID :    ", GLUI_SPINNER_INT, &(patternID), ID_PATTERN_ID, callbackGLUI);
//	patternIDSpinner->set_alignment(GLUI_ALIGN_CENTER);
//	patternIDSpinner->set_w(120);
//	patternIDSpinner->set_int_limits(0, 10, GLUI_LIMIT_WRAP);
//
//	GLUI_Spinner *patternSizeSpinner = glui->add_spinner_to_panel(panel, "Pattern Size : ", GLUI_SPINNER_FLOAT, &(texViewSize), ID_PATTERN_SIZE, callbackGLUI);
//	patternSizeSpinner->set_alignment(GLUI_ALIGN_CENTER);
//	patternSizeSpinner->set_w(120);
//	patternSizeSpinner->set_int_limits(1.0, 16.0, GLUI_LIMIT_WRAP);
//
//	GLUI_Spinner *tiltAngleSpinner = glui->add_spinner_to_panel(panel, "Tilt Angle :     ", GLUI_SPINNER_FLOAT, &(tiltTheta));
//	tiltAngleSpinner->set_alignment(GLUI_ALIGN_CENTER);
//	tiltAngleSpinner->set_w(120);
//	tiltAngleSpinner->set_int_limits(0, 90, GLUI_LIMIT_WRAP);
//
//
//	//////////////////////////////////////////////
//	// Add buttons
//
//	glui->add_separator_to_panel(panel, false, 4);
//	GLUI_Panel *subPanel = glui->add_panel_to_panel(panel, "", GLUI_PANEL_NONE);
//
//	GLUI_Button *readSurfButton = glui->add_button_to_panel(subPanel, "Read Surface", ID_READ_SURFACE, callbackGLUI);
//	glui->add_separator_to_panel(subPanel, false, 6);
//	GLUI_Button *saveStructButton = glui->add_button_to_panel(subPanel, "Save Struc", ID_SAVE_STRUC, callbackGLUI);
//
//
//	glui->add_column_to_panel(subPanel, false);
//	GLUI_Button *createStructButton = glui->add_button_to_panel(subPanel, "Create Struc", ID_CREATE_STRUC, callbackGLUI);
//	glui->add_separator_to_panel(subPanel, false, 6);
//
//	readSurfButton->set_alignment(GLUI_ALIGN_LEFT);
//	createStructButton->set_alignment(GLUI_ALIGN_LEFT);
//
//	glui->add_separator_to_panel(panel, false, 0);
//}
//
//
//static void addRenderPanel(GLUI_Panel *panel)
//{
//    //glui->add_separator_to_panel(appAdjpanel,false,1);
//    //glui->add_separator_to_panel(appAdjpanel,false,0);
//
//	GLUI_Panel * subPanel1 = glui->add_panel_to_panel(panel, "", GLUI_PANEL_NONE);
//	subPanel1->set_alignment(GLUI_ALIGN_CENTER);
//
//	//////////////////////////////////////////////
//	// Show input models
//
//	GLUI_Panel * subPanel1Left = glui->add_panel_to_panel( subPanel1 , "Input Model");
//
//	glui->add_checkbox_to_panel(subPanel1Left, "axes ",       &showAxes,       ID_SHOW_AXES,            callbackGLUI);
//	//glui->add_checkbox_to_panel(subPanel1Left, "ground ",     &showGround,     ID_SHOW_GROUND,          callbackGLUI);
//	glui->add_checkbox_to_panel(subPanel1Left, "surf",        &showSurf,       ID_SHOW_SURF,            callbackGLUI);
//	glui->add_checkbox_to_panel(subPanel1Left, "surf wire",   &showSurfWire,   ID_SHOW_SURF_WIRE,       callbackGLUI);
//	glui->add_checkbox_to_panel(subPanel1Left, "surf tex",    &showSurfTex,    ID_SHOW_SURF_TEX,        callbackGLUI);
//
//
//
//	//////////////////////////////////////////////
//	// Show base surface
//
//	glui->add_column_to_panel(subPanel1, false);
//	GLUI_Panel * subPanel1Right = glui->add_panel_to_panel( subPanel1 , "Base Mesh");
//
//	//glui->add_checkbox_to_panel(subPanel1Left, "cross2D",    &showCross2D,   ID_SHOW_CROSS_2D,   callbackGLUI);
//	glui->add_checkbox_to_panel(subPanel1Right, "cross base",  &showCross3D,    ID_SHOW_CROSS_3D,    callbackGLUI);
//	glui->add_checkbox_to_panel(subPanel1Right, "cross dual",  &showCrossDual,  ID_SHOW_CROSS_DUAL,  callbackGLUI);
//	glui->add_checkbox_to_panel(subPanel1Right, "cross inner", &showCrossInner, ID_SHOW_CROSS_INNER, callbackGLUI);
//
//	glui->add_checkbox_to_panel(subPanel1Right, "ori points",  &showOriPoints,  ID_SHOW_ORI_POINTS,  callbackGLUI);
//
//
//	//////////////////////////////////////////////
//	// Show structure
//
//	GLUI_Panel * subPanel2 = glui->add_panel_to_panel(panel, "", GLUI_PANEL_NONE);
//	subPanel2->set_alignment(GLUI_ALIGN_CENTER);
//
//	GLUI_Panel * subPanel2Left = glui->add_panel_to_panel(subPanel2, "TI Construct");
//
//	glui->add_checkbox_to_panel(subPanel2Left, "face  ",   &showGeomFace,   ID_SHOW_GEOM_FACE,   callbackGLUI);
//	glui->add_checkbox_to_panel(subPanel2Left, "edge   ",  &showGeomEdge,   ID_SHOW_GEOM_EDGE,   callbackGLUI);
//	glui->add_checkbox_to_panel(subPanel2Left, "vertex  ", &showGeomVertex, ID_SHOW_GEOM_VERTEX, callbackGLUI);
//
//	glui->add_column_to_panel(subPanel2, false);
//	GLUI_Panel * subPanel2Right = glui->add_panel_to_panel(subPanel2, "TI Structure");
//
//	glui->add_checkbox_to_panel(subPanel2Right, "struc ", &showStruc, ID_SHOW_STRUC, callbackGLUI);
//	glui->add_checkbox_to_panel(subPanel2Right, "struc wire", &showStrucWire, ID_SHOW_STRUC_WIRE, callbackGLUI);
//	glui->add_checkbox_to_panel(subPanel2Right, "struc graph", &showStrucGraph, ID_SHOW_STRUC_NORMAL, callbackGLUI);
//
//
//}
//
//
//
//
////**************************************************************************************//
////                                   GLUI Callback
////**************************************************************************************//
//
//void callbackGLUI(int id)
//{
//    void myExit(int exitCode);
//    void myHelp();
//    void myAbout();
//    void resetStatusBar(const char * fmt, ... );
//    void resetProj();
//
//    int winID;
//
//
//    // Make sure the view is mainWindow
//    winID = glutGetWindow();
//    if (winID != mainWindowID)
//	glutSetWindow(mainWindowID);
//
//    switch(id)
//	{
//
//
//    ////////////////////////////////////////////////////////////////////
//    // QUIT / HELP / ABOUT
//    ////////////////////////////////////////////////////////////////////
//
//	case ID_QUIT :
//		myExit(EXIT_SUCCESS);
//		break;
//
//    case ID_HELP :
//		myHelp();
//		break;
//
//    case ID_ABOUT :
//		myAbout();
//		break;
//
//    case ID_ROTATE_PANELS :
//		rotatePanelsOffset(1);
//		resetStatusBar("cycle control panel.");
//		break;
//
//    case ID_CLOSE_PANELS :
//		closeAllRollouts();
//		resetStatusBar("all panels closed.");
//		break;
//
//
//	////////////////////////////////////////////////////////////////////
//    // Basic Adjustment Panel
//    ////////////////////////////////////////////////////////////////////
//
//	case ID_PATTERN_ID:
//		if (patternID == PATTERN_HEX)	        myStruc.InitCrossMesh2D(CROSS_HEX, 16);
//		else if (patternID == PATTERN_QUAD)     myStruc.InitCrossMesh2D(CROSS_QUAD, 20);
//		myStruc.BindTexture(texViewSize);
//		break;
//
//	case ID_PATTERN_SIZE:
//		myStruc.BindTexture(texViewSize);
//		glutPostRedisplay();
//		break;
//
//	case ID_READ_SURFACE:
//		ReaSurfaceFile_OBJ();
//		preMouseX = -1; // Prevent idle rotation
//		preMouseY = -1;
//		glutPostRedisplay();
//		break;
//
//	case ID_CREATE_STRUC:
//		myStruc.CreateStructure_Arch(texViewSize, tiltTheta);
//		myStruc.BuildPartsGraph_Arch();
//		myStruc.IdentifyBoundaryParts();
//		glutPostRedisplay();
//		glutPostRedisplay();
//		break;
//
//	case ID_SAVE_STRUC:
//		SaveStructureFiles();
//		break;
//    }
//
//    if (winID != mainWindowID)
//	glutSetWindow(winID);
//}
//
//
//
//
////**************************************************************************************//
////                                 Open and Save File
////**************************************************************************************//
//
//void GetSurfaceID(char *objFileName)
//{
//	string sObjFileName = string(objFileName);
//
//	if      (sObjFileName.find("CylinderA80")   != string::npos)     surfaceID = SURFACE_CylinderA80;
//	else if (sObjFileName.find("HyperbolicA90") != string::npos)     surfaceID = SURFACE_HyperbolicA90;
//	else if (sObjFileName.find("SeaShell")      != string::npos)     surfaceID = SURFACE_SeaShell;
//	else if (sObjFileName.find("Torus")         != string::npos)     surfaceID = SURFACE_Torus;
//	else if (sObjFileName.find("Spindle")       != string::npos)     surfaceID = SURFACE_Spindle;
//	else if (sObjFileName.find("SphereA60")     != string::npos)     surfaceID = SURFACE_SphereA60;
//
//	printf("surfaceID: %d   \n", surfaceID);
//}
//
//void ReaSurfaceFile_OBJ()
//{
//	HWND hWnd=GetForegroundWindow();
//	char objFileName[256];
//
//	if( GetOpenFileName(hWnd, objFileName, 256 ) )
//	{
//		myStruc.LoadSurface(objFileName);
//
//		myStruc.BindTexture(texViewSize);
//
//		GetSurfaceID(objFileName);
//		//SetSurfaceParameters();
//
//		resetStatusBar(objFileName);
//	}
//}
//
//void SaveStructureFiles()
//{
//	HWND hWnd = GetForegroundWindow();
//	char dummyFileName[FILE_NAME_LENGTH];
//
//	if (GetSaveFileName(hWnd, dummyFileName, FILE_NAME_LENGTH))
//	{
//		char folderPath[FILE_NAME_LENGTH];
//		GetFolderPath(dummyFileName, folderPath);
//
//		myStruc.WriteStructure(folderPath);
//
//		resetStatusBar(folderPath);
//	}
//}
//
//bool GetOpenFileName(HWND hWnd, LPSTR szFile, int StringSize)
//{
//	OPENFILENAME   ofn;
//	char   szFileTitle[256];
//	strcpy(szFileTitle, "Open File");
//	szFile[0]=0;
//
//	memset(&ofn,0,sizeof(OPENFILENAME));
//	ofn.lStructSize     =   sizeof(OPENFILENAME);
//	ofn.hwndOwner   =   hWnd;
//	ofn.lpstrFilter  = "All\0*.*\0.obj\0*.obj\0";
//	ofn.nFilterIndex    =   1;
//	ofn.lpstrFile          =   szFile;
//	ofn.nMaxFile        =   StringSize;
//	ofn.lpstrTitle         =   szFileTitle;
//	ofn.Flags              =   OFN_FILEMUSTEXIST;
//
//	if(GetOpenFileName(&ofn)!=TRUE)
//	{
//		DWORD   Errval;
//		char   Errstr[50]= "Common   Dialog   Error:   ";
//		char   buf[5];
//		Errval=CommDlgExtendedError();
//		if(Errval!=0)
//		{
//			wsprintf(buf, "%ld ",Errval);
//			strcat(Errstr,buf);
//			MessageBox(NULL,Errstr, "Warning ",MB_OK|MB_ICONSTOP);
//		}
//		return false;
//	}
//
//	return   true;
//}
//
//bool GetSaveFileName(HWND hWnd, LPSTR szFile, int StringSize)
//{
//	OPENFILENAME   ofn;
//	char   szFileTitle[256];
//	strcpy(szFileTitle, "Save File");
//	szFile[0]=0;
//
//	memset(&ofn,0,sizeof(OPENFILENAME));
//	ofn.lStructSize     =   sizeof(OPENFILENAME);
//	ofn.hwndOwner   =   hWnd;
//	ofn.lpstrFilter  = "All\0*.*\0.obj\0*.obj\0";
//	ofn.nFilterIndex    =   1;
//	ofn.lpstrFile          =   szFile;
//	ofn.nMaxFile        =   StringSize;
//	ofn.lpstrTitle         =   szFileTitle;
//	ofn.Flags              =   OFN_FILEMUSTEXIST;
//
//	if(GetSaveFileName(&ofn)!=TRUE)
//	{
//		DWORD   Errval;
//		char   Errstr[50]= "Common   Dialog   Error:   ";
//		char   buf[5];
//		Errval=CommDlgExtendedError();
//		if(Errval!=0)
//		{
//			wsprintf(buf, "%ld ",Errval);
//			strcat(Errstr,buf);
//			MessageBox(NULL,Errstr, "Warning ",MB_OK|MB_ICONSTOP);
//		}
//		return false;
//	}
//
//	return   true;
//}
//
//
//
//
////**************************************************************************************//
////                                  Auxiliary Functions
////**************************************************************************************//
//
//void resetStatusBar(const char * fmt, ... )
//{
//	va_list argp;
//	char    str[_MAX_STR_SIZE];
//
//
//	// 1. string to be put onto the status bar
//
//	va_start(argp, fmt);
//	vsprintf(str, fmt, argp);
//	va_end(argp);
//
//
//	// 2. put it on the status bar if it is not on the status bar now
//
//	if (strcmp(str,statusStr)) {
//		strcpy(statusStr,str);
//		if (statusBar) statusBar->set_text(statusStr);
//	}
//}
//
//
//#ifdef WIN32
//DWORD WINAPI
//openHelpMessageBox()
//{
//	MessageBox(NULL,_HELP_MESSAGE,programName,MB_ICONINFORMATION|MB_SETFOREGROUND|MB_TASKMODAL);
//	return 0;
//}
//
//DWORD WINAPI
//openAboutMessageBox()
//{
//	MessageBox(NULL,aboutStr,programName,MB_SETFOREGROUND|MB_TASKMODAL);
//	return 0;
//}
//#endif
//
//
//void myExit(int exitCode)
//{
//	///////////////////////////////////////////
//	// really quit?
//
//#ifdef WIN32
//
//	int r = MessageBox(NULL,"Really Quit?",programName,MB_ICONEXCLAMATION|MB_YESNO|MB_DEFBUTTON2);
//	if (r == IDNO)
//		return;
//
//#endif
//
//	exit(exitCode);
//}
//
//
//void myHelp()
//{
//	// feedback
//#ifdef _WIN32
//
//	HANDLE hThread;
//	DWORD  IDThread;
//
//	// Create a thread for the message box window
//	hThread = CreateThread( NULL,						// no security attributes
//		0,						// use default stack size
//		(LPTHREAD_START_ROUTINE) openHelpMessageBox,	// thread function
//		NULL,						// no thread function argument
//		0,						// use default creation flags
//		&IDThread );					// returns thread identifier
//
//	// Check the return value for success.
//	if (hThread == NULL) fprintf(stderr,"Error: creation menu.\n");
//
//#else
//
//	fprintf(stderr,_HELP_MESSAGE);
//
//#endif
//}
//
//
//void myAbout()
//{
//#ifdef _WIN32
//
//	HANDLE hThread;
//	DWORD  IDThread;
//
//	// Create a thread for the message box window
//	hThread = CreateThread( NULL,						// no security attributes
//		0,						// use default stack size
//		(LPTHREAD_START_ROUTINE) openAboutMessageBox,	// thread function
//		NULL,						// no thread function argument
//		0,						// use default creation flags
//		&IDThread );					// returns thread identifier
//
//	// Check the return value for success.
//	if (hThread == NULL) fprintf(stderr,"Error: creation menu.\n");
//
//#else
//
//	// feedback
//	resetStatusBar(PROGRAM_ABOUT_ONE_LINE);
//
//#endif
//}
//
