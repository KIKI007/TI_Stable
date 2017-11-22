//
// Created by ziqwang on 22.11.17.
//

#ifndef TI_STABLE_TI_CUBE_H
#define TI_STABLE_TI_CUBE_H

#include <Eigen/Dense>
#include<Eigen/StdVector>

using Eigen::MatrixXd
typedef std::vector<MatrixXd,Eigen::aligned_allocator<MatrixXd> > stdVecMatrixXd;


class TI_Cube {

public:
    TI_Cube();
    TI_Cube(int m, int n);
    ~TI_Cube();
private:

    //The Cubes has m columns and n rows
    void init(int m, int n);

public:

    void generate(stdVecMatrixXd &cubes);

private:

    double cube_length_; //unit mm

    double tolerance_; //within 0 and 1, presents the percentage in shrinking

};


#endif //TI_STABLE_TI_CUBE_H
