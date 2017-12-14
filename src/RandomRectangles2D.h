//
// Created by ziqwang on 13.12.17.
//

#ifndef TI_STABLE_RANDOMRECTANGLES2D_H
#define TI_STABLE_RANDOMRECTANGLES2D_H


#include "PolygonPoints.h"
#include <string>
#include <igl/triangle/triangulate.h>
#include <vector>
#include <map>
using Eigen::RowVector2d;
using Eigen::Vector3d;
typedef  std::vector<PolygonPoints> vecPolys;
using std::vector;
using std::pair;
class RandomRectangles2D{

public:
    RandomRectangles2D()
    {
        len_ = 10;
        N_ = 10;
    }

public:

    void createRectangles(vecPolys &P, vector<pair<int, int>> &Conn, vector<int> &fixed)
    {
        //firstly add boundary
        add_boundary(P);

        for(int id = 0; id < N_; id++)
        {
            for(int jd = 0; jd < N_; jd++)
            {
                MatrixXd points(3, 4);
                points.col(0) = Vector3d(jd * len_ / N_, id * len_ / N_, 0);
                points.col(1) = Vector3d((jd + 1) * len_ / N_, id * len_ / N_, 0);
                points.col(2) = Vector3d((jd + 1) * len_ / N_, (id + 1) * len_ / N_, 0);
                points.col(3) = Vector3d( jd * len_ / N_, (id + 1) * len_ / N_, 0);

                std::vector<int> ids;
                ids.push_back(0);ids.push_back(1);ids.push_back(2);ids.push_back(3);
                PolygonPoints p(ids, points);
                p.do_scale(0.99);
                p.set_color();
                P.push_back(p);
            }
        }
        for(int id = 0; id < 4; id++)
            fixed.push_back(id);
        build_connect(Conn);
        return;
    }

    void add_boundary(vecPolys &P)
    {
        PolygonPoints P0 ,P1, P2, P3, P4;
        std::vector<int> ids;

        ids.clear();
        ids.push_back(0);ids.push_back(1);ids.push_back(2);ids.push_back(3);
        P0.set_ids(ids);
        MatrixXd points = MatrixXd(3, 4);
        points.col(0) = Vector3d(0, -1, 0);
        points.col(1) = Vector3d(len_, -1, 0);
        points.col(2) = Vector3d(len_, 0, 0);
        points.col(3) = Vector3d(0, 0, 0);
        P0.set_points(points);

        ids.clear();
        ids.push_back(0);ids.push_back(1);ids.push_back(2);ids.push_back(3);
        P1.set_ids(ids);
        points = MatrixXd(3, 4);
        points.col(0) = Vector3d(len_, 0, 0);
        points.col(1) = Vector3d(len_ + 1, 0, 0);
        points.col(2) = Vector3d(len_ + 1, len_, 0);
        points.col(3) = Vector3d(len_, len_, 0);
        P1.set_points(points);

        ids.clear();
        ids.push_back(0);ids.push_back(1);ids.push_back(2);ids.push_back(3);
        P2.set_ids(ids);
        points = MatrixXd(3, 4);
        points.col(0) = Vector3d(0, len_, 0);
        points.col(1) = Vector3d(len_, len_, 0);
        points.col(2) = Vector3d(len_, len_ + 1, 0);
        points.col(3) = Vector3d(0, len_ + 1, 0);
        P2.set_points(points);

        ids.clear();
        ids.push_back(0);ids.push_back(1);ids.push_back(2);ids.push_back(3);
        P3.set_ids(ids);
        points = MatrixXd(3, 4);
        points.col(0) = Vector3d(0, len_, 0);
        points.col(1) = Vector3d(-1, len_, 0);
        points.col(2) = Vector3d(-1, 0, 0);
        points.col(3) = Vector3d(0, 0, 0);
        P3.set_points(points);

        P.push_back(P0);
        P.push_back(P1);
        P.push_back(P2);
        P.push_back(P3);
    }

    void build_connect(vector<pair<int, int>> &Conn)
    {
        int ID;
        for(int id = 0; id < N_; id++)
        {
            for(int jd = 0;jd < N_; jd++)
            {
                ID = 4 + id * N_ + jd;
                if(id == 0) Conn.push_back(pair<int, int>(0, ID));
                if(jd == N_ - 1) Conn.push_back(pair<int, int>(1, ID));
                else Conn.push_back(pair<int, int>(ID + 1, ID));
                if(id == N_ - 1) Conn.push_back(pair<int, int>(2, ID));
                else Conn.push_back(pair<int, int>(ID + N_, ID));
                if(jd == 0) Conn.push_back(pair<int, int>(3, ID));

                if(jd < N_ - 1 && id < N_ - 1) Conn.push_back(pair<int, int>(ID + 1 + N_, ID));
                if(jd > 0 && id < N_ - 1) Conn.push_back(pair<int, int>(ID - 1 + N_, ID));
            }
        }
        return;
    }

private:
    int N_;
    int len_;
};


#endif //TI_STABLE_RANDOMRECTANGLES2D_H
