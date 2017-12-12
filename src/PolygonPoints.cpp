//
// Created by ziqwang on 12.12.17.
//

#include "PolygonPoints.h"


void PolygonPoints::sat_project(std::vector<double> &proj, Vector3d &axis, Vector3d &ori){
    for(int id = 0; id < points_.cols(); id++)
    {
        proj.push_back(axis.dot(points_.col(id) - ori));
    }
    return;
}

void PolygonPoints::get_list_min(std::vector<double> &proj, double &min, int &idx)
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

bool PolygonPoints::collision(PolygonPoints &B,
               Vector3d &n,
               bool &nA,
               int &pa,
               int &pb)
//Compute the collision info
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
        B.sat_project(proj, axis, ori);

        double min; int min_id;
        get_list_min(proj, min, min_id);

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
        sat_project(proj, axis, ori);

        double min; int min_id;
        get_list_min(proj, min, min_id);

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

bool PolygonPoints::collision(PolygonPoints    &B,
                              Vector3d         &n,
                              bool             &nA,
                              int              &pa,
                              int              &pb,
                              int              &pc,
                              double           eps)
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

bool PolygonPoints::collision(PolygonPoints &B,
               Vector3d &n,
               bool &nA,
               int &Ia,
               int &Ib,
               double &muA,
               double &muB)
{
    bool is_collision = collision(B, n, nA, Ia, Ib);
    Vector3d c0, c1, c2;

    c0 = point(Ia) - point_next(Ia);
    c1 = -B.point(Ib) + B.point_next(Ib);
    c2 = point_next(Ia) - B.point_next(Ib);

    double eps = 1e-8;
    if(std::abs(n.dot(c1)) < eps &&  std::abs(n.dot(c0)) < eps)
    {
        Eigen::SparseMatrix<double> Q(2, 2), A(0, 2);
        std::vector<T> triplist;
        triplist.push_back(T(0, 0, 2 * c0.dot(c0)));
        triplist.push_back(T(1, 0, 2 * c1.dot(c0)));
        triplist.push_back(T(1, 1, 2 * c1.dot(c1)));
        Q.setFromTriplets(triplist.begin(), triplist.end());

        Eigen::VectorXd c(2);
        c << c0.dot(c2), c1.dot(c2);

        double cf = c2.dot(c2);

        Eigen::VectorXd lc(0, 1);
        Eigen::VectorXd uc(0, 1);
        Eigen::VectorXd lx(2);
        Eigen::VectorXd ux(2);
        lx << 0, 0;
        ux << 1, 1;

        Eigen::VectorXd ans(2);

        igl::mosek::MosekData mosek_data;
        igl::mosek::mosek_quadprog(Q, c , cf, A, lc, uc, lx , ux, mosek_data, ans);

        muA = ans(0);
        muB = ans(1);
    }
    else
    {
        if(nA)
        {
            c0 = point(Ia);
            c1 = point_next(Ia);
            c2 = B.point(Ib);
            muA = (c2 - c1).dot(c0 - c1) / (c0 - c1).dot(c0 - c1);
            muB = 1;

        }
        else
        {
            c0 = B.point(Ib);
            c1 = B.point_next(Ib);
            c2 = point(Ia);
            muA = 1;
            muB = (c2 - c1).dot(c0 - c1) / (c0 - c1).dot(c0 - c1);
        }
    }
    return is_collision;
}