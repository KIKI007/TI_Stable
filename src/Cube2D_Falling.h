//
// Created by ziqwang on 25.11.17.
//

#ifndef TI_STABLE_CUBE2D_FALLING_H
#define TI_STABLE_CUBE2D_FALLING_H

#include <Eigen/Dense>

/* ShapeOp */
#include <Solver.h>
#include <Constraint.h>
#include <Force.h>

#include "OneSideCollisionConstrain.h"

using Eigen::MatrixXd;

class Cube2D_Falling {
public:

    Cube2D_Falling();

public:
    void simulate(ShapeOp::Solver &solver, MatrixXd &V);


private:
    MatrixXd left_fixed_;
    MatrixXd right_fixed_;
    MatrixXd middle_dynamic_;

    double t_total_;
    double dt_interval_;
};



Cube2D_Falling::Cube2D_Falling()
{
    left_fixed_ = right_fixed_ = MatrixXd(4, 3);
    middle_dynamic_ = MatrixXd(5, 3);

    left_fixed_ <<      0, 0, 0,
                        3, 0, 0,
                        3, 3, 0,
                        0, 3, 0;

    right_fixed_ <<     4, 0, 0,
                        7, 0, 0,
                        7, 3, 0,
                        4, 3, 0;

    middle_dynamic_ <<  3, 2.5, 0,
                        4, 2.5, 0,
                        5, 3, 0,
                        3.5, 6, 0,
                        2, 3, 0;

    t_total_ = 5;
    dt_interval_ = 0.1;
}

void Cube2D_Falling::simulate(ShapeOp::Solver &solver, MatrixXd &V)
{
    V = MatrixXd::Zero(13, 3);
    V <<    left_fixed_,
            right_fixed_,
            middle_dynamic_;

    ShapeOp::Matrix3X p = V.transpose();
    solver.setPoints(p);

    //set fixed point (left and right)
    ShapeOp::Scalar weight(0.1);
    for(int id = 0; id < 13; id++)
    {
        std::vector<int> id_vector;
        id_vector.push_back(id);
        auto c = std::make_shared<ShapeOp::ClosenessConstraint>(id_vector, weight, solver.getPoints());
        solver.addConstraint(c);
    }

    weight = 100;
    //set rigid body
    {
        std::vector<int> id_vector;
        id_vector.clear();
        for(int id = 8; id < 13; id++)
        {
            id_vector.push_back(id);
        }

        auto c = std::make_shared<ShapeOp::SimilarityConstraint>(id_vector, weight, solver.getPoints(), false, true, false);
        solver.addConstraint(c);
        // edit the shapes one of which the rigid constraint brings the involved vertices close to.
        std::vector<ShapeOp::Matrix3X> shapes;

        //outer
        ShapeOp::Matrix3X shape = middle_dynamic_.transpose(); //column major
        shapes.push_back(shape);

//        //inner
//        ShapeOp::Vector3 center(0, 0, 0);
//        for(int id = 0; id < shape.cols(); id++)
//            center += shape.col(id);
//        center/= shape.cols();
//        for(int id = 0; id < shape.cols(); id++)
//            shape.col(id) = (shape.col(id) - center) * 0.5 + center;
//        shapes.push_back(shape);

        c->setShapes(shapes);

    }

    weight = 100;
    //set  Collision
    {
        std::vector<int> id_vector,obj_vector;

        //cube
        for(int id = 8; id < 13; id++) id_vector.push_back(id);

        //cube1
        for(int id = 0; id < 4; id++) obj_vector.push_back(id);

        auto c = std::make_shared<ShapeOp::OneSideCollisionConstrain>(id_vector, weight, solver.getPoints(), obj_vector);
        solver.addConstraint(c);

        //cube2
        obj_vector.clear();
        for(int id = 4; id < 8; id++) obj_vector.push_back(id);

        c = std::make_shared<ShapeOp::OneSideCollisionConstrain>(id_vector, weight, solver.getPoints(), obj_vector);
        solver.addConstraint(c);
    }

//    for(int id = 8; id < 12; id++)
//    {
//        auto f = std::make_shared<ShapeOp::VertexForce>(ShapeOp::Vector3(0, -10, 0), id);
//        solver.addForces(f);
//    }

//    solver.initialize(true);
//    solver.setTimeStep(dt_interval_);
//    solver.setDamping(0);
    solver.initialize(false);
//    for(int iter = 0; iter < (int)(t_total_/dt_interval_); iter ++)
//    {
//        solver.solve(100);
//    }
    return;
}

#endif //TI_STABLE_CUBE2D_FALLING_H
