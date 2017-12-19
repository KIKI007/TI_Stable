//
// Created by ziqwang on 07.12.17.
//

#ifndef TI_STABLE_OPTSOLVER_H
#define TI_STABLE_OPTSOLVER_H

#include "PolygonPoints.h"
#include "PolyhedraPoints.h"
#include <igl/viewer/Viewer.h>
#include <igl/mosek/mosek_linprog.h>
using Eigen::VectorXd;
using std::vector;
typedef std::vector<MatrixXd,Eigen::aligned_allocator<MatrixXd> > stdVecMatrixXd;
typedef std::vector<MatrixXi,Eigen::aligned_allocator<MatrixXi> > stdVecMatrixXi;
typedef Eigen::Triplet<double> T;
void solve_linear(VectorXd &x_0, VectorXd &a, double dx, VectorXd &x, double &mk_pk)
{
    Eigen::VectorXd c(8);
    c << 0, 0 ,0, 0, 0, 0, 1, 1;

    //for A
    Eigen::SparseMatrix<double> A(1, 8);
    std::vector<T> triplist;
    for(int id = 0; id < 6 ;id ++)
    {
        triplist.push_back(T(0, id, -a(id + 1)));
    }
    triplist.push_back(T(0, 6, 1));
    triplist.push_back(T(0, 7, -1));
    A.setFromTriplets(triplist.begin(), triplist.end());

    //for lc, uc
    Eigen::VectorXd lc(1);
    lc << a(0);
    Eigen::VectorXd uc(1);
    uc << a(0);

    //for lx, ux
    Eigen::VectorXd lx(8), ux(8);
    for(int id = 0; id < 8 ;id++)
    {
        if(0)
        {
            lx(id) = x_0(id);
            ux(id) = x_0(id);
        }
        else if(id >= 6)
        {
            lx(id) = 0;
            ux(id) = std::numeric_limits<double>::infinity();
        }
        else
        {
            {
                lx(id) = x_0(id) - dx;
                ux(id) = x_0(id) + dx;
            }
        }
    }

    Eigen::VectorXd ans(8);
    bool sucess = igl::mosek::mosek_linprog(c, A, lc, uc, lx, ux, ans);

    for(int id = 0; id < 6; id++)
    {
        x(id) = ans(id);
    }

    mk_pk = ans(6) + ans(7);
}

void two_collision_solver(vector<PolygonPoints> &p, VectorXd &x0)
{
    //get the coefficient of penetration distance
    bool get_a_coeff(VectorXd &a, vector<PolygonPoints> &p, VectorXd &x, double &f_xk);
    double penetration_distance(vector<PolygonPoints> p, VectorXd &x);

    //parameter for trust region algorithm
    const int MAX_ITER_TIMES = 100;
    const double MAX_TRUST_REGION_SIZE  = 0.1;
    const double MIN_TRUST_REGION_SIZE  = 1e-8;
    const double ACCEPT_RATIO           = 0;
    const double INIT_TRUST_REGION_SIZE = 0.03;

    int         iter_times  = 0;
    double      dx          = INIT_TRUST_REGION_SIZE;   //trust region size
    double      mk_0, mk_pk, f_xk, f_xkpk;              //function and model's improvement
    //x0 = VectorXd::Zero(6);

    while(iter_times < MAX_ITER_TIMES && dx > MIN_TRUST_REGION_SIZE)
    {
        iter_times++;

        VectorXd a;
        if(!get_a_coeff(a, p, x0, f_xk))
            break;

        mk_0 = f_xk;

        VectorXd x(6); solve_linear(x0, a, dx, x, mk_pk);

        f_xkpk = penetration_distance(p, x);

        //output information
        double pho = (f_xk - f_xkpk) / (mk_0 - mk_pk);
        std::cout << "Iter:\t" << iter_times << std::endl
                  << "dx:\t" << (x - x0).transpose() << std::endl
                  << "x:\t" <<   x0.transpose() << std::endl
                  << "f_xkpk:\t" << f_xkpk << std::endl
                  << "pho:\t" << pho << std::endl
                  << "dx:\t" << dx << std::endl << std::endl;

        //trust region expanding
        if(pho < 0.5)
        {
            dx = dx * 0.25;
        }
        else
        {
            if(pho > 0.75 && (x - x0).norm() >= dx - MIN_TRUST_REGION_SIZE)
            {
                dx = std::min(2 * dx, MAX_TRUST_REGION_SIZE);
            }
        }
        if(pho > ACCEPT_RATIO)
        {
            x0 = x;
        }
    }
}



bool get_a_coeff(VectorXd &a, vector<PolygonPoints> &P_ORI, VectorXd &x, double &f_xk)
//a is the coefficient of penetration distance function
//P_ORI is two collision polygons in its origin position
//x is present rotation and translation
    //x(0 + 3 * i) is theta (rotation angle) in Pi
    //x(1 + 3 * i) is x (translation in x axis) in Pi
    //x(2 + 3 * i) is y (translation in y axis) in Pi
//f_xk is the penetration distance
{
    Vector3d x0[2];
    x0[0] = x.segment(0, 3);
    x0[1] = x.segment(3, 3);

    Vector3d ct_ori[2];
    P_ORI[0].get_center(ct_ori[0]);
    P_ORI[1].get_center(ct_ori[1]);

    PolygonPoints P[2];
    P[0] = P_ORI[0];
    P[0].do_transformation(x0[0]);
    P[1] = P_ORI[1];
    P[1].do_transformation(x0[1]);

    /************ collision info **************/

    int Ia, Ib;
    bool na;
    double mu;
    Vector3d n, p[2], p_ori[2];
    if(!P[0].collision(P[1], n, na, Ia, Ib))
        return false;

    p[0]        = P[0].point(Ia);
    p[1]        = P[1].point(Ib);
    p_ori[0]    = P_ORI[0].point(Ia);
    p_ori[1]    = P_ORI[1].point(Ib);

    f_xk = std::abs(n.dot(p[0] - p[1]));

    /****** penetration distance info *********/
    a = VectorXd::Zero(7);

    //add n1
    double theta_na = (na ? x(0) : x(3));
    double extra = 0;
    Vector3d n1(-n(1), n(0), 0);
    for(int id = 0; id < 2; id++)
    {
        double theta_0 = x(3 * id);
        double x_0 = x(3 * id + 1);
        double y_0 = x(3 * id + 2);
        MatrixXd dR_theta(3, 3);
        dR_theta << -std::sin(theta_0)    ,   -std::cos(theta_0)   ,     0,
                     std::cos(theta_0)    ,   -std::sin(theta_0)   ,     0,
                           0                ,                      0   , 1;

        int sign = (id & 1 ? -1 : 1);
        a(3 * id + 1) = n.dot(dR_theta * (p_ori[id] - ct_ori[id])) * sign;

        //a2
        a(3 * id + 2) = n(0) * sign;

        //a3
        a(3 * id + 3) = n(1) * sign;

        //a0
        a(0) +=  (sign) * (n. dot(dR_theta * (p_ori[id] - ct_ori[id])) * (-theta_0) - n (0) * x_0 -  n(1) * y_0);
    }
    a(0)  += (n - theta_na * n1).dot(p[0] - p[1]) ;
    extra += n1.dot(p[0] - p[1]);

    if(na) a(1) += extra;
    else   a(4) += extra;
    return true;
}

double penetration_distance(vector<PolygonPoints> p, VectorXd &x)
{
    Vector3d x0 = x.segment(0, 3);
    Vector3d x1 = x.segment(3, 3);
    p[0].do_transformation(x0);
    p[1].do_transformation(x1);

    Vector3d n;
    int Ia, Ib;
    bool na;
    p[0].collision(p[1], n, na, Ia, Ib);
    return std::abs(n.dot(p[0].point(Ia) - p[1].point(Ib)));
}

typedef std::vector<Eigen::RowVector3d,Eigen::aligned_allocator<Eigen::RowVector3d> > VecRowVector3d;

void color_list(VecRowVector3d &colors, int n)
{
    colors.push_back(Eigen::RowVector3d((rand()%255)/255.0,(rand()%255)/255.0,(rand()%255)/255.0));
    for(int id = 1; id < n; id++)
    {
        Eigen::RowVector3d color;
        while(1)
        {
            color = (Eigen::RowVector3d((rand()%255)/255.0,(rand()%255)/255.0,(rand()%255)/255.0));
            int jd = 0;
            for(; jd < id; jd++)
            {
//                if((color - colors[jd]).norm() < 0.2)
//                    break;
            }
            if(jd == id)
                break;
        }
        colors.push_back(color);
    }
}

void set_mesh(vector<PolygonPoints> &P, igl::viewer::Viewer &viewer, MatrixXd &V, MatrixXi &F, MatrixXd &C, bool change_color = true)
{
    if(change_color) C.setZero();
    V.setZero();
    F.setZero();

    int nV = 0, nF = 0;
    vector<int> sum_nV;
    MatrixXd tV;
    MatrixXi tF;

    for(int id = 0; id < P.size(); id++)
    {
        nV += P[id].nV();
        P[id].triangulate(tF);
        nF += tF.rows();

        int prev_num_vertices = 0;
        if(id > 0) prev_num_vertices = sum_nV[id - 1] + P[id - 1].nV();
        sum_nV.push_back(prev_num_vertices);
    }

    V = MatrixXd(nV, 3);
    F = MatrixXi::Zero(nF, 3);
    if(change_color) C = MatrixXd::Zero(nF, 3);
    int ID = 0;
    for(int id = 0; id < P.size(); id++)
    {
        for(int jd = 0; jd < P[id].nV(); jd++)
        {
            V.row(ID++) = P[id].point(jd);
        }
    }

    ID = 0;
    VecRowVector3d colors;
    color_list(colors, P.size());
    for(int id = 0; id < P.size(); id++)
    {
        P[id].triangulate(tF);
        for (int jd = 0; jd < tF.rows(); jd++) {
            F.row(ID) = tF.row(jd) + Eigen::RowVector3i(sum_nV[id], sum_nV[id], sum_nV[id]);
            if(change_color) C.row(ID) = P[id].get_color();
            ID++;
        }
    }
    viewer.data.clear();
    viewer.data.set_mesh(V, F);
    viewer.data.set_colors(C);
    viewer.core.align_camera_center(V, F);
}

void set_mesh(vector<PolyhedraPoints> &P, igl::viewer::Viewer &viewer, MatrixXd &V, MatrixXi &F, MatrixXd &C, bool change_color = true)
{
    if(change_color) C.setZero();
    V.setZero();
    F.setZero();

    int nV = 0, nF = 0;
    vector<int> sum_nV;
    MatrixXd tV;
    MatrixXi tF;

    for(int id = 0; id < P.size(); id++)
    {
        nV += P[id].nV();
        P[id].triangulate(tF);
        nF += tF.rows();

        int prev_num_vertices = 0;
        if(id > 0) prev_num_vertices = sum_nV[id - 1] + P[id - 1].nV();
        sum_nV.push_back(prev_num_vertices);
    }

    V = MatrixXd(nV, 3);
    F = MatrixXi::Zero(nF, 3);
    if(change_color) C = MatrixXd::Zero(nF, 3);
    int ID = 0;
    for(int id = 0; id < P.size(); id++)
    {
        for(int jd = 0; jd < P[id].nV(); jd++)
        {
            V.row(ID++) = P[id].point(jd);
        }
    }

    ID = 0;
    for(int id = 0; id < P.size(); id++)
    {
        P[id].triangulate(tF);
        for (int jd = 0; jd < tF.rows(); jd++) {
            F.row(ID) = tF.row(jd) + Eigen::RowVector3i(sum_nV[id], sum_nV[id], sum_nV[id]);
            if(change_color) C.row(ID) = P[id].get_color(0);
            ID++;
        }
    }
    viewer.data.clear();
    viewer.data.set_mesh(V, F);
    viewer.data.set_colors(C);
    //viewer.core.align_camera_center(V, F);
}
#endif //TI_STABLE_OPTSOLVER_H
