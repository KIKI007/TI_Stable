//
// Created by ziqwang on 08.12.17.
//

#include "PolygonsCollisionSolver.h"

PolygonsCollisionSolver::PolygonsCollisionSolver()
{

}

void PolygonsCollisionSolver::setPolygons(vecPolys &P)
{
    P_ = P;
}

void PolygonsCollisionSolver::setConnection(int a, int b) {
    Conn_.push_back(std::pair<int, int>(a, b));
}

bool PolygonsCollisionSolver::get_a_coeff(VectorXd &a, vector<PolygonPoints> &P_ORI, VectorXd &x, double &f_xk)
//a is the coefficient of penetration distance function
//P_ORI is two collision polygons in its origin position
//x is present rotation and translation
//x(0 + 3 * i) is theta (rotation angle) in Pi
//x(1 + 3 * i) is x (translation in x axis) in Pi
//x(2 + 3 * i) is y (translation in y axis) in Pi
//f_xk is the penetration distance
{
    Vector3d x0[2];
    x0[0] = x.segment(0, 3);
    x0[1] = x.segment(3, 3);

    Vector3d ct_ori[2];
    P_ORI[0].center_points(ct_ori[0]);
    P_ORI[1].center_points(ct_ori[1]);

    PolygonPoints P[2];
    P[0] = P_ORI[0];P[0].Rotate_translate(x0[0]);
    P[1] = P_ORI[1];P[1].Rotate_translate(x0[1]);

    /************ collision info **************/

    int Ia, Ib;
    bool na;
    Vector3d n, p[2], p_ori[2];
    if(!P[0].collision(P[1], n, na, Ia, Ib))
        return false;

    p[0]        = P[0].point(Ia);
    p[1]        = P[1].point(Ib);
    p_ori[0]    = P_ORI[0].point(Ia);
    p_ori[1]    = P_ORI[1].point(Ib);

    f_xk = std::abs(n.dot(p[0] - p[1]));

    /****** penetration distance info *********/
    a = VectorXd::Zero(7);

    //add n1
    double theta_na = (na ? x(0) : x(3));
    double extra = 0;
    Vector3d n1(-n(1), n(0), 0);
    for(int id = 0; id < 2; id++)
    {
        double theta_0 = x(3 * id);
        double x_0 = x(3 * id + 1);
        double y_0 = x(3 * id + 2);
        MatrixXd dR_theta(3, 3);
        dR_theta << -std::sin(theta_0)    ,   -std::cos(theta_0)   ,     0,
                std::cos(theta_0)    ,   -std::sin(theta_0)   ,     0,
                0                ,                      0   , 1;

        int sign = (id & 1 ? -1 : 1);
        a(3 * id + 1) = n.dot(dR_theta * (p_ori[id] - ct_ori[id])) * sign;

        //a2
        a(3 * id + 2) = n(0) * sign;

        //a3
        a(3 * id + 3) = n(1) * sign;

        //a0
        a(0) +=  (sign) * (n. dot(dR_theta * (p_ori[id] - ct_ori[id])) * (-theta_0) - n (0) * x_0 -  n(1) * y_0);
    }
    a(0)  += (n - theta_na * n1).dot(p[0] - p[1]) ;
    extra += n1.dot(p[0] - p[1]);

    if(na) a(1) += extra;
    else   a(4) += extra;
    return true;
}

double PolygonsCollisionSolver::penetration_distance(vecPolys p, VectorXd &x)
{
    Vector3d x0 = x.segment(0, 3);
    Vector3d x1 = x.segment(3, 3);
    p[0].Rotate_translate(x0);
    p[1].Rotate_translate(x1);

    Vector3d n;
    int Ia, Ib;
    bool na;
    if(p[0].collision(p[1], n, na, Ia, Ib))
        return std::abs(n.dot(p[0].point(Ia) - p[1].point(Ib)));
    else
        return 0;
}

bool PolygonsCollisionSolver::solve_linear(std::vector<T> &triplist,
                                           std::vector<double> &Ac,
                                           std::vector<bool> &in_opt,
                                           VectorXd &x0,
                                           VectorXd &x,
                                           int id_term,
                                           int id_extra,
                                           double dx,
                                           double &mk_pk)
{
    Eigen::VectorXd c = VectorXd::Zero(id_extra);
    for(int id = P_.size() * 3; id < id_extra; id++)
    {
        c(id) = 1;
    }

    Eigen::SparseMatrix<double> A(id_term, id_extra);
    A.setFromTriplets(triplist.begin(), triplist.end());

    Eigen::VectorXd lc(id_term), uc(id_term);
    for(int id = 0; id < Ac.size(); id++) lc(id) = uc(id) = Ac[id];

    Eigen::VectorXd lx(id_extra), ux(id_extra);
    for(int id = 0; id < id_extra; id++)
    {
        if(id < 3 * P_.size())
        {
            if(in_opt[id / 3])
            {
                lx(id) = x0(id) - dx;
                ux(id) = x0(id) + dx;
            }
            else
            {
                lx(id) = x0(id);
                ux(id) = x0(id);
            }
        }
        else
        {
            lx(id) = 0;
            ux(id) = std::numeric_limits<double>::infinity();
        }
    }

    Eigen::VectorXd ans(id_extra);
    x = VectorXd(P_.size() * 3);
    bool sucess = igl::mosek::mosek_linprog(c, A, lc, uc, lx, ux, ans);

    for(int id = 0; id < P_.size() * 3; id++)
        x(id) = ans(id);

    mk_pk = 0;
    for(int id = P_.size() * 3; id < id_extra; id++)
    {
        mk_pk += ans(id);
    }

    return sucess;
}


void PolygonsCollisionSolver::collision_resolve(VectorXd &x0) {

    //parameter for trust region algorithm
    const int MAX_ITER_TIMES = 10000;
    const double MAX_TRUST_REGION_SIZE  = 0.002;
    const double MIN_TRUST_REGION_SIZE  = 1e-8;
    const double ACCEPT_RATIO           = 0;
    const double INIT_TRUST_REGION_SIZE = 0.002;

    int         iter_times  = 0;
    double      dx          = INIT_TRUST_REGION_SIZE;   //trust region size
    double      mk_0, mk_pk, f_xk, f_xkpk;              //function and model's improvement

    x0 = VectorXd::Zero(P_.size() * 3);

    //for optimization
    int id_term = 0;
    int id_extra = 3 * P_.size();
    int Ia, Ib;
    double f0;

    std::vector<T> triplist;
    std::vector<bool> in_opt; in_opt.resize(P_.size(), false);
    std::vector<double> Ac;

    VectorXd a, tx0, tx;
    vecPolys p;

    while(iter_times < MAX_ITER_TIMES && dx > MIN_TRUST_REGION_SIZE)
    {
        iter_times++;
        mk_0 =  mk_pk =  f_xk = f_xkpk = 0;
        triplist.clear();
        Ac.clear();
        in_opt.clear();
        in_opt.resize(P_.size(), false);
        id_term = 0;
        id_extra = 3 * P_.size();

        for(int id = 0; id < Conn_.size(); id++) {
            //clear
            a = VectorXd::Zero(7);
            tx0 = VectorXd::Zero(6);
            p.clear();
            Ia = Conn_[id].first;
            Ib = Conn_[id].second;
            f0 = 0;

            //inital
            p.push_back(P_[Ia]);
            p.push_back(P_[Ib]);

            tx0.segment(0, 3) = x0.segment(3 * Ia, 3);
            tx0.segment(3, 3) = x0.segment(3 * Ib, 3);


            if (!get_a_coeff(a, p, tx0, f0))
                continue;

            in_opt[Ia] = true;
            in_opt[Ib] = true;

            for (int jd = 0; jd < 3; jd++) {
                triplist.push_back(T(id_term, 3 * Ia + jd, -a(1 + jd)));
                triplist.push_back(T(id_term, 3 * Ib + jd, -a(4 + jd)));
            }
            triplist.push_back(T(id_term, id_extra++, 1));
            triplist.push_back(T(id_term, id_extra++, -1));

            Ac.push_back(a(0));

            f_xk += f0;
            mk_0 += f0;

            id_term++;
        }

//        in_opt[2] = 0;
//        in_opt[3] = 0;
        //construction of linear programming
        VectorXd x;
        solve_linear(triplist, Ac, in_opt, x0, x, id_term, id_extra, dx, mk_pk);

        for(int id = 0; id < Conn_.size(); id++)
        {
            //clear
            p.clear();
            Ia = Conn_[id].first;
            Ib = Conn_[id].second;

            //inital
            p.push_back(P_[Ia]);
            p.push_back(P_[Ib]);

            tx = VectorXd::Zero(6);
            tx.segment(0, 3) = x.segment(3 * Ia, 3);
            tx.segment(3, 3) = x.segment(3 * Ib, 3);
            f_xkpk += penetration_distance(p, tx);
        }




        //output information
        double pho = (f_xk - f_xkpk) / (mk_0 - mk_pk);
        std::cout << "Iter:\t" << iter_times << std::endl
                  << "dx:\t" << (x - x0).transpose() << std::endl
                  << "x:\t" <<   x0.transpose() << std::endl
                  << "f_xkpk:\t" << f_xkpk << std::endl
                  << "pho:\t" << pho << std::endl
                  << "dx:\t" << dx << std::endl << std::endl;

        //trust region expanding
        if(pho < 0.5)
        {
            dx = dx * 0.25;
        }
        else
        {
            if(pho > 0.75 && (x - x0).norm() >= dx - MIN_TRUST_REGION_SIZE)
            {
                dx = std::min(2 * dx, MAX_TRUST_REGION_SIZE);
            }
        }
        if(pho > ACCEPT_RATIO)
        {
            x0 = x;
        }

        if(f_xkpk < MIN_TRUST_REGION_SIZE) break;
    }
}
