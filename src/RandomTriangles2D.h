//
// Created by ziqwang on 11.12.17.
//

#ifndef TI_STABLE_RANDOMTRIANGLES2D_H
#define TI_STABLE_RANDOMTRIANGLES2D_H

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
class RandomTriangles2D{

public:
    RandomTriangles2D()
    {
        len_ = 10;
    }

public:

    void createTriangles(vecPolys &P, vector<pair<int, int>> &Conn)
    {
        //firstly add boundary
        add_boundary(P);

        //add triangles
        MatrixXd rV(4, 2);
        MatrixXi E(4, 2);
        MatrixXd H(0, 2);
        rV.row(0) = RowVector2d(0, 0);
        rV.row(1) = RowVector2d(len_, 0);
        rV.row(2) = RowVector2d(len_, len_);
        rV.row(3) = RowVector2d(0, len_);

        E <<    0,  1,
                1,  2,
                2,  3,
                3,  0;

        MatrixXd V;
        MatrixXi F;
        std::string flag = "-p -q30 -a1";
        igl::triangle::triangulate(rV, E, H, flag, V, F);
        PolygonPoints p;
        std::vector<int> ids;
        for(int id = 0; id < F.rows(); id++)
        {
            p.clear();
            ids.clear();
            ids.push_back(0);ids.push_back(1);ids.push_back(2);
            p.set_ids(ids);
            MatrixXd points = MatrixXd(3, 3);
            for(int jd = 0; jd < 3; jd++)
            {
                Vector3d point = Vector3d(V(F(id, jd), 0),V(F(id, jd), 1), 0);
                points.col(jd) = point;
            }
            p.set_points(points);
            p.do_scale(0.90);
            p.set_color();
            P.push_back(p);
        }


        build_connect(Conn, P, F);


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

    void build_connect(vector<pair<int, int>> &Conn, vecPolys &P, MatrixXi &F)
    {
        for(int id = 0; id < F.rows(); id++)
        {
            for(int jd = id + 1;jd < F.rows(); jd++)
            {
                int is_zero = 0;
                for(int kd = 0; kd < 3 && !is_zero; kd++)
                {
                    for(int ld = 0; ld < 3; ld++)
                    {
                        if(F(id, kd) == F(jd, ld))
                        {
                            is_zero = 1;
                            break;
                        }
                    }
                }
                if(is_zero == 1)
                    Conn.push_back(pair<int, int>(id + 4, jd + 4));
            }
        }
        for(int id = 4; id< P.size();id++)
        {
            Vector3d center; P[id].get_center(center);
            if(center[1] <= 1)
                Conn.push_back(pair<int, int>(0, id));
            if(center[0] >= len_ - 1)
                Conn.push_back(pair<int, int>(1, id));
            if(center[1] >= len_ - 1)
                Conn.push_back(pair<int, int>(2, id));
            if(center[0] <= 1)
                Conn.push_back(pair<int, int>(3, id));
        }

        return;
    }

private:
    int N_;
    int len_;
};

#endif //TI_STABLE_RANDOMTRIANGLES2D_H
