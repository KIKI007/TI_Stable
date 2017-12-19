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

bool PolyhedraPoints::collision(PolyhedraPoints  &B,
               Vector3d         &nA,
               Vector3d         &nB,
               vector<int>      &pa,
               vector<int>      &pb)
{
    return true;
}

void PolyhedraPoints::candidate_sat(PolyhedraPoints &B, MatrixXd *axis)
{

}