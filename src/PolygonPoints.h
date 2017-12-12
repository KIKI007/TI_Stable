//
// Created by ziqwang on 07.12.17.
//

#ifndef TI_STABLE_POLYGONPOINTS_H
#define TI_STABLE_POLYGONPOINTS_H

#include "PolygonBase.h"
#include <iostream>
#include <igl/mosek/mosek_quadprog.h>
typedef Eigen::Triplet<double> T;
class PolygonPoints :public PolygonBase{
public:

    PolygonPoints() { clear(); }

    PolygonPoints(std::vector<int> &ids, MatrixXd &points)
    :PolygonBase(ids) { set_points(points); }

public:

    inline void set_points(MatrixXd &points) { points_ = points; }

    inline void clear();

public:

    inline void  get_points(MatrixXd &points) { points = points_; }

    inline void  get_edge(int id, Vector3d &edgevec);

    inline void  get_normal(Vector3d &normal);

    inline void  get_center(Vector3d &center);

    inline Vector3d point(int ID) { assert(0 <= ID < points_.cols()); return points_.col(ID); }

    inline Vector3d point_next(int ID) { return points_.col((ID + 1) % points_.cols()); }

public:

    inline void do_transformation(Vector3d x);

    inline void do_scale(double ratio);

private:

    void sat_project(std::vector<double> &proj, Vector3d &axis, Vector3d &ori);

    void get_list_min(std::vector<double> &proj, double &min, int &idx);

public:
    bool collision(PolygonPoints    &B,
                   Vector3d         &n,
                   bool             &nA,
                   int              &pa,
                   int              &pb);

    bool collision(PolygonPoints    &B,
                   Vector3d         &n,
                   bool             &nA,
                   int              &pa,
                   int              &pb,
                   int              &pc,
                   double           eps = 0.1);

    bool collision(PolygonPoints    &B,
                   Vector3d         &n,
                   bool             &nA,
                   int              &Ia,
                   int              &Ib,
                   double           &muA,
                   double           &muB);
private:
    MatrixXd points_;
};


inline void PolygonPoints::clear()
{
    PolygonBase::clear();
    points_.setZero();
    color_ = Vector3d(0, 0, 0);
}

inline void PolygonPoints::get_edge(int ID, Vector3d &edgevec)
{
    int sta, end;
    EDGE(ID, sta, end);
    edgevec = points_.col(end) - points_.col(sta);
}

inline void PolygonPoints::get_normal(Vector3d &normal)
{
    assert(ids_.size() >= 3);

    Eigen::Vector3d pA= points_.col(ids_[0]);
    Eigen::Vector3d pB= points_.col(ids_[1]);
    Eigen::Vector3d pC= points_.col(ids_[2]);
    normal = (pC - pB).cross(pA - pB);
    normal.normalize();
    return;
}

inline void PolygonPoints::do_transformation(Vector3d x)
{
    MatrixXd R_theta(3, 3);
    R_theta << std::cos(x(0)), -std::sin(x(0)), 0,
            std::sin(x(0)),  std::cos(x(0)), 0,
            0,               0, 1;
    Vector3d center;
    get_center(center);
    for(int id = 0; id < points_.cols(); id++)
    {
        points_.col(id) = R_theta * (points_.col(id) - center) + center + Vector3d(x(1), x(2), 0);
    }
}

inline void PolygonPoints::do_scale(double ratio)
{
    Vector3d center;
    get_center(center);
    for(int id = 0; id < points_.cols(); id++)
    {
        points_.col(id) = ratio * (points_.col(id) - center) + center;
    }
}

inline void PolygonPoints::get_center(Vector3d &center)
{
    center = Vector3d(0, 0, 0);
    for(int id = 0; id < points_.cols(); id++)
        center = center + points_.col(id);
    center /= points_.cols();
}
#endif //TI_STABLE_POLYGONPOINTS_H
