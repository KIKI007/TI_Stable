//
// Created by ziqwang on 04.12.17.
//

#ifndef TI_STABLE_POLYHEDRA_H
#define TI_STABLE_POLYHEDRA_H

#include "PolygonBase.h"
class PolyhedraBase {

public:
    PolyhedraBase()
    {

    }
public:

    void add_face(PolygonBase &face)
    {
        ids_.push_back(face);
    }

    size_t nFace()
    {
        return ids_.size();
    }

    void get_face(int ID, PolygonBase &face)
    {
        assert(0<= ID && ID < ids_.size());
        face = ids_[ID];
    }

public:

    void triangulate(MatrixXi &F)
    {
        int nF = 0;
        for(int id = 0; id < ids_.size(); id++)
        {
            nF += ids_[id].nV() - 2;
        }

        F = MatrixXi::Zero(nF, 3);

        int idO = 0;
        for(int id = 0; id < ids_.size(); id++)
        {
            MatrixXi face;
            ids_[id].triangulate(face);
            F.block(idO, 0, ids_[id].nV() - 2, 3) = face;
            idO += ids_[id].nV() - 2;
        }
    }

protected:
    std::vector<PolygonBase> ids_;
};


#endif //TI_STABLE_POLYHEDRA_H
