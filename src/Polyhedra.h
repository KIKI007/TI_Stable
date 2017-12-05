//
// Created by ziqwang on 04.12.17.
//

#ifndef TI_STABLE_POLYHEDRA_H
#define TI_STABLE_POLYHEDRA_H

#include "Polygon.h"
class Polyhedra {

public:
    Polyhedra()
    {

    }
public:

    void add_face(Polygon &face)
    {
        faces_.push_back(face);
    }

public:

    size_t nFace()
    {
        return faces_.size();
    }

    void get_face(int ID, Polygon &face)
    {
        assert(0<= ID && ID < faces_.size());
        face = faces_[ID];
    }

    void triangulate(Eigen::MatrixXi &F)
    {
        int nF = 0;
        for(int id = 0; id < faces_.size(); id++)
        {
            nF += faces_[id].nV() - 2;
        }

        F = MatrixXi::Zero(nF, 3);

        int idO = 0;
        for(int id = 0; id < faces_.size(); id++)
        {
            MatrixXi face;
            faces_[id].triangulate(face);
            F.block(idO, 0, faces_[id].nV() - 2, 3) = face;
            idO += faces_[id].nV() - 2;
        }
    }

private:
    std::vector<Polygon> faces_;
};


#endif //TI_STABLE_POLYHEDRA_H
