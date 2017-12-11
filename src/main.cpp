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
#include "RandomTriangles2D.h"
#include <igl/unproject_onto_mesh.h>

using Eigen::MatrixXd;
using Eigen::MatrixXi;
igl::viewer::Viewer viewer;
//MatrixXd V(13, 3);
//MatrixXi F(7, 3);
MatrixXd V, C;
MatrixXi F;
typedef Eigen::Triplet<double> T;

int latex_select;

VectorXd x_0(0, 1);
double dx;
const int NTRI = 19;
const double TRI_LEN = 0.5;
vector<PolygonPoints> plist;
bool selected;
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

void generate_random_triangles(vector<PolygonPoints> &P)
{
    P.clear();
    latex_select = -1;

    RandomTriangles2D generator;
    generator.createTriangles(P);
    return;
}

void generate_polygons(vector<PolygonPoints> &P)
{
    P.clear();
    latex_select = -1;

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
        P4.set_color_random();

        P.push_back(P4);
    }
}


void polygon()
{
    //generate_polygons(plist);
    generate_random_triangles(plist);
    x_0 = VectorXd(0, 1);
    set_mesh(plist, viewer, V, F, C);
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

int find_polygons_from_faceid(int fid, int &sta, int &end)
{
    sta = 0;
    for(int id = 0; id < plist.size(); id++)
    {
        int nF = plist[id].nV() - 2;
        if(fid < nF) {
            end = sta + nF - 1;
            return id;
        }
        else {
            fid -= nF;
            sta += nF;
        }
    }
    return -1;
}

bool key_down(igl::viewer::Viewer& viewer, unsigned char key, int modifier)
{
    if(selected && latex_select != -1)
    {
        double dx = 0, dy = 0;
        if (key == 'W')
        {
            dy += 0.1;
        }
        else if (key == 'S')
        {
            dy -= 0.1;
        }
        else if(key == 'A')
        {
            dx -= 0.1;
        }
        else if(key == 'D')
        {
            dx += 0.1;
        }

        int sta, end;
        int poly_id = find_polygons_from_faceid(latex_select, sta, end);
        plist[poly_id].Rotate_translate(Vector3d(0, dx, dy));
        set_mesh(plist, viewer, V, F, C, false);
        return true;
    }

    return false;
}

bool mouse_down(igl::viewer::Viewer& viewer, int button, int modifier)
{
    if(!selected) return false;
    int fid;
    Eigen::Vector3f bc;
    // Cast a ray in the view direction starting from the mouse position
    double x = viewer.current_mouse_x;
    double y = viewer.core.viewport(3) - viewer.current_mouse_y;
    if(igl::unproject_onto_mesh(Eigen::Vector2f(x,y), viewer.core.view * viewer.core.model,
                                viewer.core.proj, viewer.core.viewport, V, F, fid, bc))
    {
        // paint hit red
        if(latex_select != -1)
        {
            int sta, end;
            int poly_id = find_polygons_from_faceid(latex_select, sta, end);
            for(int id = sta; id <= end; id++) C.row(id) = plist[poly_id].get_color();
        }
        int sta, end;
        int poly_id = find_polygons_from_faceid(fid, sta, end);
        for(int id = sta; id <= end; id++) C.row(id)<<1,0,0;
        latex_select = fid;
        viewer.data.set_colors(C);
        return true;
    }
    return false;
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
    if(plist.empty()) generate_random_triangles(plist);
    PolygonsCollisionSolver solver;
    for(int id = 4; id < plist.size(); id ++)
    {
        for(int jd = 0; jd < id; jd++)
        {
            solver.setConnection(id, jd);
        }
    }
    x_0.setZero();
    solver.setPolygons(plist);
    solver.collision_resolve(x_0, dx);

    //two_collision_solver(plist, x_0);

    for(int id = 0; id < plist.size(); id++)
        plist[id].Rotate_translate(x_0.segment(3 * id, 3));
    set_mesh(plist, viewer, V, F, C);

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

    latex_select = -1;

    srand(100);
    viewer.callback_init = [&](igl::viewer::Viewer& viewer)
    {
//        viewer.ngui->addButton("Mosek", mosek);
//        viewer.ngui->addButton("3D Cubes", generate_cube);
        viewer.ngui->addButton("2D Pattern", polygon);
        viewer.ngui->addButton("Solve", opt_solve);
        viewer.ngui->addVariable("Selected", selected);
        viewer.screen->performLayout();
        return false;
    };

    viewer.callback_mouse_down = &mouse_down;
    viewer.callback_key_down = &key_down;

    viewer.launch();
    return 0;
}
