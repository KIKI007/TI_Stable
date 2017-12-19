//
// Created by ziqwang on 04.12.17.

#ifndef TI_STABLE_POLYHEDRA_H
#define TI_STABLE_POLYHEDRA_H
#include "PolygonBase.h"
#include <map>
#include <queue>
using std::vector;
using std::pair;
using std::queue;


class PolyhedraBase {

public:
    PolyhedraBase()
    {

    }
public:

    inline void add_faces(vector<PolygonBase> faces) { fv_ = faces; int_vv();int_e();}

    void set_color(Vector3d color) {
        for(auto& p: fv_)
        {
            p.color_ = color;
        }
    }

    inline void set_color() {
        for(auto& p: fv_)
        {
            p.color_ = Vector3d(1, 1, 0);
        }
    }

    inline void set_color_random()
    {
        for(auto& p: fv_)
        {
            p.color_ = Vector3d(rand()%255 / 255.0, rand()%255 / 255.0, rand()%255 / 255.0);
        }
    }

public:

    inline size_t nFace() { return fv_.size(); }

    inline void get_face(int ID, PolygonBase &face) { assert(0<= ID && ID < fv_.size());face = fv_[ID]; }

    void get_edges_id(vector<pair<int, int>> &edges);

    Vector3d get_color(int ID) { assert(0<= ID && ID < fv_.size()); return fv_[ID].color_; }

public:

    void triangulate(MatrixXi &F);

private:
    //initialize vertices-vertices which computes vertices' neighbor
    void int_vv();

    void int_e();

protected:
    vector<PolygonBase> fv_;
    vector< vector<int>> vv_;
    vector< pair<int, int>> e_;
};


#endif //TI_STABLE_POLYHEDRA_H
