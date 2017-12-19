//
// Created by ziqwang on 15.12.17.
//

#include <ccd/ccd.h>
#include <testsuites/support.h>
#include <ccd/quat.h> // for work with quaternions
#include <iostream>
void ccdSupport(const void *_obj, const ccd_vec3_t *_dir,
                ccd_vec3_t *v)
{
    // Support function is made according to Gino van den Bergen's paper
    //  A Fast and Robust CCD Implementation for Collision Detection of
    //  Convex Objects

    ccd_obj_t *obj = (ccd_obj_t *)_obj;
    ccd_vec3_t dir;
    ccd_quat_t qinv;

    ccdVec3Copy(&dir, _dir);
    ccdQuatInvert2(&qinv, &obj->quat);

    ccdQuatRotVec(&dir, &qinv);

    if (obj->type == CCD_OBJ_BOX){
        ccd_box_t *box = (ccd_box_t *)obj;
        ccdVec3Set(v, ccdSign(ccdVec3X(&dir)) * box->x * CCD_REAL(0.5),
                   ccdSign(ccdVec3Y(&dir)) * box->y * CCD_REAL(0.5),
                   ccdSign(ccdVec3Z(&dir)) * box->z * CCD_REAL(0.5));
    }else if (obj->type == CCD_OBJ_SPHERE){
        ccd_sphere_t *sphere = (ccd_sphere_t *)obj;
        ccd_real_t len;

        len = ccdVec3Len2(&dir);
        if (len - CCD_EPS > CCD_ZERO){
            ccdVec3Copy(v, &dir);
            ccdVec3Scale(v, sphere->radius / CCD_SQRT(len));
        }else{
            ccdVec3Set(v, CCD_ZERO, CCD_ZERO, CCD_ZERO);
        }
    }else if (obj->type == CCD_OBJ_CYL){
        ccd_cyl_t *cyl = (ccd_cyl_t *)obj;
        ccd_real_t zdist, rad;

        zdist = dir.v[0] * dir.v[0] + dir.v[1] * dir.v[1];
        zdist = CCD_SQRT(zdist);
        if (ccdIsZero(zdist)){
            ccdVec3Set(v, CCD_ZERO, CCD_ZERO,
                       ccdSign(ccdVec3Z(&dir)) * cyl->height * CCD_REAL(0.5));
        }else{
            rad = cyl->radius / zdist;

            ccdVec3Set(v, rad * ccdVec3X(&dir),
                       rad * ccdVec3Y(&dir),
                       ccdSign(ccdVec3Z(&dir)) * cyl->height * CCD_REAL(0.5));
        }
    }

    // transform support vertex
    ccdQuatRotVec(v, &obj->quat);
    ccdVec3Add(v, &obj->pos);
}

void ccdObjCenter(const void *_obj, ccd_vec3_t *center)
{
    ccd_obj_t *obj = (ccd_obj_t *)_obj;

    ccdVec3Set(center, CCD_ZERO, CCD_ZERO, CCD_ZERO);
    // rotation is not needed
    ccdVec3Add(center, &obj->pos);
}

//int main(int argc, char *argv[])
//{
//    ccd_box_t *obj1, *obj2;
//    _ccd_vec3_t orgin;
//    obj1 = new ccd_box_t();
//    obj1->x = 10;obj1->y = 10;obj1->z = 10;
//    obj1->type = CCD_OBJ_BOX;
//    orgin.v[0] = 0;orgin.v[1] = 0; orgin.v[2] = 0;
//    ccdVec3Copy(&obj1->pos,&orgin);
//
//    obj2 = new ccd_box_t();
//    obj2->x = 10;obj2->y = 10;obj2->z = 10;
//    obj2->type = CCD_OBJ_BOX;
//    orgin.v[0] = 10;orgin.v[1] = 0; orgin.v[2] = 0;
//    ccdVec3Copy(&obj2->pos,&orgin);
//
//    ccd_t ccd;
//    CCD_INIT(&ccd); // initialize ccd_t struct
//
//    // set up ccd_t struct
//    ccd.support1       = ccdSupport; // support function for first object
//    ccd.support2       = ccdSupport; // support function for second object
//    ccd.max_iterations = 100;     // maximal number of iterations
//
//    ccd_real_t depth;
//    ccd_vec3_t dir;
//    ccd_vec3_t pos;
//    ccd_vec3_t sep;
//
//    //int intersect = ccdGJKSeparate(obj1, obj2, &ccd, &sep);
//    int intersect = ccdGJKSeparate(obj1, obj2, &ccd, &sep);
//    std::cout << intersect << std::endl;
////    std::cout << depth << std::endl;
////    std::cout << dir.v[0] << ", " << dir.v[1] << ", " << dir.v[2] << std::endl;
////    std::cout << pos.v[0] << ", " << pos.v[1] << ", " << pos.v[2] << std::endl;
//    std::cout << sep.v[0] << ", " << sep.v[1] << ", " << sep.v[2] << std::endl;
//
//    ccd_vec3_t v0, v1;
//    ccdSupport(obj1, &dir, &v0);
//    ccdVec3Scale(&dir, -1);
//    ccdSupport(obj2, &dir, &v1);
//    std::cout << v0.v[0] << ", " << v0.v[1] << ", " << v0.v[2] << std::endl;
//    std::cout << v1.v[0] << ", " << v1.v[1] << ", " << v1.v[2] << std::endl;
//    // now intersect holds true if obj1 and obj2 intersect, false otherwise
//}