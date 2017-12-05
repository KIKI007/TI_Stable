//
// Created by ziqwang on 04.12.17.
//

#ifndef TI_STABLE_POLYGON_H
#define TI_STABLE_POLYGON_H

#include <vector>
#include <Eigen/Dense>
using Eigen::MatrixXd;
using Eigen::MatrixXi;
using Eigen::Vector3d;

class Polygon {
public:
    Polygon(){};

    ~Polygon(){};

public:
    void add_point(int vidx)
    {
        data_.push_back(vidx);
    }

    void set_points(std::vector<int> &data)
    {
        data_ = data;
    }

    void clear()
    {
        data_.clear();
    }

    void get_edge(int ID, int &sta, int &end)
    {
        sta = data_[ ID      % data_.size()];
        end = data_[(ID + 1) % data_.size()];
        return;
    }

    void get_normal(MatrixXd &points, Vector3d &normal)
    {
        assert(data_.size() >= 3);
        Eigen::Vector3d pA= points.row(data_[0]);
        Eigen::Vector3d pB= points.row(data_[1]);
        Eigen::Vector3d pC= points.row(data_[2]);

        normal = (pC - pB).cross(pA - pB);
        normal.normalize();

        return;
    }

    void get_edge(Eigen::MatrixXd &points, int ID, Eigen::Vector3d &edgevec)
    {
        int sta, end;
        get_edge(ID, sta, end);
        edgevec = points.row(end) - points.row(sta);
    }

    void triangulate(MatrixXi &F)
    {
        int nF = data_.size() - 2;
        F = MatrixXi::Zero(nF, 3);
        for(int id = 0; id < data_.size() - 2; id++)
        {
            F.row(id) = Eigen::RowVector3i(data_[0], data_[id + 1], data_[id + 2]);
        }
    }

    int nV(){return data_.size();}

private:
    std::vector<int> data_;
};


#endif //TI_STABLE_POLYGON_H
