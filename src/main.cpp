/* Libigl */
#include <igl/readOFF.h>
#include <igl/viewer/Viewer.h>

/* system */
#include <sstream>
#include <iostream>

///* ShapeOp */
//#include <Solver.h>
//#include <Constraint.h>
//#include <Force.h>

#include "Cube2D_Falling.h"
#include "VertexCollisionConstrain.h"
#include "TI_Cube.h"
#include "RigidBodyConstrain.h"
#include "igl/mosek/mosek_linprog.h"
#include "PolygonPoints.h"
#include "PolyhedraPoints.h"
#include "OptSolver.h"
#include "PolygonsCollisionSolver.h"

using Eigen::MatrixXd;
using Eigen::MatrixXi;
igl::viewer::Viewer viewer;
//MatrixXd V(13, 3);
//MatrixXi F(7, 3);
MatrixXd V;
MatrixXi F;
typedef Eigen::Triplet<double> T;

VectorXd x_0(0, 1);
const int NTRI = 20;
const double TRI_LEN = 0.5;

void mosek()
{
    Eigen::VectorXd c(2);
    c << -2,
            5;
    Eigen::SparseMatrix<double> A(1, 2);
    std::vector<T> triplist;
    triplist.push_back(T(0, 0, 1));
    triplist.push_back(T(0, 1, 1));
    A.setFromTriplets(triplist.begin(), triplist.end());
    Eigen::VectorXd lc(1);
    lc << 200;
    Eigen::VectorXd uc(1);
    uc << 10000000;
    Eigen::VectorXd lx(2);
    lx << 100,
            80;
    Eigen::VectorXd ux(2);
    ux << 200,
            170;

    Eigen::VectorXd x(2);
    bool sucess = igl::mosek::mosek_linprog(c, A, lc, uc, lx, ux, x);
    if(sucess)
        std::cout << x;
    else
        std::cout << "fail";
}

void generate_polygons(vector<PolygonPoints> &P)
{
    PolygonPoints P0 ,P1, P2, P3, P4;
    std::vector<int> ids;

    ids.clear();
    ids.push_back(0);ids.push_back(1);ids.push_back(2);ids.push_back(3);
    P0.set_ids(ids);
    MatrixXd points = MatrixXd(3, 4);
    points.col(0) = Vector3d(0, -1, 0);
    points.col(1) = Vector3d(5, -1, 0);
    points.col(2) = Vector3d(5, 0, 0);
    points.col(3) = Vector3d(0, 0, 0);
    P0.set_points(points);

    ids.clear();
    ids.push_back(0);ids.push_back(1);ids.push_back(2);ids.push_back(3);
    P1.set_ids(ids);
    points = MatrixXd(3, 4);
    points.col(0) = Vector3d(5, 0, 0);
    points.col(1) = Vector3d(6, 0, 0);
    points.col(2) = Vector3d(6, 5, 0);
    points.col(3) = Vector3d(5, 5, 0);
    P1.set_points(points);

    ids.clear();
    ids.push_back(0);ids.push_back(1);ids.push_back(2);ids.push_back(3);
    P2.set_ids(ids);
    points = MatrixXd(3, 4);
    points.col(0) = Vector3d(0, 5, 0);
    points.col(1) = Vector3d(5, 5, 0);
    points.col(2) = Vector3d(5, 6, 0);
    points.col(3) = Vector3d(0, 6, 0);
    P2.set_points(points);

    ids.clear();
    ids.push_back(0);ids.push_back(1);ids.push_back(2);ids.push_back(3);
    P3.set_ids(ids);
    points = MatrixXd(3, 4);
    points.col(0) = Vector3d(0, 5, 0);
    points.col(1) = Vector3d(-1, 5, 0);
    points.col(2) = Vector3d(-1, 0, 0);
    points.col(3) = Vector3d(0, 0, 0);
    P3.set_points(points);

    P.push_back(P0);
    P.push_back(P1);
    P.push_back(P2);
    P.push_back(P3);

    for(int id = 0; id < NTRI; id++)
    {
        P4.clear();
        ids.clear();
        ids.push_back(0);
        ids.push_back(1);
        ids.push_back(2);
        ids.push_back(3);
        ids.push_back(4);
        ids.push_back(5);
        P4.set_ids(ids);
        points = MatrixXd(3, 6);
        points.col(0) = Vector3d(0, 0, 0);
        points.col(1) = Vector3d(TRI_LEN, -TRI_LEN , 0);
        points.col(2) = Vector3d(TRI_LEN * 2, 0, 0);
        points.col(3) = Vector3d(TRI_LEN * 2, TRI_LEN, 0);
        points.col(4) = Vector3d(TRI_LEN , 2 * TRI_LEN, 0);
        points.col(5) = Vector3d(0, TRI_LEN, 0);
        P4.set_points(points);

        double dx = (rand() % 2300) / 500.0;
        double dy = (rand() % 2300)  / 500.0;
        double theta = (rand() % 1000) /1000.0 * 3.1415926;

        P4.Rotate_translate(Vector3d(theta, dx, dy));
        P.push_back(P4);
    }
}


void polygon()
{
    vector<PolygonPoints> plist;
    generate_polygons(plist);
    x_0 = VectorXd(0, 1);
    set_mesh(plist, viewer);
}

void generate_cube()
{
    TI_Cube cubes(1, 1);
    stdVecMatrixXd vec_V;
    stdVecMatrixXi vec_F;
    std::vector<PolyhedraPoints> Ps;
    cubes.generate_1v6(Ps);
    for(int id = 0; id < Ps.size(); id++)
    {
        MatrixXd tV;
        MatrixXi tF;
        Ps[id].triangulate(tF); vec_F.push_back(tF);
        Ps[id].get_points(tV); vec_V.push_back(tV.transpose());
    }

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

void move_point()
{
    for(int id = 0; id < 8; id++)
        V.row(id) -= Eigen::RowVector3d(0, 0, 0.1);
    viewer.data.clear();
    viewer.data.set_mesh(V, F);
}


void opt_solve()
{
    vector<PolygonPoints> plist;
    generate_polygons(plist);

    PolygonsCollisionSolver solver;
    for(int id = 4; id < NTRI + 4; id ++)
    {
        for(int jd = 0; jd < id; jd++)
        {
            solver.setConnection(id, jd);
        }
    }
    solver.setPolygons(plist);
    solver.collision_resolve(x_0);

    //two_collision_solver(plist, x_0);

    for(int id = 0; id < plist.size(); id++)
        plist[id].Rotate_translate(x_0.segment(3 * id, 3));
    set_mesh(plist, viewer);

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


    viewer.callback_init = [&](igl::viewer::Viewer& viewer)
    {
        viewer.ngui->addButton("Mosek", mosek);
        viewer.ngui->addButton("3D Cubes", generate_cube);
        viewer.ngui->addButton("2D", polygon);
        viewer.ngui->addButton("Solve", opt_solve);
        viewer.screen->performLayout();
        return false;
    };

    viewer.launch();
    return 0;
}
