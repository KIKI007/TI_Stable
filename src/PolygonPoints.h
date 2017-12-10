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
        assert(0 <= ID < points_.cols());
        return points_.col(ID);
    }

    Vector3d pointNext(int ID)
    {
        return points_.col((ID + 1) % points_.cols());
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
        double max_dist = std::numeric_limits<double>::infinity() * (-1);
        bool is_collision = true;

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

            if(min >=0)
                is_collision = false;

            if(max_dist < min)
            {
                max_dist = min;

                pa = id;
                pb = min_id;

                n =  -axis;
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

            if(min >=0)
                is_collision = false;

            if(max_dist < min)
            {
                max_dist = min;

                pa = min_id;
                pb = id;

                n = axis;
                nA = false;
            }
        }
        return is_collision;
    }

    bool collision(PolygonPoints &B,
                   Vector3d &n,
                   bool &nA,
                   int &pa,
                   int &pb,
                   int &pc,
                   double eps = 0.1)
    {
        bool is_collision = collision(B, n, nA, pa, pb);
        double distance_ab = n.dot(point(pa) - B.point(pb));
        if(nA)
        {
            pc = -1;
            for(int id = 0; id < B.nV(); id++)
            {
                double distance_ac = n.dot(point(pa) - B.point(id));
                if(id != pb && distance_ac < distance_ab + eps)
                {
                    pc = id;
                    break;
                }
            }
        }
        else
        {
            pc = -1;
            for(int id = 0; id < nV(); id++)
            {
                double distance_bc = n.dot(point(id) - B.point(pb));
                if(id != pa && distance_bc < distance_ab + eps)
                {
                    pc = id;
                    break;
                }
            }
        }
        return true;
    }

private:
    MatrixXd points_;
};





#endif //TI_STABLE_POLYGONPOINTS_H
