//
// Created by ziqwang on 22.11.17.
//


#include "TI_Cube.h"

TI_Cube::TI_Cube() {
    init(0, 0);
}

TI_Cube::~TI_Cube() {

}

void TI_Cube::init(int m, int n) {
    num_cubes_col_ = m;
    num_cubes_row_ = n;
    tolerance_ = 0.8;
    cube_length_ = 1;
}

void TI_Cube::shrink(MatrixXd &mat, double ratio)
{
    Eigen::RowVector3d centroid(0, 0, 0);
    for(int id = 0; id < mat.rows(); id++)
    {
        centroid += mat.row(id);
    }
    centroid /= mat.rows();

    for(int id = 0; id < mat.rows(); id++)
    {
        mat.row(id) = (mat.row(id) - centroid) * ratio + centroid;
    }
}

void TI_Cube::generate_1v6(std::vector<PolyhedraPoints> &plist)
{
    MatrixXd mat(3, 3);

    Vector3d X, Y, Z;
    Vector3d O, O1, X1, Y1, Z1;
    O << 0, 0, 0;
    X << 1, 0, 0;
    Y << 0, 1, 0;
    Z << 0, 0, 1;

    O1 << 0.5, 0.5, 0.5;
    X1 << 0.5, 0.5, -1; X1.normalize();
    Y1 << -0.5, 0.5, 0; Y1.normalize();
    Z1 = X1.cross(Y1);  Z1.normalize();

    mat.col(0) = X1.transpose();
    mat.col(1) = Y1.transpose();
    mat.col(2) = Z1.transpose();

    mat = mat.inverse();

    X = mat * X; X.normalize();
    Y = mat * Y; Y.normalize();
    Z = mat * Z; Z.normalize();

    int dy[7] = {0, 0,  1, 1, 0, -1, -1};
    int dx[7] = {0, -1,-1, 0, 1,  1,  0};
    for(int k = 0; k < 7; k++)
    {
        int i = dx[k];
        int j = dy[k];
        double r = cube_length_ * sqrt(2) / 2;
        Vector3d O(j * sqrt(3) * r + i * sqrt(3)/ 2.0 * r,
                i * 1.5 * r,
                0);
        PolyhedraPoints P;
        cube_OXYZ(O,
                  X * cube_length_,
                  Y * cube_length_,
                  Z * cube_length_,
                  P);
        if(k == 0) P.shrink(tolerance_);
        plist.push_back(P);
    }
}

void TI_Cube::generate(std::vector<PolyhedraPoints> &plist) {

    MatrixXd mat(3, 3);

    Vector3d X, Y, Z;
    Vector3d O, O1, X1, Y1, Z1;
    O << 0, 0, 0;
    X << 1, 0, 0;
    Y << 0, 1, 0;
    Z << 0, 0, 1;

    O1 << 0.5, 0.5, 0.5;
    X1 << 0.5, 0.5, -1; X1.normalize();
    Y1 << -0.5, 0.5, 0; Y1.normalize();
    Z1 = X1.cross(Y1);  Z1.normalize();

    mat.col(0) = X1.transpose();
    mat.col(1) = Y1.transpose();
    mat.col(2) = Z1.transpose();

    mat = mat.inverse();

    X = mat * X;
    Y = mat * Y;
    Z = mat * Z;

    for(int i = 0; i < num_cubes_row_; i++)
    {
        for(int j = 0; j< num_cubes_col_; j++)
        {
            double r = cube_length_ * sqrt(2) / 2;
            Vector3d O(j * sqrt(3) * r + (-1) * (i&1) * sqrt(3)/ 2.0 * r,
                       i * 1.5 * r,
                       0);
            MatrixXd cV; PolyhedraPoints P;MatrixXi cF;
            cube_OXYZ(O,
                      X * cube_length_ * tolerance_,
                      Y * cube_length_ * tolerance_,
                      Z * cube_length_ * tolerance_,
                      P);
            plist.push_back(P);
        }
    }
}



TI_Cube::TI_Cube(int m, int n) {
    init(m, n);
}

void TI_Cube::cube_OXYZ(Vector3d O, Vector3d X, Vector3d Y, Vector3d Z, PolyhedraPoints &P) {

    //non-zero vectors
    assert(X.norm() > TI_zero_eps_ && Y.norm() > TI_zero_eps_ && Z.norm() > TI_zero_eps_);

    //perpendicular
    assert(std::abs(X.dot(Y)) < TI_zero_eps_
           && std::abs(Y.dot(Z)) < TI_zero_eps_
           && std::abs(Z.dot(X)) < TI_zero_eps_ );

    MatrixXd cV = MatrixXd::Zero(3, 8);
    cV.col(0) = O;
    cV.col(1) = (O + X);
    cV.col(2) = (O + Y);
    cV.col(3) = (O + Z);
    cV.col(4) = (O + X + Y);
    cV.col(5) = (O + X + Z);
    cV.col(6) = (O + Y + Z);
    cV.col(7) = (O + X + Y + Z);

    P.set_points(cV);

    vector<PolygonBase> polys;
    PolygonBase polygon;
    std::vector<int> ids;

    ids.push_back(0);ids.push_back(2);
    ids.push_back(4);ids.push_back(1);
    polygon.set_ids(ids);polys.push_back(polygon);

    ids.clear();
    ids.push_back(1);ids.push_back(4);
    ids.push_back(7);ids.push_back(5);
    polygon.set_ids(ids);polys.push_back(polygon);

    ids.clear();
    ids.push_back(1);ids.push_back(5);
    ids.push_back(3);ids.push_back(0);
    polygon.set_ids(ids);polys.push_back(polygon);

    ids.clear();
    ids.push_back(0);ids.push_back(3);
    ids.push_back(6);ids.push_back(2);
    polygon.set_ids(ids);polys.push_back(polygon);

    ids.clear();
    ids.push_back(3);ids.push_back(5);
    ids.push_back(7);ids.push_back(6);
    polygon.set_ids(ids);polys.push_back(polygon);

    ids.clear();
    ids.push_back(2);ids.push_back(6);
    ids.push_back(7);ids.push_back(4);
    polygon.set_ids(ids);polys.push_back(polygon);

    P.add_faces(polys);
    P.set_color(Vector3d(1, 1, 0));
//    cF <<   0, 2, 4,
//            4, 1, 0,
//            1, 4, 7,
//            7, 5, 1,
//            1, 5, 3,
//            3, 0, 1,
//            0, 3, 6,
//            6, 2, 0,
//            3, 5, 7,
//            7, 6, 3,
//            2, 6, 7,
//            7, 4, 2;
}
