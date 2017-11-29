//
// Created by ziqwang on 28.11.17.
//

#include "OneSideCollisionConstrain.h"
#include <iostream>
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
            axislist.push_back(objA.col(n_id) - objA.col(id));
            axislist.back().normalize();
        }

        //objB
        for (long id = 0; id < objB.cols(); id++) {
            long n_id = (id + 1) % objB.cols();
            axislist.push_back(objB.col(n_id) - objB.col(id));
            axislist.back().normalize();
        }

        //compute the min_intersect_length
        for (size_t id = 0; id < axislist.size(); id++) {
            std::vector<Scalar> t_projA, t_projB;
            project_in_axis(objA, t_projA, axislist[id]);
            project_in_axis(objB, t_projB, axislist[id]);

            Scalar maxB, minB;
            sequences_min_max(t_projA, minB, maxB);

            Scalar maxA, minA;
            sequences_min_max(t_projB, minA, maxA);

            Scalar positive_move_dist = maxB - minA;
            Scalar negative_move_dist = maxA - minB;

            if (positive_move_dist < 0 || negative_move_dist < 0)
                return false;
            else {
                Scalar intersect_length =
                        positive_move_dist > negative_move_dist ? negative_move_dist : positive_move_dist;
                if (intersect_length < min_intersect_length) {
                    min_intersect_length = intersect_length;
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

            Scalar minA, maxA;
            sequences_min_max(projA, minA, maxA);

            Scalar positive_move_dist = maxB - minA;
            Scalar negative_move_dist = maxA - minB;

            int move_dist = 0;
            if(positive_move_dist < negative_move_dist)
            {
                move_dist = positive_move_dist > 0 ? 1 : 0;
            }
            else
            {
                move_dist = negative_move_dist > 0 ? -1 : 0;
            }

            Scalar eps = 1e-5;
            Scalar min_move;
            if(move_dist > 0)
            {
                min_move = maxB - minA;
                for(size_t id = 0; id < idI_.size(); id++)
                {
                    if(min_move > projA[id] - minA && projA[id] - minA > eps)
                    {
                        min_move = projA[id] - minA;
                    }
                }
            }
            else
            {
                min_move = maxA - minB;
                for(size_t id = 0; id < idI_.size(); id++)
                {
                    if(min_move > -projA[id] + maxA && -projA[id] + maxA > eps)
                    {
                        min_move = -projA[id] + maxA;
                    }
                }
            }


            for(size_t id = 0; id < idI_.size(); id++)
            {
                axisVec.normalize();
                int ID = idI_[id];
                if(move_dist > 0) projections.col(id + idO_) = positions.col(ID) + axisVec * (maxB - projA[id] > 0 ? maxB - projA[id] : 0);
                if(move_dist < 0) projections.col(id + idO_) = positions.col(ID) + axisVec * (minB - projA[id] < 0 ? minB - projA[id] : 0);
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