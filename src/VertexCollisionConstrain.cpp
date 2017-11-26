//
// Created by ziqwang on 26.11.17.
//

#include "VertexCollisionConstrain.h"
///////////////////////////////////////////////////////////////////////////////
namespace ShapeOp {
    SHAPEOP_INLINE VertexCollisionConstrain::VertexCollisionConstrain(const std::vector<int> &idI,
                                                                      Scalar weight,
                                                                      const Matrix3X &positions,
                                                                      const std::vector<int> &obj_idI):
        Constraint(idI, weight)
    {
        assert(obj_idI.size() >= 3 && idI.size() == 1);
        obj_idI_ = obj_idI;
    }

    SHAPEOP_INLINE void VertexCollisionConstrain::project(const Matrix3X &positions, Matrix3X &projections) const {
        dynamic_pt_ = positions.col(idI_[0]);

        ShapeOp::Vector3 center(0, 0, 0), a, b;
        for (int id = 0; id < obj_idI_.size(); id++) {
            center += positions[obj_idI_[id]];
        }
        center /= obj_idI_.size();

        ShapeOp::Scalar area = 0, Obj_area = 0;
        for (int id = 0; id < obj_idI_.size(); id++) {
            int n_id = (id + 1) % obj_idI_.size();
            a = positions.col(obj_idI_[id]);
            b = positions.col(obj_idI_[n_id]);

            area += (dynamic_pt_ - a).cross(dynamic_pt_ - b).norm();
            Obj_area += (center - a).cross(center - b).norm();
        }
        double eps_ = 1e-3;
        if (std::abs(std::abs(area) - std::abs(Obj_area)) < eps_) {
            projections = weight_ * dynamic_pt_;
        } else {
            double length;
            Vector3 mvec(0, 0, 0), min_mvec(0, 0, 0);
            double min_move = 1e10;

            for (int id = 0; id < obj_idI_.size(); id++) {
                int n_id = (id + 1) % obj_idI_.size();
                a = positions.col(obj_idI_[id]);
                b = positions.col(obj_idI_[n_id]);

                length = (dynamic_pt_ - a).dot(b - a) / (b - a).norm();
                mvec = (b - a) / (b - a).norm * length - (dynamic_pt_ - a);
                if (mvec.norm() < min_move) {
                    min_mvec = mvec;
                    min_move = mvec.norm();
                }
            }
            projections = weight_ * (dynamic_pt_ + mvec);
        }
    }

    SHAPEOP_INLINE void VertexCollisionConstrain::addConstraint(std::vector<Triplet> &triplets, int &idO) const {
        idO_ = id0;
        triplets.push_back(Triplet(id0, idI_[0], weight_));
    }
}