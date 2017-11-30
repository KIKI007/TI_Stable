//
// Created by ziqwang on 28.11.17.
//

#include "OneSideCollisionConstrain.h"
#include <iostream>
#include <map>
namespace ShapeOp {

    SHAPEOP_INLINE OneSideCollisionConstrain::OneSideCollisionConstrain(const std::vector<int> &idI,
                                                          Scalar weight,
                                                          const Matrix3X &positions,
                                                          const std::vector<int> &obj_idI):
            Constraint(idI, weight)
    {
        assert(obj_idI.size() >= 3 && idI.size() >= 3);
        obj_idI_ = obj_idI;
    }

    SHAPEOP_INLINE void OneSideCollisionConstrain::project_in_axis(Matrix3X &obj,
                                                            std::vector<Scalar> &proj,
                                                            Vector3 &axisVec) const
    {
        for(int id = 0; id < obj.cols(); id++)
        {
            Vector3 pt = obj.col(id);
            proj.push_back(pt.dot(axisVec));
        }
        return;
    }

    SHAPEOP_INLINE void OneSideCollisionConstrain::sequences_min_max(std::vector<Scalar> &seq, Scalar &min, Scalar& max) const
    {
        min = max = seq.at(0);
        for(size_t id = 1; id < seq.size(); id++)
        {
            if(min > seq.at(id)) min = seq.at(id);
            if(max < seq.at(id)) max = seq.at(id);
        }

        return;
    }

    SHAPEOP_INLINE bool OneSideCollisionConstrain::separate_axis_theorem( Matrix3X &objA,
                                                                   Matrix3X &objB,
                                                                   std::vector<Scalar> &projA,
                                                                   std::vector<Scalar> &projB,
                                                                   Vector3 &axisVec) const
    {
        Scalar min_intersect_length = 1e10;

        std::vector<Vector3> axislist;
        //objA
        for (long id = 0; id < objA.cols(); id++) {
            long n_id = (id + 1) % objA.cols();
            Vector3 edgeVec = objA.col(n_id) - objA.col(id), projVec(0, 0, 0);
            projVec = edgeVec.cross(Vector3(0, 0, 1)); projVec.normalize();
            axislist.push_back(projVec);
            axislist.push_back(projVec * (-1));
        }

        //objB
        for (long id = 0; id < objB.cols(); id++) {
            long n_id = (id + 1) % objB.cols();
            Vector3 edgeVec = objB.col(n_id) - objB.col(id), projVec(0, 0, 0);
            projVec = edgeVec.cross(Vector3(0, 0, 1)); projVec.normalize();
            axislist.push_back(projVec);
            axislist.push_back(projVec * (-1));
        }

        //compute the min_intersect_length
        for (size_t id = 0; id < axislist.size(); id++) {
                std::vector<Scalar> t_projA, t_projB;
                project_in_axis(objA, t_projA, axislist[id]);
                project_in_axis(objB, t_projB, axislist[id]);

                Scalar maxB, minB;
                sequences_min_max(t_projB, minB, maxB);

                Scalar maxA, minA;
                sequences_min_max(t_projA, minA, maxA);

                Scalar positive_move_dist = maxB - minA;
                Scalar negative_move_dist = maxA - minB;

                if (positive_move_dist < 0 || negative_move_dist < 0)
                    return false;
                else {
                    if (positive_move_dist < min_intersect_length) {
                            min_intersect_length = positive_move_dist;
                            axisVec = axislist[id];
                            projA = t_projA;
                            projB = t_projB;
                    }
                }
            }
        return true;
    }


    SHAPEOP_INLINE void OneSideCollisionConstrain::project(const Matrix3X &positions, Matrix3X &projections) const {

        Matrix3X objA, objB;
        objA = Matrix3X::Zero(3, idI_.size());
        objB = Matrix3X::Zero(3, obj_idI_.size());

        for(int id = 0; id < idI_.size();id++){
            objA.col(id) = positions.col(idI_[id]);
        }


        for(int id = 0; id < obj_idI_.size();id++)
            objB.col(id) = positions.col(obj_idI_[id]);

        std::vector<Scalar> projA, projB;
        Vector3 axisVec;

        if(OneSideCollisionConstrain::separate_axis_theorem(objA, objB, projA, projB, axisVec))
        {
            Scalar maxB, minB;
            sequences_min_max(projB, minB, maxB);

            Scalar maxA, minA;
            sequences_min_max(projA, minA, maxA);

            std::map <int, bool> move_point;
            Scalar  eps = 1e-5;
            for(int id = 0; id < idI_.size(); id++)
            {
                if(projA[id] - minA < eps)
                    move_point.insert(std::pair<int,bool>(id, true));
            }

            for(size_t id = 0; id < idI_.size(); id++)
            {
                axisVec.normalize();
                int ID = idI_[id];
                if(move_point[id] == true) projections.col(id + idO_) = positions.col(ID) + axisVec * (maxB - projA[id] > 0 ? maxB - projA[id] : 0);
                else projections.col(id + idO_) = positions.col(ID);
                projections.col(id + idO_) *= weight_;
            }
        }
        else
        {
            //keep its position
            for(int id = 0; id < idI_.size();id++)
                projections.col(id + idO_) = weight_ * positions.col(idI_[id]);
        }
    }

    SHAPEOP_INLINE void OneSideCollisionConstrain::addConstraint(std::vector<Triplet> &triplets, int &idO) const {
        idO_ = idO;
        for(size_t id = 0; id < idI_.size(); id++)
        {
            triplets.push_back(Triplet(idO, idI_[id], weight_));
            idO++;
        }

    }
}