//
// Created by ziqwang on 22.11.17.
//

#include "TI_Cube.h"
#include "Basic_Parameters.h"

TI_Cube::TI_Cube() {
    init(0, 0);
}

TI_Cube::~TI_Cube() {

}

void TI_Cube::init(int m, int n) {
    num_cubes_col_ = m;
    num_cubes_row_ = n;
    tolerance_ = 0.95;
    cube_length_ = 1;
}


void TI_Cube::generate(stdVecMatrixXd &cVs, stdVecMatrixXi &cFs) {

    MatrixXd mat(3, 3);

    Vector3d X, Y, Z;
    Vector3d O, O1, X1, Y1, Z1;
    O << 0, 0, 0;
    X << 1, 0, 0;
    Y << 0, 1, 0;
    Z << 0, 0, 1;

    O1 << 0.5, 0.5, 0.5;
    X1 << 0.5, 0.5, -1; X1.normalize();
    Y1 << -0.5, 0.5, 0; Y1.normalize();
    Z1 = X1.cross(Y1);  Z1.normalize();

    mat.col(0) = X1.transpose();
    mat.col(1) = Y1.transpose();
    mat.col(2) = Z1.transpose();

    mat = mat.inverse();

    X = mat * X;
    Y = mat * Y;
    Z = mat * Z;

    for(int i = 0; i < num_cubes_row_; i++)
    {
        for(int j = 0; j< num_cubes_col_; j++)
        {
            double r = cube_length_ * sqrt(2) / 2;
            Vector3d O(j * sqrt(3) * r + (-1) * (i&1) * sqrt(3)/ 2.0 * r,
                       i * 1.5 * r,
                       0);
            MatrixXd cV; MatrixXi cF;
            cube_OXYZ(O,
                      X * cube_length_ * tolerance_,
                      Y * cube_length_ * tolerance_,
                      Z * cube_length_ * tolerance_,
                      cV,
                      cF);
            cVs.push_back(cV);
            cFs.push_back(cF);
        }
    }
}



TI_Cube::TI_Cube(int m, int n) {
    init(m, n);
}

void TI_Cube::cube_OXYZ(Vector3d O, Vector3d X, Vector3d Y, Vector3d Z, MatrixXd &cV, MatrixXi &cF) {

    //non-zero vectors
    assert(X.norm() > TI_zero_eps_ && Y.norm() > TI_zero_eps_ && Z.norm() > TI_zero_eps_);

    //perpendicular
    assert(std::abs(X.dot(Y)) < TI_zero_eps_
           && std::abs(Y.dot(Z)) < TI_zero_eps_
           && std::abs(Z.dot(X)) < TI_zero_eps_ );

    cV = MatrixXd::Zero(8, 3);
    cF = MatrixXi::Zero(12, 3);

    cV.row(0) = O;

    cV.row(1) = (O + X);
    cV.row(2) = (O + Y);
    cV.row(3) = (O + Z);
    cV.row(4) = (O + X + Y);
    cV.row(5) = (O + X + Z);
    cV.row(6) = (O + Y + Z);
    cV.row(7) = (O + X + Y + Z);

    cF <<   0, 2, 4,
            4, 1, 0,
            1, 4, 7,
            7, 5, 1,
            1, 5, 3,
            3, 0, 1,
            0, 3, 6,
            6, 2, 0,
            3, 5, 7,
            7, 6, 3,
            2, 6, 7,
            7, 4, 2;
}
