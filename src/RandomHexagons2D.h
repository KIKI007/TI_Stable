//
// Created by ziqwang on 14.12.17.
//

#ifndef TI_STABLE_RANDOMHEXAGONS2D_H
#define TI_STABLE_RANDOMHEXAGONS2D_H

#include "PolygonPoints.h"
#include <string>
#include <igl/triangle/triangulate.h>
#include <vector>
#include <map>
#include <cmath>
using Eigen::RowVector2d;
using Eigen::Vector3d;
typedef  std::vector<PolygonPoints> vecPolys;
using std::vector;
using std::pair;


class RandomHexagons2D{

public:
    RandomHexagons2D()
    {
        N_ = 11;
        r_ = 1;
    }

public:

    void createHexagons(vecPolys &P, vector<pair<int, int>> &Conn, vector<int> &fixed)
    {
        fixed.clear();
        Conn.clear();
        P.clear();
        for(int id = 0; id < N_; id++)
        {
            for(int jd = 0; jd < N_; jd++)
            {
                int ID = id * N_ + jd;
                Vector3d O(jd * std::sqrt(3) * r_ + (-1) * (id&1) * std::sqrt(3)/ 2.0 * r_, id * 1.5 * r_, 0);
                MatrixXd points(3, 6);
                points.col(0) = O + Vector3d(r_ * std::cos(30 / 180.0 * M_PI), r_ * std::sin(30 / 180.0 * M_PI), 0);
                points.col(1) = O + Vector3d(r_ * std::cos(90 / 180.0 * M_PI), r_ * std::sin(90 / 180.0 * M_PI), 0);
                points.col(2) = O + Vector3d(r_ * std::cos(150 / 180.0 * M_PI), r_ * std::sin(150 / 180.0 * M_PI), 0);
                points.col(3) = O + Vector3d(r_ * std::cos(210 / 180.0 * M_PI), r_ * std::sin(210 / 180.0 * M_PI), 0);
                points.col(4) = O + Vector3d(r_ * std::cos(270 / 180.0 * M_PI), r_ * std::sin(270 / 180.0 * M_PI), 0);
                points.col(5) = O + Vector3d(r_ * std::cos(330 / 180.0 * M_PI), r_ * std::sin(330 / 180.0 * M_PI), 0);

                std::vector<int> ids;
                ids.push_back(0);ids.push_back(1);ids.push_back(2);ids.push_back(3);ids.push_back(4);ids.push_back(5);
                PolygonPoints p(ids, points);

                if(id > 0 && jd > 0 && id < N_ - 1 && jd < N_ -1)
                {
                    p.do_scale(0.99);
                    p.set_color();
                }
                else
                {
                    p.set_color(Vector3d(0, 0, 0));
                    fixed.push_back(ID);
                }
                P.push_back(p);
            }
        }
        build_connect(Conn);
        return;
    }

    void build_connect(vector<pair<int, int>> &Conn)
    {
        int ID;
        for(int id = 0; id < N_; id++)
        {
            for(int jd = 0;jd < N_; jd++)
            {
                ID = id * N_ + jd;

                //top
                if(id < N_ - 1)
                {
                    Conn.push_back(pair<int,int>(ID, ID + N_));
                    if(id & 1) Conn.push_back(pair<int,int>(ID, ID + N_ - 1));
                    else Conn.push_back(pair<int,int>(ID, ID + N_ + 1));
                }

                //right
                if(jd < N_ - 1)
                {
                    Conn.push_back(pair<int,int>(ID, ID + 1));
                }
            }
        }
        return;
    }

private:
    int N_;
    double r_;
};

#endif //TI_STABLE_RANDOMHEXAGONS2D_H
