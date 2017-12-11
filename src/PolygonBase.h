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


//only store topological info
class PolygonBase {

public:
    PolygonBase() {};

    PolygonBase(std::vector<int> ids) {
        set_ids(ids);
    }

    ~PolygonBase() {};

public:

    void set_ids(std::vector<int> ids) {
        ids_ = ids;
    }

    void clear() {
        ids_.clear();
    }

    void set_color(Vector3d color) {
        color_ = color;
    }

    void set_color()
    {
        color_ = Vector3d(1, 1, 1);
    }

    void set_color_random()
    {
        color_ = Vector3d(rand()%255 / 255.0, rand()%255 / 255.0, rand()%255 / 255.0);
    }

public:

    int nV(){return ids_.size();}

    Vector3d get_color()
    {
        return color_;
    }


    void EDGE(int ID, int &sta, int &end)
    {
        assert(ids_.size() > 0);
        sta = ids_[ ID      % ids_.size()];
        end = ids_[(ID + 1) % ids_.size()];
        return;
    }

    void triangulate(MatrixXi &F)
    {
        assert(ids_.size() >= 3);

        int nF = ids_.size() - 2;
        F = MatrixXi::Zero(nF, 3);
        for(int id = 0; id < ids_.size() - 2; id++)
        {
            F.row(id) = Eigen::RowVector3i(ids_[0], ids_[id + 1], ids_[id + 2]);
        }
    }

public:
    std::vector<int> ids_;
    Vector3d color_;
};


#endif //TI_STABLE_POLYGON_H
