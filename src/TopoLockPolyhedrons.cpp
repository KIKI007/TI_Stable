//
// Created by ziqwang on 02.01.18.
//

#include "TopoLockPolyhedrons.h"
#include "construct/Part.h"
#include "construct/Polygon.h"

TopoLockPolyhedrons::TopoLockPolyhedrons() {
    struc_.Clear();

    texViewSize_ =  5;

    tiltTheta_ = 35.264389;
}

void TopoLockPolyhedrons::loadSurface(char *path_name) {
    struc_.LoadSurface(path_name);
}

void TopoLockPolyhedrons::createPolyhedrons(vecPlyhdrons &Ps) {

    Ps.clear();
    if(struc_.surface != NULL)
    {
        struc_.ResetModel();
        struc_.InitCrossMesh2D(CROSS_HEX, 16);
        struc_.CreateStructure_Arch(texViewSize_, tiltTheta_);
        struc_.BuildPartsGraph_Arch();
        struc_.IdentifyBoundaryParts();

        for(int kd = 0; kd < struc_.partList.size(); kd++)
        {
            Part *part = struc_.partList[kd];
            PolyMesh *mesh = part->polyMesh;
            PolyhedraPoints poly;
            vector<PolygonBase> faces;

            MatrixXd points(3, mesh->verList.size());
            for(int id = 0; id < mesh->verList.size(); id++)
            {
                points.col(id) = Vector3d(mesh->verList[id].x,mesh->verList[id].y,mesh->verList[id].z);
            }
            poly.set_points(points);

            for(int id = 0; id < mesh->polyList.size(); id++)
            {
                PolygonBase face;
                Polygon *polygon = mesh->polyList[id];
                face.set_ids(polygon->verIDs);
                faces.push_back(face);
            }
            poly.add_faces(faces);
            poly.set_color();
            Ps.push_back(poly);
        }
    }

    return;
}

void TopoLockPolyhedrons::set_solver(PolyhedronsCollisionSolver &solver) {
    if(!struc_.partList.empty())
    {
        for(int kd = 0; kd < struc_.partList.size(); kd++)
        {
            Part *part = struc_.partList[kd];
            if(part->atBoundary) solver.setFixed(kd);
            for(int id = 0; id < part->initNeighbors.size(); id++)
            {
                Part *partB = part->initNeighbors[id];
                if(partB == NULL) continue;
                if(part->partID < partB->partID)
                {
                    solver.setConnection(part->partID, partB->partID);
                }
            }
        }
    }

    return;
}
