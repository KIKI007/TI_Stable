//
// Created by ziqwang on 08.12.17.
//

#ifndef TI_STABLE_POLYGONSCOLLISIONSOLVER_H
#define TI_STABLE_POLYGONSCOLLISIONSOLVER_H

#include "PolygonPoints.h"
#include <vector>
#include <Eigen/Sparse>
#include "igl/mosek/mosek_linprog.h"
#include <map>
typedef  std::vector<PolygonPoints> vecPolys;
using Eigen::VectorXd;
typedef Eigen::Triplet<double> T;
using std::vector;
class PolygonsCollisionSolver {

public:
    PolygonsCollisionSolver();

public:

    void setPolygons(vecPolys &P);

    void setConnection(int a, int b);

public:

    void collision_resolve(VectorXd &x, double &dx);

private:

    bool get_a_coeff(VectorXd &a, vecPolys &P_ORI, VectorXd &x, double &f_xk);

    double penetration_distance(vecPolys p, VectorXd &x);

    bool solve_linear(std::vector<T> &triplist,
                      std::vector<double> &Ac,
                      std::vector<bool> &in_opt,
                      VectorXd &x0,
                      VectorXd &x,
                      int id_term,
                      int id_extra,
                      double dx,
                      double &mk_pk);
private:
    vecPolys P_;

    std::vector<std::pair<int, int>> Conn_;
};


#endif //TI_STABLE_POLYGONSCOLLISIONSOLVER_H
