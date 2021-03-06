//
// Created by ziqwang on 22.11.17.
//

#ifndef TI_STABLE_TI_CUBE_H
#define TI_STABLE_TI_CUBE_H
const double TI_zero_eps_ = 1e-6;
#include <Eigen/Dense>
#include<Eigen/StdVector>
#include "PolyhedraPoints.h"
using Eigen::MatrixXd;
using Eigen::MatrixXi;
using Eigen::VectorXd;
using Eigen::VectorXi;
using Eigen::Vector3d;
typedef std::vector<MatrixXd,Eigen::aligned_allocator<MatrixXd> > stdVecMatrixXd;
typedef std::vector<MatrixXi,Eigen::aligned_allocator<MatrixXi> > stdVecMatrixXi;
class TI_Cube {

public:
    TI_Cube();
    TI_Cube(int m, int n);
    ~TI_Cube();
private:

    //The Cubes has m columns and n rows
    void init(int m, int n);

public:

    void generate_1v6(std::vector<PolyhedraPoints> &plist);

    void generate(std::vector<PolyhedraPoints> &plist);

    void cube_OXYZ(Vector3d O, Vector3d X, Vector3d Y, Vector3d Z, PolyhedraPoints &P);

    void shrink(MatrixXd &mat, double ratio);

private:

    double cube_length_; //unit mm

    double tolerance_; //within 0 and 1, presents the percentage in shrinking

    int num_cubes_col_;

    int num_cubes_row_;
};


#endif //TI_STABLE_TI_CUBE_H
