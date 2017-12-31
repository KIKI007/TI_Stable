/* Libigl */
#include <igl/readOFF.h>
#include <igl/viewer/Viewer.h>
#include <igl/unproject_onto_mesh.h>
#include <igl/mosek/mosek_linprog.h>
#include <igl/png/writePNG.h>
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

using std::cos;
using std::sin;

igl::viewer::Viewer viewer;
MatrixXd V, C;
MatrixXi F;

VectorXd x_0(0, 1);
int TI_3D_N;
int TI_3D_M;
double TI_3D_Tolerance;
bool gap_selected(false);
bool fix_selected(false);

vector<PolyhedraPoints> polyhedra_list;
vector<PolygonPoints> polygons_list;
vector<pair<int, int>> polyons_collision_pair;

vector<pair<int,int>> polyhedrons_collision_pair;
vector<int> polyhedrons_fixed_list;

vector<int> polygons_fixed_id;
vector<int> polygons_fixed_extra_id;
pair<int, int> polygons_gap_par;

enum Selected_Mode {
    NON = 0,
    MOVE,
    FIX,
    GAP
}selected_mode(NON);

class select_record{
public:
    select_record(Selected_Mode mode = NON)
    {
        mode_ = mode;
        clear();
    }

public:

    void clear()
    {
        poly_sta_end_.clear();
        poly_id_.clear();
    }

    void add(int pid, int sta, int end)
    {
        poly_id_.push_back(pid);
        poly_sta_end_.push_back(pair<int, int> (sta, end));
        return;
    }

    void get(pair<int, int> &gap)
    {
        gap.first = gap.second = -1;
        int size = poly_id_.size();
        if(size >= 2)
        {
            gap.first = poly_id_[size - 2];
            gap.second = poly_id_[size - 1];
        }
    }

    bool get(int &pid, int &sta, int &end)
    {
        if(poly_id_.empty()) return false;
        if(mode_ == NON || mode_ == FIX) return false;
        if(mode_ == MOVE)
        {
            pid = poly_id_.back();
            sta = poly_sta_end_.back().first;
            end = poly_sta_end_.back().second;
            return true;
        }
        else if(mode_ == GAP)
        {
            int size = poly_id_.size();
            if(size >= 2)
            {
                pid = poly_id_[size - 2];
                sta = poly_sta_end_[size - 2].first;
                end = poly_sta_end_[size - 2].second;
                return true;
            }
        }
        return false;
    }

public:
    vector<int> poly_id_;
    vector<pair<int, int>> poly_sta_end_;
    Selected_Mode  mode_;
}recorder;


void generate_random_hexagons(vector<PolygonPoints> &P, vector<pair<int, int>>&Conn, vector<int> &fixeds)
{
    P.clear();
    fixeds.clear();
    Conn.clear();
    x_0.setZero();
    RandomHexagons2D generator;
    generator.createHexagons(P, Conn, fixeds);
    return;
}

void generate_random_triangles(vector<PolygonPoints> &P, vector<pair<int, int>>&Conn, vector<int> &fixeds)
{
    P.clear();
    fixeds.clear();
    Conn.clear();
    x_0.setZero();
    RandomTriangles2D generator;
    generator.createTriangles(P, Conn, fixeds);
    return;
}

void generate_random_rectangles(vector<PolygonPoints> &P, vector<pair<int, int>>&Conn, vector<int> &fixeds)
{
    P.clear();
    fixeds.clear();
    Conn.clear();
    x_0.setZero();
    RandomRectangles2D generator;
    generator.createRectangles(P, Conn, fixeds);
    return;
}

void hexagons_2d_rendering()
{
    generate_random_hexagons(polygons_list, polyons_collision_pair, polygons_fixed_id);
    x_0 = VectorXd(0, 1);
    set_mesh(polygons_list, viewer, V, F, C);
}

void rectangles_2d_rendering()
{
    generate_random_rectangles(polygons_list, polyons_collision_pair, polygons_fixed_id);
    x_0 = VectorXd(0, 1);
    set_mesh(polygons_list, viewer, V, F, C);
}

void triangles_2d_rendering()
{
    generate_random_triangles(polygons_list, polyons_collision_pair, polygons_fixed_id);
    x_0 = VectorXd(0, 1);
    set_mesh(polygons_list, viewer, V, F, C);
}

int find_polygons_from_faceid(int fid, int &sta, int &end)
{
    sta = 0;
    for(int id = 0; id < polygons_list.size(); id++)
    {
        int nF = polygons_list[id].nV() - 2;
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
    if(selected_mode == MOVE)
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
        int poly_id;
        if(recorder.get(poly_id, sta, end))
        {
            polygons_list[poly_id].do_transformation(Vector3d(0, dx, dy));
            set_mesh(polygons_list, viewer, V, F, C, false);
        }
        return true;
    }

    return false;
}

void clear_color()
{
    vector<pair<int, int>> pse = recorder.poly_sta_end_;
    vector<int> pid = recorder.poly_id_;
    int sta, end;
    for(int kd = 0; kd < pse.size(); kd++)
    {
        sta = pse[kd].first;
        end = pse[kd].second;
        for(int id = sta; id <= end; id++)
            C.row(id) = polygons_list[pid[kd]].get_color();
    }
    viewer.data.set_colors(C);
}

bool mouse_down(igl::viewer::Viewer& viewer, int button, int modifier)
{
    int fid;
    Eigen::Vector3f bc;
    // Cast a ray in the view direction starting from the mouse position
    double x = viewer.current_mouse_x;
    double y = viewer.core.viewport(3) - viewer.current_mouse_y;

    if(igl::unproject_onto_mesh(Eigen::Vector2f(x,y), viewer.core.view * viewer.core.model,
                                viewer.core.proj, viewer.core.viewport, V, F, fid, bc))
    {

        //clear last color
        int pid, sta, end;
        if(recorder.get(pid, sta, end))
        {
                for(int id = sta; id <= end; id++)
                    C.row(id) = polygons_list[pid].get_color();
        }

        //add to record
        pid = find_polygons_from_faceid(fid, sta, end);
        recorder.add(pid, sta, end);

        // paint color
        if(selected_mode == MOVE) {
            for (int id = sta; id <= end; id++)
                C.row(id) << 1, 0, 0;
        }
        else if(selected_mode == FIX)
        {
            for(int id = sta; id <= end; id++)
                C.row(id)<<  0, 0, 1;
        }
        else if(selected_mode == GAP)
        {
            for(int id = sta; id <= end; id++)
                C.row(id)<<  0, 1, 0;
        }

        viewer.data.set_colors(C);
        return true;
    }
    else
    {
        clear_color();
        recorder.clear();

    }
    return false;
}

void opt_solve()
{
    if(polygons_list.empty())
    {
        generate_random_rectangles(polygons_list, polyons_collision_pair, polygons_fixed_id);
        //generate_random_triangles(polygons_list, polyons_collision_pair);
    }

    PolygonsCollisionSolver solver;

    //set collision pair
    {
        for(int id = 0; id < polyons_collision_pair.size(); id ++)
        {
            solver.setConnection(polyons_collision_pair[id].first, polyons_collision_pair[id].second);
        }
    }

    //set fixed
    {
        for(int id = 0; id < polygons_fixed_id.size(); id++)
            solver.setFixed(polygons_fixed_id[id]);
        for(int id = 0; id < polygons_fixed_extra_id.size(); id++)
            solver.setFixed(polygons_fixed_extra_id[id]);
        polygons_fixed_extra_id.clear();
        fix_selected = false;
    }

    //set gap
    {
        solver.setGap(polygons_gap_par.first, polygons_gap_par.second);
        polygons_gap_par = pair<int, int>(-1, -1);
        gap_selected = false;
    }

    //set polygons
    solver.setPolygons(polygons_list);

    //solve
    x_0.setZero();double dx = 0;
    solver.collision_resolve(x_0, dx);

    //rendering the result
    for(int id = 0; id < polygons_list.size(); id++)
        polygons_list[id].do_transformation(x_0.segment(3 * id, 3));
    set_mesh(polygons_list, viewer, V, F, C);

}

void TI_cube_generation()
{
    TI_Cube cube(TI_3D_M, TI_3D_N);
    cube.tolerance_ = TI_3D_Tolerance;

    //create polyhedra list
    polyhedra_list.clear();
    cube.generate(polyhedra_list);

    polyhedrons_collision_pair.clear();
    polyhedrons_fixed_list.clear();

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
                polyhedrons_collision_pair.push_back(pair<int, int>(ID, ID + TI_3D_M));
                if(id & 1)
                {
                    if(jd != 0)
                        polyhedrons_collision_pair.push_back(pair<int, int>(ID, ID + TI_3D_M - 1));
                }
                else
                {
                    if(jd != TI_3D_M -1)
                        polyhedrons_collision_pair.push_back(pair<int, int>(ID, ID + TI_3D_M + 1));
                }
            }
            //right
            if(jd < TI_3D_M - 1)
            {
                polyhedrons_collision_pair.push_back(pair<int, int>(ID, ID + 1));
            }

            if(id == 0 || jd == 0 || id == TI_3D_N - 1 || jd == TI_3D_M - 1) {
                polyhedrons_fixed_list.push_back(ID);
            }
        }
    }
}

void TI_cube_rendering()
{
    TI_cube_generation();
    int Ia, Ib;
    Ia = (TI_3D_N / 2) * TI_3D_M + TI_3D_M/2 - 1;
    Ib = (TI_3D_N / 2) * TI_3D_M + TI_3D_M/2;
    polyhedra_list[Ia].set_color(Vector3d(1, 0, 0));
    polyhedra_list[Ib].set_color(Vector3d(0, 0, 1));
    set_mesh(polyhedra_list, viewer, V, F, C, true);
}

void collision_resolve()
{

    PolyhedronsCollisionSolver solver;
    if(polyhedra_list.empty()) TI_cube_generation();

    solver.setPolyhedrons(polyhedra_list);
    solver.setViewer(&viewer);

    for(auto x: polyhedrons_collision_pair) solver.setConnection(x.first, x.second);

    for(auto x : polyhedrons_fixed_list) solver.setFixed(x);

    int Ia, Ib;
    Ia = (TI_3D_N / 2) * TI_3D_M + TI_3D_M/2 - 1;
    Ib = (TI_3D_N / 2) * TI_3D_M + TI_3D_M/2;
    solver.setGap(Ia, Ib);

    double d_x = 0;
    solver.collision_resolve(x_0, d_x);

    //set color
    VectorXd zvalue(polyhedra_list.size());
    for(int id = 0; id < polyhedra_list.size(); id++)
    {
        zvalue(id) = -x_0(id * 7 + 6);
    }

    MatrixXd Cz;
    igl::jet(zvalue, true, Cz);

    for(int id = 0; id < polyhedra_list.size(); id++)
    {
        polyhedra_list[id].set_color(Cz.row(id));
        polyhedra_list[id].do_transformation(solver.vec4quat(x_0.segment(id * 7, 4)), x_0.segment(id * 7 + 4, 3));
    }

    //reset fixed color
    for(auto id : polyhedrons_fixed_list) polyhedra_list[id].set_color(Vector3d(0.4, 0.4, 0.4));

        set_mesh(polyhedra_list, viewer, V, F, C);
}


void init()
{
    srand(100);
    x_0.setZero();

    TI_3D_N = 10;
    TI_3D_M = 10;
    TI_3D_Tolerance = 0.98;
}
int main() {
    init();
    viewer.callback_init = [&](igl::viewer::Viewer& viewer)
    {
        viewer.ngui->addWindow(Eigen::Vector2i(viewer.ngui->window()->size()[0] + 20, 10));
        viewer.ngui->addGroup("2D TI Stable");
        viewer.ngui->addButton("2D Triangles", triangles_2d_rendering);
        viewer.ngui->addButton("2D Rectangles", rectangles_2d_rendering);
        viewer.ngui->addButton("2D Hexagons", hexagons_2d_rendering);
        viewer.ngui->addButton("Solve", opt_solve);
        viewer.ngui->addVariable<Selected_Mode>("Selected Mode", [&](Selected_Mode mode){
            clear_color();
            recorder = select_record(mode);
            selected_mode = mode;}, [&](){
            return selected_mode;
        }) ->setItems({"None", "Move", "Fix", "Gap"});

        viewer.ngui->addButton("Select", [&]()
        {
            if(selected_mode == FIX) {
                fix_selected = true;
                polygons_fixed_extra_id.clear();
                polygons_fixed_extra_id = recorder.poly_id_;
            }
            else if(selected_mode == GAP)
            {
                gap_selected = true;
                recorder.get(polygons_gap_par);
            }
        });
        viewer.ngui->addVariable<bool>("Fix Selected", fix_selected, false);
        viewer.ngui->addVariable<bool>("Gap Selected", gap_selected, false);
        viewer.ngui->addGroup("3D TI Stable");

        viewer.ngui->addVariable("Rows", TI_3D_N);
        viewer.ngui->addVariable("Cols", TI_3D_M);
        viewer.ngui->addVariable("Tolerance", TI_3D_Tolerance);
        viewer.ngui->addButton("3D Cubes", TI_cube_rendering);
        viewer.ngui->addButton("Collision solve", collision_resolve);

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
