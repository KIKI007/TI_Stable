//
// Created by ziqwang on 28.12.17.
//

#ifndef TI_STABLE_POLYHEDRONSCOLLISIONSOLVER_H
#define TI_STABLE_POLYHEDRONSCOLLISIONSOLVER_H


#include <vector>
#include <map>
#include <Eigen/Sparse>

#include "igl/mosek/mosek_linprog.h"
#include "PolyhedraPoints.h"
#include "Timer_evaluation.h"
#include <igl/viewer/Viewer.h>

using std::vector;
using Eigen::VectorXd;
using Eigen::Vector4d;
typedef vector<VectorXd,Eigen::aligned_allocator<VectorXd> > vecVectorXd;
typedef Eigen::Triplet<double> T;
typedef vector<PolyhedraPoints> vecPlyhdrons;

class PolyhedronsCollisionSolver {
public:

    PolyhedronsCollisionSolver();

public:

    void setPolyhedrons(vecPlyhdrons &P);

    void setConnection(int a, int b);

    void setFixed(int a);

    void setGap(int a, int b);

    void setViewer(igl::viewer::Viewer *viewer){viewer_ = viewer;};

public:

    void collision_resolve(VectorXd &x, double &dx);

    Quaterniond vec4quat(Vector4d u);

    Quaterniond vec3quat(Vector3d u);

private:

    void get_g_coeff(vecVectorXd &coeff_g, vecPlyhdrons &P, double &f_xk);

    bool solve_linear(int n,
                      int m,
                      vector<T> &triplist,
                      vector<double> &vec_c,
                      vector<double> &vec_lc,
                      vector<double> &vec_lx,
                      vector<double> &vec_ux,
                      VectorXd &x,
                      double &mk_pk);

    void set_mesh(vector<PolyhedraPoints> &P,
                  igl::viewer::Viewer &viewer,
                  MatrixXd &V,
                  MatrixXi &F,
                  MatrixXd &C,
                  bool change_color);
    //double signed_distance(vecPolys p, VectorXd &x0, VectorXd &x);
private:

    vecPlyhdrons P_;

    std::vector<std::pair<int, int>> Conn_;

    std::vector<int> fixed_;

    std::pair<int, int> gap_;

    igl::viewer::Viewer *viewer_;
};


#endif //TI_STABLE_POLYHEDRONSCOLLISIONSOLVER_H
