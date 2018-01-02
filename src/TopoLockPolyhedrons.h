//
// Created by ziqwang on 02.01.18.
//

#ifndef TI_STABLE_TOPOLOCKPOLYHEDRONS_H
#define TI_STABLE_TOPOLOCKPOLYHEDRONS_H

#include "construct/Struc.h"
#include "PolyhedronsCollisionSolver.h"
#include "PolyhedraPoints.h"
#include "construct/PolyMesh.h"

class TopoLockPolyhedrons {

public:
    TopoLockPolyhedrons();

public:

    void loadSurface(char *path_name);

    void setPara(float tex, float til) {texViewSize_ = tex; tiltTheta_ = til;}

public:

    void createPolyhedrons(vecPlyhdrons &Ps);

    void set_solver(PolyhedronsCollisionSolver &solver);

private:
    Struc struc_;

    float texViewSize_;

    float tiltTheta_;
};


#endif //TI_STABLE_TOPOLOCKPOLYHEDRONS_H
