/* Libigl */
#include <igl/readOFF.h>
#include <igl/viewer/Viewer.h>
#include <igl/unproject_onto_mesh.h>
#include <igl/mosek/mosek_linprog.h>
#include <igl/png/writePNG.h>
#include <cstring>
#include <igl/file_dialog_open.h>
#include <igl/jet.h>
#include <ctime>


/* system */
#include <sstream>
#include <iostream>
#include <cmath>

#include "TI_Cube.h"
#include "ObjectsRendering.h"
#include "PolygonsCollisionSolver.h"
#include "PolyhedronsCollisionSolver.h"
#include "RandomTriangles2D.h"
#include "RandomRectangles2D.h"
#include "RandomHexagons2D.h"
#include "TopoLockPolyhedrons.h"

using std::cos;
using std::sin;

igl::viewer::Viewer viewer;

MatrixXd V, C;
MatrixXi F;

int Index_Z = 2;

int TI_3D_N;
int TI_3D_M;

bool show_poly_id;
bool show_poly_values;
double TI_3D_Tolerance;
float tex;
float title;
VectorXd x_0(0, 1);

vector<PolyhedraPoints> polyhedra_list;
vector<pair<int,int>> collision_pair;
vector<int> fixed_list;
pair<int, int> gap_pair;

char in_path[100] = "";

void TI_cube_generation()
{
    TI_Cube cube(TI_3D_M, TI_3D_N);
    cube.tolerance_ = TI_3D_Tolerance;

    //create polyhedra list
    polyhedra_list.clear();
    cube.generate(polyhedra_list);

    collision_pair.clear();
    fixed_list.clear();

    x_0.setZero();
    //create pair
    for(int id = 0; id < TI_3D_N; id++)
    {
        for(int jd = 0; jd < TI_3D_M; jd++)
        {
            int ID = id * TI_3D_M + jd;
            //top
            if(id < TI_3D_N - 1)
            {
                collision_pair.push_back(pair<int, int>(ID, ID + TI_3D_M));
                if(id & 1)
                {
                    if(jd != 0)
                        collision_pair.push_back(pair<int, int>(ID, ID + TI_3D_M - 1));
                }
                else
                {
                    if(jd != TI_3D_M -1)
                        collision_pair.push_back(pair<int, int>(ID, ID + TI_3D_M + 1));
                }
            }
            //right
            if(jd < TI_3D_M - 1)
            {
                collision_pair.push_back(pair<int, int>(ID, ID + 1));
            }

            if(id == 0 || jd == 0 || id == TI_3D_N - 1 || jd == TI_3D_M - 1) {
                fixed_list.push_back(ID);
            }
        }
    }
}

void TI_cube_rendering()
{
    void ShowPolyhedronsID(void);
    TI_cube_generation();
    int Ia, Ib;
    Ia = (TI_3D_N / 2) * TI_3D_M + TI_3D_M/2 - 1;
    Ib = (TI_3D_N / 2) * TI_3D_M + TI_3D_M/2;
    polyhedra_list[Ia].set_color(Vector3d(1, 0, 0));
    polyhedra_list[Ib].set_color(Vector3d(0, 0, 1));
    set_mesh(polyhedra_list, viewer, V, F, C, true);
    Index_Z = 2;
    ShowPolyhedronsID();
}

void collision_resolve()
{

    PolyhedronsCollisionSolver solver;
    if(polyhedra_list.empty()) TI_cube_generation();

    solver.setPolyhedrons(polyhedra_list);
    solver.setViewer(&viewer);

    for(auto x: collision_pair) solver.setConnection(x.first, x.second);

    for(auto x : fixed_list) solver.setFixed(x);

    int Ia, Ib;
    Ia = (TI_3D_N / 2) * TI_3D_M + TI_3D_M/2 - 1;
    Ib = (TI_3D_N / 2) * TI_3D_M + TI_3D_M/2;
    solver.setGap(Ia, Ib);

    double d_x = 0;
    solver.collision_resolve(x_0, d_x);

//    //set color
//    VectorXd zvalue(polyhedra_list.size());
//    for(int id = 0; id < polyhedra_list.size(); id++)
//    {
//        Vector3d center;
//        polyhedra_list[id].get_center(center);
//        zvalue(id) = -x_0(id * 7 + 5) - center[1];
//    }
//
//    MatrixXd Cz;
//    igl::jet(zvalue, true, Cz);
//
    for(int id = 0; id < polyhedra_list.size(); id++)
    {
        //polyhedra_list[id].set_color(Cz.row(id));
        polyhedra_list[id].do_transformation(solver.vec4quat(x_0.segment(id * 7, 4)), x_0.segment(id * 7 + 4, 3));
    }
//
//    //reset fixed color
//    for(auto id : fixed_list) polyhedra_list[id].set_color(Vector3d(0.4, 0.4, 0.4));
//
//        set_mesh(polyhedra_list, viewer, V, F, C);
}

void generate_TI_surface()
{
    if(in_path[0] == '\0') return;
    polyhedra_list.clear();
    fixed_list.clear();
    TopoLockPolyhedrons tplocker(TI_3D_Tolerance);
    tplocker.loadSurface(in_path);
    tplocker.setPara(tex, title);
    tplocker.createPolyhedrons(polyhedra_list);

    int Ia = gap_pair.first;
    int Ib = gap_pair.second;
    polyhedra_list[Ia].set_color(Vector3d(1, 0, 0));

    polyhedra_list[Ib].set_color(Vector3d(0, 0, 1));

    set_mesh(polyhedra_list, viewer, V, F, C);
    Index_Z = 1;
    void ShowPolyhedronsID(void);
    ShowPolyhedronsID();
    return;
}

void gap_enlarge_TI_surface()
{

    if(in_path[0] == '\0') return;

    TopoLockPolyhedrons tplocker(TI_3D_Tolerance);
    tplocker.loadSurface(in_path);
    tplocker.setPara(tex, title);
    tplocker.createPolyhedrons(polyhedra_list);

    PolyhedronsCollisionSolver solver;
    solver.setViewer(&viewer);
    solver.setPolyhedrons(polyhedra_list);

    tplocker.set_solver(solver);

    int Ia = gap_pair.first;
    int Ib = gap_pair.second;
    solver.setGap(Ia, Ib);

    double d_x = 0;
    x_0.setZero();
    solver.collision_resolve(x_0, d_x);
    solver.get_polys(polyhedra_list);

//    //set color
//    VectorXd zvalue(polyhedra_list.size());
//    for(int id = 0; id < polyhedra_list.size(); id++)
//    {
//        Vector3d center;
//        polyhedra_list[id].get_center(center);
//        zvalue(id) = -x_0(id * 7 + 6) - center[1];
//    }
//
//    MatrixXd Cz;
//    igl::jet(zvalue, true, Cz);
//
    for(int id = 0; id < polyhedra_list.size(); id++)
    {
        //polyhedra_list[id].set_color(Cz.row(id));
        polyhedra_list[id].do_transformation(solver.vec4quat(x_0.segment(id * 7, 4)), x_0.segment(id * 7 + 4, 3));
    }
//
//    //reset fixed color
//    solver.get_fixed_list(fixed_list);
//    for(auto id : fixed_list) polyhedra_list[id].set_color(Vector3d(0.4, 0.4, 0.4));
//
//    set_mesh(polyhedra_list, viewer, V, F, C);
    return;
}

void init()
{
    srand(100);
    x_0.setZero();

    TI_3D_N = 10;
    TI_3D_M = 10;
    TI_3D_Tolerance = 0.98;

    title =  35.264389;

    gap_pair.first = 0;
    gap_pair.second = 1;
    tex = 8;

    show_poly_id = false;

    in_path[0] = '\0';
}

void ShowPolyhedronsID()
{
    if(show_poly_id)
    {

        viewer.data.labels_strings.clear();
        viewer.data.labels_positions = MatrixXd(0, 0);

        for(int id = 0; id < polyhedra_list.size(); id++)
        {
            int maxID = 0;
            for(int jd = 1; jd < polyhedra_list[id].nV(); jd++)
            {
                if(polyhedra_list[id].point(jd)(Index_Z) > polyhedra_list[id].point(maxID)(Index_Z))
                    maxID = jd;
            }
                viewer.data.add_label(polyhedra_list[id].point(maxID), std::to_string(id));
        }
    }
    else
    {
        viewer.data.labels_strings.clear();
        viewer.data.labels_positions = MatrixXd(0, 0);
    }
    return;
}

void ShowPolyhedronsValues()
{
    if(show_poly_values)
    {
        viewer.data.labels_strings.clear();
        viewer.data.labels_positions = MatrixXd(0, 0);

        for(int id = 0; id < polyhedra_list.size(); id++)
        {

            int maxID = 0;
            for(int jd = 1; jd < polyhedra_list[id].nV(); jd++)
            {
                if(polyhedra_list[id].point(jd)(Index_Z) > polyhedra_list[id].point(maxID)(Index_Z))
                    maxID = jd;
            }

            if(x_0.size() > id * 7 + 6)
            {
                double dist = std::sqrt((x_0(id * 7 + 4) * x_0(id * 7 + 4)
                               + x_0(id * 7 + 5) * x_0(id * 7 + 5)
                               + x_0(id * 7 + 6) * x_0(id * 7 + 6)));
                std::ostringstream out; out << std::setprecision(2) << dist;
                viewer.data.add_label(polyhedra_list[id].point(maxID), out.str());
            }
        }
    }
    else
    {
        viewer.data.labels_strings.clear();
        viewer.data.labels_positions = MatrixXd(0, 0);
    }
    return;
}

void open_file()
{
    string path =  igl::file_dialog_open();
    std::strcpy(in_path, path.c_str());
    if(path != "") {
        generate_TI_surface();
    }
    return;
}

int main(int argc, char** argv){
    init();
    viewer.callback_init = [&](igl::viewer::Viewer& viewer)
    {
        viewer.ngui->addVariable<bool>("Poly ID", [&](bool value){
            show_poly_id = value; ShowPolyhedronsID();}, [&](){
            return show_poly_id;
        });
        viewer.ngui->addVariable<bool>("Poly Value", [&](bool value){
            show_poly_values = value; ShowPolyhedronsValues();}, [&](){
            return show_poly_values;
        });

        viewer.ngui->addWindow(Eigen::Vector2i(viewer.ngui->window()->size()[0] + 20, 10));

        viewer.ngui->addGroup("Tolerance");
        viewer.ngui->addVariable("Tolerance", TI_3D_Tolerance);

        viewer.ngui->addGroup("3D Cubes");
        viewer.ngui->addVariable("Rows", TI_3D_N);
        viewer.ngui->addVariable("Cols", TI_3D_M);
        viewer.ngui->addButton("Create Cubes", TI_cube_rendering);
        viewer.ngui->addButton("Gap Enlarge", collision_resolve);

        viewer.ngui->addGroup("3D TI");
        viewer.ngui->addVariable<int>("gapA", gap_pair.first);
        viewer.ngui->addVariable<int>("gapB", gap_pair.second);
        viewer.ngui->addVariable<float>("texViewSize", tex);
        viewer.ngui->addVariable<float>("tiltTheta", title);
        viewer.ngui->addButton("Open File", open_file);
        viewer.ngui->addButton("Generate", generate_TI_surface);
        viewer.ngui->addButton("Analysis", gap_enlarge_TI_surface);

        viewer.ngui->addGroup("Screen Capture");
        viewer.ngui->addButton("capture", [&](){
            Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> R(5120,2880);
            Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> G(5120,2880);
            Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> B(5120,2880);
            Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> A(5120,2880);
            // Draw the scene in the buffers
            viewer.core.draw_buffer(viewer.data,viewer.opengl,false,R,G,B,A);
            // Save it to a PNG
            std::time_t T = std::time(nullptr);
            std::string time_str = std::asctime(std::localtime(&T));
            time_str = "../img/" + time_str + ".png";
            std::cout << "Write image to " << time_str << std::endl;
            igl::png::writePNG(R,G,B,A, time_str);
        });

        viewer.screen->performLayout();
        return false;
    };
    viewer.launch();
    return 0;
}
