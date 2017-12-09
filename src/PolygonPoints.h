//
// Created by ziqwang on 07.12.17.
//

#ifndef TI_STABLE_POLYGONPOINTS_H
#define TI_STABLE_POLYGONPOINTS_H

#include "PolygonBase.h"
#include <iostream>
class PolygonPoints :public PolygonBase{

public:
    PolygonPoints()
    {

    }
    PolygonPoints(std::vector<int> &ids, MatrixXd &points)
    :PolygonBase(ids)
    {
        set_points(points);
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

    void get_normal(Vector3d &normal)
    {
        assert(ids_.size() >= 3);

        Eigen::Vector3d pA= points_.col(ids_[0]);
        Eigen::Vector3d pB= points_.col(ids_[1]);
        Eigen::Vector3d pC= points_.col(ids_[2]);
        normal = (pC - pB).cross(pA - pB);
        normal.normalize();

        return;
    }

    void get_edge(int ID, Vector3d &edgevec)
    {
        int sta, end;
        EDGE(ID, sta, end);
        edgevec = points_.col(end) - points_.col(sta);
    }

    Vector3d point(int ID)
    {
        assert(0 <= ID < points_.size());
        return points_.col(ID);
    }

    void Rotate_translate(Vector3d x)
    {
        MatrixXd R_theta(3, 3);
        R_theta << std::cos(x(0)), -std::sin(x(0)), 0,
                   std::sin(x(0)),  std::cos(x(0)), 0,
                   0             ,  0             , 1;
        Vector3d center;
        center_points(center);
        for(int id = 0; id < points_.cols(); id++)
        {
            points_.col(id) = R_theta * (points_.col(id) - center) + center + Vector3d(x(1), x(2), 0);
        }
    }

public:

    void project(std::vector<double> &proj, Vector3d &axis, Vector3d &ori)
    {
        for(int id = 0; id < points_.cols(); id++)
        {
            proj.push_back(axis.dot(points_.col(id) - ori));
        }
        return;
    }

    void min_max_proj(std::vector<double> &proj, double &min, int &idx)
    {
        min = std::numeric_limits<double>::infinity();

        for(int id = 0; id < proj.size(); id++)
        {
            if(min > proj[id])
            {
                min = proj[id];
                idx = id;
            }
        }
        return;
    }

    void center_points(Vector3d &center)
    {
        center = Vector3d(0, 0, 0);
        for(int id = 0; id < points_.cols(); id++)
            center = center + points_.col(id);
        center /= points_.cols();
    }

    bool collision(PolygonPoints &B,
                   Vector3d &n,
                   bool &nA,
                   int &pa,
                   int &pb)
    {
        double min_dist = std::numeric_limits<double>::infinity();


        //A->B
        Vector3d nrm; get_normal(nrm);
        for(int id = 0; id < nV(); id++)
        {
            Vector3d edge; get_edge(id, edge);
            Vector3d axis; axis = edge.cross(nrm); axis.normalize();
            Vector3d ori; ori = points_.col(id);

            std::vector<double> proj;
            B.project(proj, axis, ori);

            double min; int min_id;
            min_max_proj(proj, min, min_id);
            if(min >= 0)
            {
                return false;
            }
            else if(min_dist > std::abs(min))
            {
                min_dist = std::abs(min);

                pa = id;
                pb = min_id;

                n =  axis;
                nA = true;
            }

        }

        //B->A
        B.get_normal(nrm);
        for(int id = 0; id < B.nV(); id++)
        {
            Vector3d edge; B.get_edge(id, edge);
            Vector3d axis; axis = edge.cross(nrm); axis.normalize();
            Vector3d ori; ori = B.points_.col(id);

            std::vector<double> proj;
            project(proj, axis, ori);

            double min; int min_id;
            min_max_proj(proj, min, min_id);
            if(min >= 0)
            {
                return false;
            }
            else if(min_dist > std::abs(min))
            {
                min_dist = std::abs(min);

                pa = min_id;
                pb = id;

                n = axis;
                nA = false;
            }
        }
        return true;
    }

private:
    MatrixXd points_;
};


#endif //TI_STABLE_POLYGONPOINTS_H
