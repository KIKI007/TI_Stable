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

bool PolygonsCollisionSolver::get_a_coeff(vecVectorXd &a, vector<PolygonPoints> &P_ORI, VectorXd &x, double &f_xk)
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

    PolygonPoints P[2];
    P[0] = P_ORI[0];P[0].Rotate_translate(x0[0]);
    P[1] = P_ORI[1];P[1].Rotate_translate(x0[1]);

    /************ collision info **************/
    int Ia, Ib, Ic;
    bool na;
    Vector3d n;
    P[0].collision(P[1], n, na, Ia, Ib, Ic);

    VectorXd ta;
    get_a_coeff(ta, P_ORI, P, n, na, Ia, Ib, x, f_xk);
    a.push_back(ta);

//    std::cout << "n:\t" << n.transpose() << std::endl
//              << "na:\t" << na << std::endl
//              << "Ia:\t" <<   Ia << std::endl
//              << "Ib:\t" << Ib << std::endl
//              << "Ic:\t" << Ic << std::endl
//              << "f_xk:\t" << f_xk << std::endl;

    if(Ic != -1)
    {
        double f;
        if(na) get_a_coeff(ta, P_ORI, P, n, na, Ia, Ic, x, f);
        else get_a_coeff(ta, P_ORI, P, n, na, Ic, Ib, x, f);
        a.push_back(ta);
    }
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
    double mu;
    if(p[0].collision(p[1], n, na, Ia, Ib)) {
        return std::abs(n.dot(p[0].point(Ia) - p[1].point(Ib)));
    }
    else
        return 0;
}

bool PolygonsCollisionSolver::solve_linear(std::vector<T> &triplist,
                                           std::vector<double> &vec_c,
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
        c(id) = vec_c[id - P_.size() * 3];
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
    for(int id = 0; id < id_extra; id++)
    {
        mk_pk += c(id) * ans(id);
    }
    std::cout << "mk_pk:\t " << mk_pk << std::endl;

    return sucess;
}


void PolygonsCollisionSolver::collision_resolve(VectorXd &x0, double &dx) {

    //parameter for trust region algorithm
    const int MAX_ITER_TIMES = 10000;
    const double MAX_TRUST_REGION_SIZE  = 0.01;
    const double MIN_TRUST_REGION_SIZE  = 1e-8;
    const double ACCEPT_RATIO           = 0.5;
    const double ACCEPT_F_XK            = 1e-4;
    const double INIT_TRUST_REGION_SIZE = 0.01;

    int         iter_times  = 0;
    //double      dx          = INIT_TRUST_REGION_SIZE;   //trust region size
    double      mk_0, mk_pk, f_xk, f_xkpk;              //function and model's improvement

    if(x0.size() == 0)
    {
        x0 = VectorXd::Zero(P_.size() * 3);
        dx = INIT_TRUST_REGION_SIZE;
    }
    //for optimization
    int id_term = 0;
    int id_extra = 3 * P_.size();
    int Ia, Ib;
    double f0;

    std::vector<T> triplist;
    std::vector<bool> in_opt; in_opt.resize(P_.size(), true);
    std::vector<double> Ac;
    std::vector<double> c;

    VectorXd tx0, tx;
    vecPolys p;
    vecVectorXd a;

    for(int id = 0; id < in_opt.size(); id++) in_opt[id] = true;

    in_opt[0] = 0;
    in_opt[1] = 0;
    in_opt[2] = 0;
    in_opt[3] = 0;

    while(iter_times < MAX_ITER_TIMES && dx > MIN_TRUST_REGION_SIZE)
    {
        iter_times++;
        std::cout << "Iter:\t" << iter_times << std::endl;

        mk_0 =  mk_pk =  f_xk = f_xkpk = 0;
        triplist.clear();
        Ac.clear();
        id_term = 0;
        id_extra = 3 * P_.size();
        c.clear();


        for(int id = 0; id < Conn_.size(); id++)
        {
            //clear
            a.clear();
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


            if(!get_a_coeff(a, p, tx0, f0))
                continue;


            for(int kd = 0; kd < a.size(); kd++)
            {
                for(int jd = 0; jd < 3; jd++)
                {
                    triplist.push_back(T(id_term, 3 * Ia + jd, -a[kd](1 + jd)));
                    triplist.push_back(T(id_term, 3 * Ib + jd, -a[kd](4 + jd)));
                }
                triplist.push_back(T(id_term, id_extra++,  1));
                triplist.push_back(T(id_term, id_extra++, -1));
                Ac.push_back(a[kd](0));
                id_term++;
            }

            if(a.size() == 1)
            {
                c.push_back(0);
                c.push_back(1);
            }
            else
            {
                int s[6];
                for(int jd = 5; jd >= 0; jd--)
                {
                    s[jd] = id_extra + (jd - 4);
                }

                int coef[6] = {0, 1, 0, -1, -1, 1};
                double coef_c[6] = {0, 0.5, 0, 0.5, 0.5, 0.5};
                for(int jd = 0; jd < 6; jd++)
                {
                    triplist.push_back(T(id_term, s[jd], coef[jd]));
                    c.push_back(coef_c[jd]);
                }

                Ac.push_back(0);
                id_term++;
                id_extra = s[5] + 1;
            }
            f_xk += f0;
            mk_0 += f0;
        }
        //construction of linear programming
        VectorXd x;
        solve_linear(triplist, c, Ac, in_opt, x0, x, id_term, id_extra, dx, mk_pk);

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

        std::cout << "dx:\t" << (x - x0).transpose() << std::endl
                  << "x:\t" <<   x0.transpose() << std::endl
                  << "f_xk:\t" << f_xk << std::endl
                  << "f_xkpk:\t" << f_xkpk << std::endl
                  << "mk_pk:\t" << mk_pk << std::endl
                  << "pho:\t" << pho << std::endl
                  << "dx:\t" << dx << std::endl << std::endl;

        //trust region expanding
        if(pho < 0.5 || f_xk < f_xkpk)
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
        if(pho > ACCEPT_RATIO && f_xk > f_xkpk)
        {
            x0 = x;
        }

        if(f_xkpk < ACCEPT_F_XK) break;
    }
}

void PolygonsCollisionSolver::get_a_coeff(VectorXd &a,
                                          vecPolys &P_ORI,
                                          PolygonPoints *P,
                                          Vector3d n,
                                          bool na,
                                          int Ia,
                                          int Ib,
                                          VectorXd &x,
                                          double &f_xk) {

    Vector3d ct_ori[2], p[2], p_ori[2];
    P_ORI[0].center_points(ct_ori[0]);
    P_ORI[1].center_points(ct_ori[1]);

    p[0]        = P[0].point(Ia);
    p[1]        = P[1].point(Ib);
    p_ori[0]    = P_ORI[0].point(Ia);
    p_ori[1]    = P_ORI[1].point(Ib);

    f_xk = n.dot(p[0] - p[1]);
    f_xk = f_xk > 0 ? 0 : -f_xk;

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
}
