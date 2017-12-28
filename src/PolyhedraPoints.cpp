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
        Vector3d u = norm_cross_from_polyhedrons(axisA[id], axisB[id]);
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
            axisA[kd] = axisA[id]; axisA[kd].normalized();
            axisB[kd] = axisB[id];axisB[kd].normalized();
            axis[kd] = axis[id];axis[kd].normalized();
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
    //n^T(q - p)
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
                if(!axisA[id].isZero())
                {
                    nA = axisA[id] * (kd ? 1 : -1);
                    nB = axisB[id];
                }
                else
                {
                    nA = axisA[id];
                    nB = axisB[id] * (kd ? 1 : -1);
                }

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

double PolyhedraPoints::approximate_signed_distance(PolyhedraPoints &B, VectorXd &x) {

    PolyhedraPoints &A = (*this);
    Vector3d nA, nB;
    Vector3d cA, cB;
    Vector3d pA, pB;
    Vector3d uA, uB;

    vecVector3d pAs, pBs;
    double signed_dist = 0, dist = 0;

    collision(B, nA, nB, pAs, pBs, signed_dist);

    Vector3d n = norm_cross_from_polyhedrons(nA, nB);

    //std::cout << nA.transpose() << std::endl << nB.transpose() << std::endl;
    //std::cout << n.dot(pAs[0] - pBs[0]) - signed_dist << std::endl;

    A.get_center(cA); B.get_center(cB);
    vector<double> approx_dist;

    for(int id = 0; id < pAs.size(); id++)
    {
        for(int jd = 0; jd < pBs.size(); jd++)
        {
            dist = n.dot(pAs[id] - pBs[jd]);
            pA = pAs[id] - cA;
            pB = pBs[jd] - cB;
            uA = x(0)*x.segment(1, 3);
            uB = x(7)*x.segment(8, 3);

            dist += pA.cross(n).dot(uA);
            dist -= pB.cross(n).dot(uB);
            dist += n.dot(x.segment(4, 3) - x.segment(11, 3));

            if(nA.isZero())
            {
                //dist += (-nB).cross(uB).dot(pAs[id] - pBs[jd]);
                dist += n.cross(pAs[id] - pBs[jd]).dot(uB);
            }
            else if(nB.isZero())
            {
                //dist += (-nA).cross(uA).dot(pAs[id] - pBs[jd]);
                dist += n.cross(pAs[id] - pBs[jd]).dot(uA);
            }
            else
            {
                //dist += (-nA).cross(nB.cross(uB)).dot(pAs[id] - pBs[jd]);
                //dist += (-nA).cross(uA).cross(nB).dot(pAs[id] - pBs[jd]);
                dist += (nA.cross(pAs[id] - pBs[jd]).cross(nB)).dot(uB);
                dist += ((pAs[id] - pBs[jd]).cross(nB).cross(nA)).dot(uA);
            }

            approx_dist.push_back(dist);
        }
    }

    if(nA.isZero())
    {
        std::cout << "approxB n:\t" << (nB - (nB).cross(uB)).transpose() << std::endl;
    }
    else if(nB.isZero())
    {
        std::cout << "approxA n:\t" << (nA - (nA).cross(uA)).transpose() << std::endl;
    }
    else
    {
        std::cout << "approxAB n:\t" << (n  + (-nA).cross(nB.cross(uB)) + (-nA).cross(uA).cross(nB)).transpose() << std::endl;
    }

    double ans = approx_dist[0];
    for(double x: approx_dist) ans = x < ans ? x : ans;

    return ans;
}

Vector3d PolyhedraPoints::norm_cross_from_polyhedrons(Vector3d na, Vector3d nb) {
    VectorXd u =  na.isZero() ? (nb) : (nb.isZero() ? na : na.cross(nb));
    u.normalize();
    return u;
}

bool PolyhedraPoints::collision(PolyhedraPoints &B, double &signed_dist) {
    Vector3d nA, nB;
    vecVector3d pAs, pBs;
    return collision(B, nA, nB, pAs, pBs, signed_dist);
}
