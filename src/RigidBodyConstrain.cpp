//
// Created by ziqwang on 03.12.17.
//
#include <iostream>
#include "RigidBodyConstrain.h"
namespace ShapeOp {
    SHAPEOP_INLINE RigidBodyConstrain::RigidBodyConstrain(const std::vector<int> &idI,
                                                                        Scalar weight,
                                                                        const Matrix3X &positions):
            Constraint(idI, weight)
    {
        assert(idI.size() >= 4);
}

    SHAPEOP_INLINE void RigidBodyConstrain::project(const Matrix3X &positions, Matrix3X &projections) const{
        assert(shape_.cols() == idI_.size());

        Matrix3X X = Matrix3X::Zero(3, idI_.size());
        for(int id = 0; id < idI_.size(); id++)
        {
            X.col(id) = positions.col(idI_[id]);
        }

        //shape has been already subtracted its centroid
        subtract_centroid(X);

        Matrix33 R;
        svd_rotation_matrix(X, shape_, R);

        std::cout << "distance" << (X - R * shape_).norm() << std::endl;
        projections.block(0, idO_, 3, shape_.cols()) = R * shape_ * weight_;

    }

    void RigidBodyConstrain::subtract_centroid(Matrix3X &mat) const
    {
        Vector3 centroid(0, 0, 0);
        for(int id = 0; id < mat.cols(); id++)
        {
            centroid += mat.col(id);
        }
        centroid /= mat.cols();

        for(int id = 0; id < mat.cols(); id++)
        {
            mat.col(id) -= centroid;
        }
    }


    void RigidBodyConstrain::svd_rotation_matrix(const Matrix3X &X, const Matrix3X &Y, Matrix33 &R) const
    {
        Eigen::MatrixXd S= X*Y.transpose();
        Eigen::JacobiSVD<Eigen::MatrixXd> svd(S, Eigen::ComputeThinU | Eigen::ComputeThinV);

        Matrix33 U = svd.matrixU(), V = svd.matrixV();

        Scalar sign = (U*V.transpose()).determinant() >0 ? 1 : -1;

        Matrix33 L = Matrix33::Zero(3, 3);
        L(0, 0) = 1;
        L(1, 1) = 1;
        L(2, 2) = sign;

        R = U*L*V.transpose();
    }

    SHAPEOP_INLINE void RigidBodyConstrain::addConstraint(std::vector<Triplet> &triplets, int &idO) const {

        idO_ = idO;
        int n_idx = static_cast<int>(idI_.size());
        Scalar coef1 = (1.0 - 1.0 / n_idx) * weight_;
        Scalar coef2 = -weight_ / n_idx;
        for (int i = 0; i < n_idx; ++i) {
            for (int j = 0; j < n_idx; ++j)
                triplets.push_back(Triplet(idO, idI_[j], (i == j ? coef1 : coef2)));
            idO++;
        }

    }

    SHAPEOP_INLINE void RigidBodyConstrain::setShape(Matrix3X &shape)
    {
        shape_ = shape;
        subtract_centroid(shape_);
    }
}