//
// Created by ziqwang on 07.12.17.
//

#ifndef TI_STABLE_POLYHEDRAPOINTS_H
#define TI_STABLE_POLYHEDRAPOINTS_H

#include "PolyhedraBase.h"
#include "PolygonPoints.h"
class PolyhedraPoints : public PolyhedraBase{

public:
    PolyhedraPoints()
    {

    }

public:
    void set_points(MatrixXd &points)
    {
        points_ = points;
    }

    void get_points(MatrixXd &points)
    {
        points = points_;
    }
public:

    inline void get_normal(int ID, Vector3d &normal)
    {
        assert(0 <= ID && ID < ids_.size());
        PolygonBase &face = ids_[ID];

        Vector3d p[3];
        for(int id = 0; id < 3; id++) p[id] = points_.col(face.ids_[id]);

        normal = (p[2] - p[1]).cross(p[0] - p[1]);
        normal.normalize();
    }

    void center_points(Vector3d &center)
    {
        center = Vector3d(0, 0, 0);
        for(int id = 0; id < points_.cols(); id++)
            center = center + points_.col(id);
        center /= points_.cols();
    }

    void shrink(double ratio)
    {
        Vector3d center;
        center_points(center);
        for(int id = 0; id < points_.cols(); id++)
        {
            points_.col(id) = (points_.col(id) - center) * ratio + center;
        }

    }


private:
    MatrixXd points_;
};


#endif //TI_STABLE_POLYHEDRAPOINTS_H
