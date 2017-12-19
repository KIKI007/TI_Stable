//
// Created by ziqwang on 18.12.17.
//

#include "PolyhedraBase.h"

void PolyhedraBase::triangulate(MatrixXi &F)
{
    int nF = 0;
    for(int id = 0; id < fv_.size(); id++)
    {
        nF += fv_[id].nV() - 2;
    }

    F = MatrixXi::Zero(nF, 3);

    int idO = 0;
    for(int id = 0; id < fv_.size(); id++)
    {
        MatrixXi face;
        fv_[id].triangulate(face);
        F.block(idO, 0, fv_[id].nV() - 2, 3) = face;
        idO += fv_[id].nV() - 2;
    }
}

void PolyhedraBase::int_vv(){
    int nV = 0;
    for(int id = 0; id < fv_.size(); id++)
    {
        for(int jd = 0; jd < fv_[id].nV(); jd++)
        {
            if(nV < fv_[id].ids_[jd])
                nV = fv_[id].ids_[jd];
        }
    }

    nV = nV + 1; //index is from 0 ~ (nV - 1)

    vv_.resize(nV);
    for(int id = 0; id < nV; id++)
    {
        vector<bool> visited;
        visited.resize(nV, false);
        for(int jd = 0; jd < fv_.size(); jd++)
        {
            for(int kd = 0; kd < fv_[jd].nV(); kd++)
            {
                int sta, end;
                fv_[jd].EDGE(kd, sta, end);
                if(sta == id && visited[end] == false)
                {
                    vv_[id].push_back(end);
                    visited[end] = true;
                }
                if(end == id && visited[sta] == false)
                {
                    vv_[id].push_back(sta);
                    visited[sta] = true;
                }
            }
        }
    }

    return;
}