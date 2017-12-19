//
// Created by ziqwang on 07.12.17.
//

#ifndef TI_STABLE_POLYHEDRAPOINTS_H
#define TI_STABLE_POLYHEDRAPOINTS_H

#include "PolyhedraBase.h"
#include "PolygonPoints.h"
#include <Eigen/Geometry>
using std::vector;
using Eigen::Vector3d;
using Eigen::Quaterniond;
typedef std::vector<Vector3d,Eigen::aligned_allocator<Vector3d> > vecVector3d;

class PolyhedraPoints : public PolyhedraBase{
public:
    PolyhedraPoints()
    {

    }

public:
    void set_points(MatrixXd &points) { points_ = points; }

    void get_points(MatrixXd &points) { points = points_; }

    Vector3d point(int ID) {return points_.col(ID % points_.cols());}

    inline int nV(){ return points_.cols();}

public:


    inline void get_normal(int ID, Vector3d &normal);

    void get_normals(vecVector3d &normals);

    int get_edges(vecVector3d &edges);

    inline void get_center(Vector3d &center);

    void shrink(double ratio);

    void transformation(Quaterniond quat, Vector3d v);

public:

    bool collision(PolyhedraPoints    &B,
                   Vector3d         &nA,
                   Vector3d         &nB,
                   vecVector3d      &pa,
                   vecVector3d      &pb,
                   double           &signed_dist);

public:

    double max_project(Vector3d n, vecVector3d &p, double tol = 0.1);

    void candidate_sat(PolyhedraPoints &B,
                       vecVector3d &axis,
                       vecVector3d &axisA,
                       vecVector3d &axisB);

private:
    MatrixXd points_;
};

inline void PolyhedraPoints::get_normal(int ID, Vector3d &normal) {
    assert(0 <= ID && ID < fv_.size());
    PolygonBase &face = fv_[ID];

    Vector3d p[3];
    for(int id = 0; id < 3; id++) p[id] = points_.col(face.ids_[id]);

    normal = (p[2] - p[1]).cross(p[0] - p[1]);
    normal.normalize();
    return;
}

inline void PolyhedraPoints::get_center(Vector3d &center) {
    center = Vector3d(0, 0, 0);
    for(int id = 0; id < points_.cols(); id++)
        center = center + points_.col(id);
    center /= points_.cols();
    return;
}




#endif //TI_STABLE_POLYHEDRAPOINTS_H
