/* Libigl */
#include <igl/readOFF.h>
#include <igl/viewer/Viewer.h>

/* system */
#include <sstream>
#include <iostream>

/* ShapeOp */
#include <Solver.h>
#include <Constraint.h>
#include <Force.h>

#include "Cube2D_Falling.h"
#include "VertexCollisionConstrain.h"
using Eigen::MatrixXd;
using Eigen::MatrixXi;
int main() {

    igl::viewer::Viewer viewer;
    MatrixXd V(12, 3);
    MatrixXi F(6, 3);
    ShapeOp::Solver s;

    F<<     0, 1, 2,
            2, 3, 0,
            4, 5, 6,
            6, 7, 4,
            8, 9, 10,
            10, 11, 8;
    Cube2D_Falling cubefall;
    cubefall.simulate(s);

    V = s.getPoints().transpose();
    std::cout << V << std::endl;

    viewer.data.set_mesh(V, F);
    viewer.launch();
    return 0;
}
