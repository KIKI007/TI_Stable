////
//// Created by ziqwang on 28.11.17.
////
//
//#ifndef TI_STABLE_COLLISIONCONSTRAIN_H
//#define TI_STABLE_COLLISIONCONSTRAIN_H
//
//#include "PolyhedraBase.h"
//#include "Constraint.h"
//namespace ShapeOp {
//    class SHAPEOP_API OneSideCollisionConstrain : public Constraint {
//
//    public:
//        OneSideCollisionConstrain(
//                             const std::vector<int> &idI,
//                             PolyhedraBase &p,
//                             Scalar weight,
//                             const Matrix3X &positions
//
//                             );
//        virtual ~OneSideCollisionConstrain
//                () {}
//
//        /** \brief Find the closest configuration from the input positions that satisfy the constraint.*/
//        virtual void project(const Matrix3X &positions, Matrix3X &projections) const override final;
//
//        /** \brief Add the constraint to the linear system.*/
//        virtual void addConstraint(std::vector<Triplet> &triplets, int &idO) const override final;
//
//
//    public:
//
//        void set_collision_object(const std::vector<int> &obj_idI)
//
//        bool separate_axis_theorem( Matrix3X &objA,
//                                    Matrix3X &objB,
//                                    std::vector<Scalar> &sdistA,
//                                    std::vector<Scalar> &sdistB,
//                                    Vector3 &axisVec,
//                                    Vector3 &origin) const;
//
//        void sequences_min_max(std::vector<Scalar> &seq,
//                               Scalar &min,
//                               Scalar& max) const;
//
//        void project_in_plane(Matrix3X &objA,
//                              std::vector<Scalar> &sdist,
//                              Vector3 &axisVec,
//                              Vector3 &Origin) const;
//
//    private:
//        std::vector<int> obj_idI_;
//        PolyhedraBase p_;
//        int cf_;
//};
//
//}
//
//#endif //TI_STABLE_COLLISIONCONSTRAIN_H
