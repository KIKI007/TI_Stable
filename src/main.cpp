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
igl::viewer::Viewer viewer;
MatrixXd V(13, 3);
MatrixXi F(7, 3);
ShapeOp::Solver s;
void SolveOpt()
{
    s.solve(100);
    V = s.getPoints().transpose();
    viewer.data.set_mesh(V, F);
}
int main() {

    F<<     0, 1, 2,
            2, 3, 0,
            4, 5, 6,
            6, 7, 4,
            8, 9, 10,
            10, 11, 12,
            12, 8, 10;
    Cube2D_Falling cubefall;
    cubefall.simulate(s, V);
    viewer.data.set_mesh(V, F);
    viewer.callback_init = [&](igl::viewer::Viewer& viewer)
    {
        viewer.ngui->addButton("Hello", SolveOpt);
        viewer.screen->performLayout();
        return false;
    };

    viewer.launch();
    return 0;
}
