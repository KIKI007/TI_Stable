//
// Created by ziqwang on 26.11.17.
//

#ifndef TI_STABLE_VERTEXCOLLISIONCONSTRAIN_H
#define TI_STABLE_VERTEXCOLLISIONCONSTRAIN_H

#include "Constraint.h"

namespace ShapeOp {
    class SHAPEOP_API VertexCollisionConstrain : public Constraint {
    public:
        VertexCollisionConstrain(const std::vector<int> &idI,
                                 Scalar weight,
                                 const Matrix3X &positions,
                                 const std::vector<int> &obj_idI);

        virtual ~VertexCollisionConstrain() {}

        /** \brief Find the closest configuration from the input positions that satisfy the constraint.*/
        virtual void project(const Matrix3X &positions, Matrix3X &projections) const override final;

        /** \brief Add the constraint to the linear system.*/
        virtual void addConstraint(std::vector<Triplet> &triplets, int &idO) const override final;

    private:
        std::vector<int> obj_idI_;

        Matrix3X dynamic_pt_;
    };

}

#endif //TI_STABLE_VERTEXCOLLISIONCONSTRAIN_H
