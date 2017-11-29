//
// Created by ziqwang on 28.11.17.
//

#ifndef TI_STABLE_COLLISIONCONSTRAIN_H
#define TI_STABLE_COLLISIONCONSTRAIN_H

#include "Constraint.h"
namespace ShapeOp {
    class SHAPEOP_API OneSideCollisionConstrain : public Constraint {

    public:
        OneSideCollisionConstrain(const std::vector<int> &idI,
                             Scalar weight,
                             const Matrix3X &positions,
                             const std::vector<int> &obj_idI);
        virtual ~OneSideCollisionConstrain
                () {}

        /** \brief Find the closest configuration from the input positions that satisfy the constraint.*/
        virtual void project(const Matrix3X &positions, Matrix3X &projections) const override final;

        /** \brief Add the constraint to the linear system.*/
        virtual void addConstraint(std::vector<Triplet> &triplets, int &idO) const override final;


    public:

        bool separate_axis_theorem( Matrix3X &objA,
                                    Matrix3X &objB,
                                    std::vector<Scalar> &projA,
                                    std::vector<Scalar> &projB,
                                    Vector3 &axisVec) const;

        void sequences_min_max(std::vector<Scalar> &seq, Scalar &min, Scalar& max) const;

        void project_in_axis(Matrix3X &obj, std::vector<Scalar> &proj, Vector3 &axisVec) const;

    private:
    std::vector<int> obj_idI_;
};

}

#endif //TI_STABLE_COLLISIONCONSTRAIN_H
