///////////////////////////////////////////////////////////////
//
// Controls.h
//
//   Defined Variables to Control the Program
//
// by Peng SONG ( songpenghit@gmail.com )
// 
// 10/Nov/2017
//
///////////////////////////////////////////////////////////////


#ifndef _CONTROLS
#define _CONTROLS


#define SURFACE_CylinderA80       1
#define SURFACE_HyperbolicA90     2
#define SURFACE_SeaShell          3
#define SURFACE_SphereA60         4
#define SURFACE_Torus             5
#define SURFACE_Spindle           6

#define PATTERN_QUAD              1
#define PATTERN_HEX               2



//**************************************************************************************//
//                                Control Parameters 
//**************************************************************************************//

//#define CO_PLANAR_THRES          0.000001
//#define CO_PLANAR_THRES          0.00001
#define CO_PLANAR_THRES          0.0001

#define MODEL_HEXAGON            1
#define MODEL_HEXAGON_5x5        2
#define MODEL_HEXAGON_10         3
#define MODEL_SQUARE             4
#define MODEL_SQUARE_5X5         5
#define MODEL_SQUARE_10          6

#define CROSS_TRI                1
#define CROSS_QUAD               2
#define CROSS_HEX                3

#define MOBILITY_SCORE_MAX       3.0
#define MOBILITY_SCORE_DIFF      0.0001

#define NONE_CROSS              -1
#define NONE_PART               -1

#define MOBILITY_SPHERE_R        3.0f



//**************************************************************************************//
//                                Control Parameters 
//**************************************************************************************//

// Initial World Pose
#define INIT_WORLD_POSITION            Vector3f(0.0,  -0.0, -3.8)
#define INIT_WORLD_ROT_AXIS            Vector3f(1.0,  0.0,  0.0)
#define INIT_WORLD_ROT_ANGLE           30.0
#define INIT_WORLD_SCALE               1.0

// Initial World Axes Pose
#define INIT_WORLD_AXES_POSITION       Vector3f(0.0,  0.0, -4.0)
#define INIT_WORLD_AXES_ROT_AXIS       Vector3f(1.0,  0.0,  0.0)
#define INIT_WORLD_AXES_ROT_ANGLE      15.0




//**************************************************************************************//
//                                    Definitions
//**************************************************************************************//

// DRAW Mode
#define DRAW_POINT             0
#define DRAW_LINE              1
#define DRAW_FLAT              2
#define DRAW_SMOOTH            3



#endif
