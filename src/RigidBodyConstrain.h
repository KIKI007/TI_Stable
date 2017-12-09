////
//// Created by ziqwang on 03.12.17.
////
//
//#ifndef TI_STABLE_RIGIDBODYCONSTRAIN_H
//#define TI_STABLE_RIGIDBODYCONSTRAIN_H
//
//
//#include "Constraint.h"
//namespace ShapeOp {
//    class SHAPEOP_API RigidBodyConstrain : public Constraint {
//
//    public:
//        RigidBodyConstrain(const std::vector<int> &idI,
//                           Scalar weight,
//                           const Matrix3X &positions);
//
//        virtual ~RigidBodyConstrain
//                () {}
//
//        /** \brief Find the closest configuration from the input positions that satisfy the constraint.*/
//        virtual void project(const Matrix3X &positions, Matrix3X &projections) const override final;
//
//        /** \brief Add the constraint to the linear system.*/
//        virtual void addConstraint(std::vector<Triplet> &triplets, int &idO) const override final;
//
//        void setShape(Matrix3X &shape);
//
//    private:
//        void subtract_centroid(Matrix3X &mat) const;
//
//        void svd_rotation_matrix(const Matrix3X &X, const Matrix3X &Y, Matrix33 &R) const;
//
//    private:
//        Matrix3X shape_;
//    };
//}
//
//#endif //TI_STABLE_RIGIDBODYCONSTRAIN_H
