//
// Created by ziqwang on 04.12.17.

#ifndef TI_STABLE_POLYHEDRA_H
#define TI_STABLE_POLYHEDRA_H
#include "PolygonBase.h"
using std::vector;
class PolyhedraBase {

public:
    PolyhedraBase()
    {

    }
public:

    inline void add_faces(vector<PolygonBase> faces) { fv_ = faces; int_vv();}

public:

    inline size_t nFace() { return fv_.size(); }

    inline void get_face(int ID, PolygonBase &face) { assert(0<= ID && ID < fv_.size());face = fv_[ID]; }

public:

    void triangulate(MatrixXi &F);

private:
    //initialize vertices-vertices which computes vertices' neighbor
    void int_vv();

protected:
    vector<PolygonBase> fv_;
    vector< vector<int>> vv_;
};


#endif //TI_STABLE_POLYHEDRA_H
