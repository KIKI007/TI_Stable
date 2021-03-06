cmake_minimum_required(VERSION 3.1)
project(TI_Stable)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (CMAKE_CXX_STANDARD 11)

# libigl
option(LIBIGL_USE_STATIC_LIBRARY "Use libigl as static library" OFF)
option(LIBIGL_WITH_ANTTWEAKBAR      "Use AntTweakBar"    OFF)
option(LIBIGL_WITH_CGAL             "Use CGAL"           OFF)
option(LIBIGL_WITH_COMISO           "Use CoMiso"         OFF)
option(LIBIGL_WITH_CORK             "Use Cork"           OFF)
option(LIBIGL_WITH_EMBREE           "Use Embree"         OFF)
option(LIBIGL_WITH_LIM              "Use LIM"            OFF)
option(LIBIGL_WITH_MATLAB           "Use Matlab"         OFF)
option(LIBIGL_WITH_MOSEK            "Use MOSEK"          ON)
option(LIBIGL_WITH_NANOGUI          "Use Nanogui menu"   ON)
option(LIBIGL_WITH_OPENGL           "Use OpenGL"         ON)
option(LIBIGL_WITH_OPENGL_GLFW      "Use GLFW"           ON)
option(LIBIGL_WITH_PNG              "Use PNG"            ON)
option(LIBIGL_WITH_PYTHON           "Use Python"         OFF)
option(LIBIGL_WITH_TETGEN           "Use Tetgen"         OFF)
option(LIBIGL_WITH_TRIANGLE         "Use Triangle"       ON)
option(LIBIGL_WITH_VIEWER           "Use OpenGL viewer"  ON)
option(LIBIGL_WITH_XML              "Use XML"            OFF)
option(LIBIGL_WITH_SHAPEOP          "Use ShapeOp"        OFF)

################################################################################
### Compile the ShapeOp part ###
if(LIBIGL_WITH_SHAPEOP)
    set(SHAPEOP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ShapeOp")
    add_subdirectory("${SHAPEOP_DIR}")
    target_include_directories(ShapeOp PUBLIC "${SHAPEOP_DIR}/libShapeOp/src")
    target_include_directories(ShapeOp PUBLIC "${EIGEN3_INCLUDE_DIR}")
endif()

find_package(LIBIGL REQUIRED)

# Add your project file
file(GLOB SRCFILES src/*.cpp src/*.h)
add_executable(${PROJECT_NAME}_bin ${SRCFILES})
target_link_libraries(${PROJECT_NAME}_bin igl::core igl::viewer igl::mosek igl::triangle igl::png)
