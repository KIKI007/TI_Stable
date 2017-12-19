//
// Created by ziqwang on 17.12.17.
//
#include "PolyhedraPoints.h"

void PolyhedraPoints::shrink(double ratio)
{
    Vector3d center;
    get_center(center);
    for(int id = 0; id < points_.cols(); id++)
    {
        points_.col(id) = (points_.col(id) - center) * ratio + center;
    }
    return;
}

int PolyhedraPoints::get_edges(vecVector3d &edges) {
    vector<pair<int, int>> edgeIDs;
    get_edges_id(edgeIDs);
    for(int id = 0; id < edgeIDs.size(); id++)
        edges.push_back(point(edgeIDs[id].first) - point(edgeIDs[id].second));
    return edgeIDs.size();
}

void PolyhedraPoints::get_normals(vecVector3d &normals) {
    for(int id = 0; id < nFace(); id++)
    {
        normals.push_back(Vector3d(0, 0, 0));
        get_normal(id, normals.back());
    }
    return;
}

void PolyhedraPoints::candidate_sat(PolyhedraPoints &B,
                                    vecVector3d &axis,
                                    vecVector3d &axisA,
                                    vecVector3d &axisB)
{
    axis.clear();
    axisA.clear();
    axisB.clear();
    PolyhedraPoints &A = (*this);

    //only A normal
    get_normals(axisA);
    axisB.insert(axisB.end(), A.nFace(), Vector3d(0, 0, 0));

    //only B normal
    axisA.insert(axisA.end(), B.nFace(), Vector3d(0, 0, 0));
    B.get_normals(axisB);

    //A and B's edge
    vecVector3d Aedges;
    A.get_edges(Aedges);
    for(int id = 0; id < Aedges.size(); id++)
    {
        int nE = B.get_edges(axisB);
        axisA.insert(axisA.end(), nE, Aedges[id]);
    }

    //build aixs
    for(int id = 0; id < axisA.size(); id++)
    {
        Vector3d u = axisA[id].isZero() ? (axisB[id]) : (axisB[id].isZero() ? axisA[id] : axisA[id].cross(axisB[id]));
        //std::cout << axisA[id].transpose() << "\t" <<  axisB[id].transpose() << std::endl;
        axis.push_back(u);
    }

    //delete the candidate
    int kd = 0;
    double eps_ = 1e-8;
    for(int id = 0; id < axis.size(); id++)
    {
        int jd = 0;
        for(jd = 0; jd < kd; jd++) {
            if(std::abs((axis[id].cross(axis[jd])).norm()) < eps_)
            {
                break;
            }
        }
        if(jd == kd)
        {
            axisA[kd] = axisA[id];
            axisB[kd] = axisB[id];
            axis[kd] = axis[id];
            kd++;
        }
    }
    axis.erase(axis.begin() + kd, axis.end());
    axisA.erase(axisA.begin() + kd, axisA.end());
    axisB.erase(axisB.begin() + kd, axisB.end());

    return;
}

bool PolyhedraPoints::collision(PolyhedraPoints &B, Vector3d &nA, Vector3d &nB, vecVector3d &pa, vecVector3d &pb, double &signed_dist)
{
    bool is_collision = true;

    PolyhedraPoints &A = *this;
    vecVector3d axis, axisA, axisB;
    vecVector3d p,q;

    candidate_sat(B, axis, axisA, axisB);

    Vector3d n;
    signed_dist = -std::numeric_limits<double>::infinity();
    double dist = 0;
    for(int id = 0; id < axis.size(); id++)
    {
        for(int kd = 0; kd < 2; kd++)
        {
            p.clear();q.clear();
            n = axis[id] * (kd ? -1 : 1);
            dist = - A.max_project(n, p) - B.max_project(n * (-1), q);
            if(dist > 0) is_collision = false;
            if(signed_dist < dist)
            {
                pa = p;
                pb = q;
                nA = axisA[id];
                nB = axisB[id];
                signed_dist = dist;
            }
        }
    }

    return is_collision;
}

double PolyhedraPoints::max_project(Vector3d n, vecVector3d &p, double tol)
{
    p.clear();
    vector<pair<double, int>> proj;
    for(int id = 0; id < nV(); id++)
    {
        pair<double, int> dat;
        dat.first = n.dot(point(id));
        dat.second = id;
        proj.push_back(dat);
    }

    std::sort(proj.begin(), proj.end(), [](const pair<double, int> &a, const pair<double, int> &b){
        return a.first > b.first;
    });

    for(int id = 0; id < proj.size(); id++)
    {
        if(proj.front().first - proj[id].first < tol)
            p.push_back(point(proj[id].second));
    }
    return proj.front().first;
}

void PolyhedraPoints::transformation(Quaterniond quat, Vector3d v)
{
    Vector3d center;
    get_center(center);
    for(int id = 0; id < nV(); id++)
    {
        Quaterniond qp(0, points_(0, id) - center[0], points_(1, id) - center[1], points_(2, id) - center[2]);
        qp = quat*qp*quat.inverse();
        Vector3d p(qp.x() + center[0] + v[0], qp.y() + center[1] + v[1], qp.z() + center[2] + v[2]);
        points_.col(id) = p;
    }

    return;
}