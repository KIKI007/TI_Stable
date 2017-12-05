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
#include "TI_Cube.h"
#include "RigidBodyConstrain.h"

using Eigen::MatrixXd;
using Eigen::MatrixXi;
igl::viewer::Viewer viewer;
//MatrixXd V(13, 3);
//MatrixXi F(7, 3);
MatrixXd V;
MatrixXi F;
//ShapeOp::Solver s;
//void SolveOpt()
//{
////    s.solve(100);
////    V = s.getPoints().transpose();
////    viewer.data.set_mesh(V, F);
//    TI_Cube cubes(5, 5);
//    stdVecMatrixXd vec_V;
//    stdVecMatrixXi vec_F;
//    cubes.generate(vec_V, vec_F);
//    V = MatrixXd(vec_V.size() * 8, 3);
//    F = MatrixXi::Zero(vec_F.size() * 12, 3);
//    for(int id = 0; id < vec_V.size(); id++)
//    {
//        for(int jd = 0; jd < vec_V[id].rows(); jd++)
//        {
//            V.row(jd + 8 * id) = vec_V[id].row(jd);
//        }
//    }
//    int ID = 0;
//    for(int id = 0; id < vec_F.size(); id++)
//    {
//        if(id % 5 == 0 || id % 5 == 4 || id < 5 || id > 4 * 5) {
//            for (int jd = 0; jd < vec_F[id].rows(); jd++) {
//                F.row(ID++) = vec_F[id].row(jd) + Eigen::RowVector3i(8 * id, 8 * id, 8 * id);
//            }
//        }
//    }
//
//    viewer.data.set_mesh(V, F);
//}
ShapeOp::Matrix3X shape;
void generate_cube()
{
    TI_Cube cubes(1, 1);
    stdVecMatrixXd vec_V;
    stdVecMatrixXi vec_F;
    cubes.generate_1v6(vec_V, vec_F);
//    V = vec_V[0];
//    F = vec_F[0];
//
//    //outer
////    ShapeOp::Matrix3X shape;
////    ShapeOp::Matrix33 R(3, 3);
////    R <<        1, 0, 0,
////            0, std::cos(M_PI / 4), -std::sin(M_PI / 4),
////            0, std::sin(M_PI / 4),  std::cos(M_PI / 4);
//    shape = V.transpose();
//    viewer.data.clear();
//    viewer.data.set_mesh(V, F);


    V = MatrixXd(vec_V.size() * 8, 3);
    F = MatrixXi::Zero(vec_F.size() * 12, 3);
    for(int id = 0; id < vec_V.size(); id++)
    {
        for(int jd = 0; jd < vec_V[id].rows(); jd++)
        {
            V.row(jd + 8 * id) = vec_V[id].row(jd);
        }
    }
    int ID = 0;
    for(int id = 0; id < vec_F.size(); id++)
    {
        for (int jd = 0; jd < vec_F[id].rows(); jd++) {
            F.row(ID++) = vec_F[id].row(jd) + Eigen::RowVector3i(8 * id, 8 * id, 8 * id);
        }
    }
    viewer.data.set_mesh(V, F);
}

void Rigid_Body_Opt()
{
    ShapeOp::Solver solver;

    ShapeOp::Matrix3X points = V.transpose();
    solver.setPoints(points);

    ShapeOp::Scalar weight(1);
    //fixed point
    {
        for (int id = 0; id < 8; id++) {
            std::vector<int> id_vector;
            id_vector.push_back(id);
            auto c = std::make_shared<ShapeOp::ClosenessConstraint>(id_vector, weight, solver.getPoints());
            solver.addConstraint(c);
        }
    }

    //set rigid body
    {
        std::vector<int> id_vector;
        id_vector.clear();
        for(int id = 0; id < 8; id++)
        {
            id_vector.push_back(id);
        }

        auto c = std::make_shared<ShapeOp::RigidBodyConstrain>(id_vector, weight, solver.getPoints());
        solver.addConstraint(c);
        // edit the shapes one of which the rigid constraint brings the involved vertices close to.
        std::vector<ShapeOp::Matrix3X> shapes;

//        //inner
//        ShapeOp::Vector3 center(0, 0, 0);
//        for(int id = 0; id < shape.cols(); id++)
//            center += shape.col(id);
//        center/= shape.cols();
//        for(int id = 0; id < shape.cols(); id++)
//            shape.col(id) = (shape.col(id) - center) * 0.5 + center;
//        shapes.push_back(shape);
        c->setShape(shape);
    }
    solver.initialize(false);
    solver.solve(1);
    V = solver.getPoints().transpose();

    viewer.data.clear();
    viewer.data.set_mesh(V, F);
}

void move_point()
{
    for(int id = 0; id < 8; id++)
        V.row(id) -= Eigen::RowVector3d(0, 0, 0.1);
    viewer.data.clear();
    viewer.data.set_mesh(V, F);
}

int main() {

//    F<<     0, 1, 2,
//            2, 3, 0,
//            4, 5, 6,
//            6, 7, 4,
//            8, 9, 10,
//            10, 11, 12,
//            12, 8, 10;
//    Cube2D_Falling cubefall;
//    cubefall.simulate(s, V);
//    viewer.data.set_mesh(V, F);

    generate_cube();

    viewer.callback_init = [&](igl::viewer::Viewer& viewer)
    {
        viewer.ngui->addButton("Move", move_point);
        viewer.ngui->addButton("Run", Rigid_Body_Opt);
        viewer.screen->performLayout();
        return false;
    };

    viewer.launch();
    return 0;
}
