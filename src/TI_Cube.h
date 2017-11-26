//
// Created by ziqwang on 22.11.17.
//

#ifndef TI_STABLE_TI_CUBE_H
#define TI_STABLE_TI_CUBE_H

#include <Eigen/Dense>
#include<Eigen/StdVector>

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

    void generate(stdVecMatrixXd &cVs, stdVecMatrixXi &cFs);

    void cube_OXYZ(Vector3d O, Vector3d X, Vector3d Y, Vector3d Z, MatrixXd &cV, MatrixXi &cF);

private:

    double cube_length_; //unit mm

    double tolerance_; //within 0 and 1, presents the percentage in shrinking

    int num_cubes_col_;

    int num_cubes_row_;
};


#endif //TI_STABLE_TI_CUBE_H
