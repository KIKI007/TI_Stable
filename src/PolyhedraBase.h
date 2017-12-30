//
// Created by ziqwang on 04.12.17.

#ifndef TI_STABLE_POLYHEDRA_H
#define TI_STABLE_POLYHEDRA_H

#include <map>
#include <queue>

#include "PolygonBase.h"

using std::vector;
using std::pair;
using std::queue;

class PolyhedraBase {

public:
    PolyhedraBase() {}
public:

    //please add all faces in once
        //it also creats neighbouring information as well as edge info
    inline void add_faces(vector<PolygonBase> faces) { fv_ = faces;
        init_VV();
        init_E();}

public:
    //add color
    void set_color(Vector3d color) { for(auto& p: fv_) p.color_ = color; }

    //add color: yellow
    inline void set_color() { for(auto& p: fv_) p.color_ = Vector3d(1, 1, 0); }

    //add random color
    inline void set_color_random() { for(auto& p: fv_) p.color_ = Vector3d(rand()%255 / 255.0, rand()%255 / 255.0, rand()%255 / 255.0); }

public:

    inline size_t nFace() { return fv_.size(); }

    inline void get_face(int ID, PolygonBase &face) { assert(0<= ID && ID < fv_.size());face = fv_[ID]; }

    inline Vector3d get_color(int ID) { assert(0<= ID && ID < fv_.size()); return fv_[ID].color_; }

    void get_edges_list(vector<pair<int, int>> &edges);

public:

    void do_triangulate(MatrixXi &F);

private:

    //initialize vertices-vertices neighbouring information
        //vertex id's neighbor is {vv_[id][jd]}
    void init_VV();

    //initialize edges' information
        //e_[id] = (a, b) represents an edge from a to b
        //it has no direction.
    void init_E();

protected:

    vector< PolygonBase   > fv_;

    vector< vector<int>   > vv_;

    vector< pair<int, int>> e_;
};


#endif //TI_STABLE_POLYHEDRA_H
