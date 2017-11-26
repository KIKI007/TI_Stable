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

#include "VertexCollisionConstrain.h"

using Eigen::MatrixXd;

class Cube2D_Falling {
public:

    Cube2D_Falling();

public:
    void simulate(ShapeOp::Solver &solver);


private:
    MatrixXd left_fixed_;
    MatrixXd right_fixed_;
    MatrixXd middle_dynamic_;

    double t_total_;
    double dt_interval_;
};



Cube2D_Falling::Cube2D_Falling()
{
    left_fixed_ = right_fixed_ = middle_dynamic_ = MatrixXd(4, 3);

    left_fixed_ <<      0, 0, 0,
                        1, 0, 0,
                        1, 1, 0,
                        0, 1, 0;

    right_fixed_ <<     2, 0, 0,
                        3, 0, 0,
                        3, 1, 0,
                        2, 1, 0;

    middle_dynamic_ <<  1.5, 2, 0,
                        2.5, 3, 0,
                        1.5, 4, 0,
                        0.5, 3, 0;

    t_total_ = 10;
    dt_interval_ = 0.1;
}

void Cube2D_Falling::simulate(ShapeOp::Solver &solver)
{
    MatrixXd V(12, 3);
    V <<    left_fixed_,
            right_fixed_,
            middle_dynamic_;

    ShapeOp::Matrix3X p = V.transpose();
    solver.setPoints(p);

    //set fixed point (left and right)
    ShapeOp::Scalar weight(1);
    for(int id = 0; id < 8; id++)
    {
        std::vector<int> id_vector;
        id_vector.push_back(id);
        auto c = std::make_shared<ShapeOp::ClosenessConstraint>(id_vector, weight, solver.getPoints());
        solver.addConstraint(c);
    }

    //set rigid body
    {
        std::vector<int> id_vector;
        id_vector.clear();
        for(int id = 8; id < 12; id++)
        {
            id_vector.push_back(id);
        }

        auto c = std::make_shared<ShapeOp::SimilarityConstraint>(id_vector, weight, solver.getPoints(), false, true, false);
        solver.addConstraint(c);
        // edit the shapes one of which the rigid constraint brings the involved vertices close to.
        std::vector<ShapeOp::Matrix3X> shapes;
        ShapeOp::Matrix3X shape = middle_dynamic_.transpose(); //column major
        shapes.push_back(shape);
        c->setShapes(shapes);
    }

    //set Vertex Collision
    {
        std::vector<int> id_vector,obj_vector;
        for(int id = 8; id < 12; id++)
        {
            id_vector.clear();
            id_vector.push_back(id);
            for(int jd = 0; jd < 4; jd++)
            {
                obj_vector.push_back(jd);
            }
            auto c = std::make_shared<VertexCollisionConstrain>(id_vector, weight, solver.getPoints(), obj_vector);
            solver.addConstraint(c);

            obj_vector.clear();
            for(int jd = 4; jd < 8; jd++)
            {
                obj_vector.push_back(jd);
            }
            c = std::make_shared<VertexCollisionConstrain>(id_vector, weight, solver.getPoints(), obj_vector);
            solver.addConstraint(c);
        }
    }

    for(int id = 8; id < 12; id++)
    {
        auto f = std::make_shared<ShapeOp::VertexForce>(ShapeOp::Vector3(0, -1, 0), id);
        solver.addForces(f);
    }

    solver.initialize(true);
    solver.setTimeStep(dt_interval_);
    solver.setDamping(0);

    for(int iter = 0; iter < (int)(t_total_/dt_interval_); iter ++)
    {
        solver.solve(100);
    }
    return;
}

#endif //TI_STABLE_CUBE2D_FALLING_H
