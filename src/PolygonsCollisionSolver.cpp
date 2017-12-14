//
// Created by ziqwang on 08.12.17.
//

#include "PolygonsCollisionSolver.h"

PolygonsCollisionSolver::PolygonsCollisionSolver()
{
    gap_ = std::pair<int, int>(-1, -1);
}

void PolygonsCollisionSolver::setPolygons(vecPolys &P)
{
    P_ = P;
}

void PolygonsCollisionSolver::setConnection(int a, int b) {
    Conn_.push_back(std::pair<int, int>(a, b));
}

void PolygonsCollisionSolver::setFixed(int a)
{
    fixed_.push_back(a);
}

void PolygonsCollisionSolver::setGap(int a, int b)
{
    gap_ = std::pair<int, int>(a, b);
}

bool PolygonsCollisionSolver::get_a_coeff(vecVectorXd &a, vector<PolygonPoints> &P_ORI, VectorXd &x, double &f_xk)
//a is the coefficient of penetration distance function
    //for 2D case when two faces are parallel, d(P, Q) = min{ d(p, q, n), d(p, r, n)}.
    //a can be a matrix of size 2 * 7
//P_ORI is two collision polygons in its origin position
//x is present rotation and translation
    //x(0 + 3 * i) is theta (rotation angle) in Pi
    //x(1 + 3 * i) is x (translation in x axis) in Pi
    //x(2 + 3 * i) is y (translation in y axis) in Pi
//f_xk is the penetration distance（with sign)
{
    Vector3d x0[2];
    x0[0] = x.segment(0, 3);
    x0[1] = x.segment(3, 3);

    PolygonPoints P[2];
    P[0] = P_ORI[0];
    P[0].do_transformation(x0[0]);
    P[1] = P_ORI[1];
    P[1].do_transformation(x0[1]);

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

double PolygonsCollisionSolver::signed_distance(vecPolys p, VectorXd &x)
//p has two polygons in its original position and orientation
//x is a vector of 6 components
    //x(0~3) for p[0]'s orientation and position
    //x(3~6) for p[1]'s orientation and position
{
    Vector3d x0 = x.segment(0, 3);
    Vector3d x1 = x.segment(3, 3);
    p[0].do_transformation(x0);
    p[1].do_transformation(x1);

    Vector3d n;
    int Ia, Ib;
    bool na;
    double mu;
    p[0].collision(p[1], n, na, Ia, Ib);
    return n.dot(p[0].point(Ia) - p[1].point(Ib));
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
//solve linear programming
    // min c^Tx
    // lc <= Ax <= uc
    // lx <= x <= ux

// triplist:     is sparse represetation of A
// vec_c:        is c but only for auxiliary variable (the variable theta, dx, dy term are all 0)
// Ac is:        the coefficient of lc = uc = Ac
// in_opt:       is whether one component of x is changable. (some polygons are fixed)
// x0:           input orginal position and orientation
// x:            output optimal result
// id_term:      rows of lc <= Ax <= uc
// id_extra:     number of auxiliary variable
// dx:           region size in trust region method
// mk_pk:        result of c^Tx
{

    /*               create c^Tx                  */
    Eigen::VectorXd c = VectorXd::Zero(id_extra);
    for(int id = P_.size() * 3; id < id_extra; id++)
    {
        c(id) = vec_c[id - P_.size() * 3];
    }

    /*            create lc <= Ax <= uc            */
    Eigen::SparseMatrix<double> A(id_term, id_extra);
    A.setFromTriplets(triplist.begin(), triplist.end());

    Eigen::VectorXd lc(id_term), uc(id_term);
    for(int id = 0; id < Ac.size(); id++) lc(id) = uc(id) = Ac[id];

    /*            create lx <= x <= ux             */
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

    /*            solve linear programming             */
    Eigen::VectorXd ans(id_extra);
    x = VectorXd(P_.size() * 3);
    bool sucess = igl::mosek::mosek_linprog(c, A, lc, uc, lx, ux, ans);

    /*              output result                      */
    for(int id = 0; id < P_.size() * 3; id++) x(id) = ans(id);

    mk_pk = c.dot(ans);
    return sucess;
}


void PolygonsCollisionSolver::collision_resolve(VectorXd &x0, double &dx)
// x0:           input orginal position and orientation
// dx:           region size in trust region method
{
    //const parameter for trust region algorithm
    const int MAX_ITER_TIMES = 1000;
    const double MAX_TRUST_REGION_SIZE  = 0.01;
    const double MIN_TRUST_REGION_SIZE  = 1e-6;
    const double ACCEPT_RATIO           = 0.5;
    //const double ACCEPT_F_XK            = 1e-6;
    const double INIT_TRUST_REGION_SIZE = 0.001;

    //variable definition for trust region algorithm
    int         iter_times, id_term, id_extra;
    int         Ia, Ib;
    double      mk_0, mk_pk, f_xk, f_xkpk, f0;              //function and model's improvement
    double mu;
    bool gap_pair;
    VectorXd tx0, tx;
    std::vector<T> triplist;
    std::vector<double> Ac, c;
    std::vector<bool> in_opt;                           // wethea a polygon is
    vecPolys p;                                         // p[0], p[1] are two polygons which has collision constrain
    vecVectorXd a;                                      // for constructing linear programming

    //initializing
    mu = 1;
    if(x0.isZero())
    {
        x0 = VectorXd::Zero(P_.size() * 3);
        dx = INIT_TRUST_REGION_SIZE;
    }
    in_opt.resize(P_.size(), true);
    for(int id = 0; id < fixed_.size(); id++)
    {
        if(fixed_[id] >= 0 && fixed_[id] < P_.size())
            in_opt[fixed_[id]] = false;
    }

    for(mu = 1; mu >= 1e-5; mu/= 10.0)
    {
        iter_times = 0;
        dx = INIT_TRUST_REGION_SIZE;


        while(iter_times < MAX_ITER_TIMES && dx > MIN_TRUST_REGION_SIZE)
        {
            iter_times++;
            std::cout << "Iter:\t" << iter_times << std::endl;

            Timer_evaluation timer_get_a;
            Timer_evaluation timer_solve_linear;
            Timer_evaluation timer_tot;
            timer_tot.begin();

            //initialized
            {
                mk_0 =  mk_pk =  f_xk = f_xkpk = 0;

                triplist.clear();
                Ac.clear();
                c.clear();

                id_term = 0;
                id_extra = 3 * P_.size();
            }

            for(int id = 0; id < Conn_.size(); id++)
            {
                //clear
                a.clear();
                p.clear();
                tx0 = VectorXd::Zero(6);
                Ia = Conn_[id].first;
                Ib = Conn_[id].second;
                f0 = 0;

                //inital
                p.push_back(P_[Ia]);
                p.push_back(P_[Ib]);

                tx0.segment(0, 3) = x0.segment(3 * Ia, 3);
                tx0.segment(3, 3) = x0.segment(3 * Ib, 3);

                gap_pair = false;
                if(Ia == gap_.first && Ib == gap_.second) gap_pair = true;
                if(Ia == gap_.second && Ib == gap_.first) gap_pair = true;

                //get a
                timer_get_a.begin();
                if(!get_a_coeff(a, p, tx0, f0))
                    continue;
                timer_get_a.pause();

                //build linear programming
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
                    if(gap_pair)
                    {
                        c.push_back(-mu);
                        c.push_back(1 + mu);
                    }
                    else
                    {
                        c.push_back(0);
                        c.push_back(1);
                    }
                }
                else
                {
                    if(gap_pair)
                    {
                        int s[8];
                        for(int jd = 7; jd >= 0; jd--)
                        {
                            s[jd] = id_extra + (jd - 4);
                        }

                        VectorXd coef_0(8); coef_0 << 0, 1, 0, -1, -1, 1, 0, 0;
                        VectorXd coef_1(8); coef_1 << -1, 1, 1, -1, 0, 0, -1, 1;
                        for(int jd = 0; jd < 8; jd++)
                        {
                            triplist.push_back(T(id_term, s[jd], coef_0[jd]));
                            triplist.push_back(T(id_term + 1, s[jd], coef_1[jd]));
                        }
                        Ac.push_back(0);Ac.push_back(0);
                        id_term+= 2;

                        coef_0 <<  0, 0.5, 0, 0.5, 0.5, 0.5, 0, 0;
                        coef_1 << -mu/2, mu/2, -mu/2, mu/2, 0, 0, mu/2, mu/2;
                        for(int jd = 0; jd < 8; jd++)
                        {
                            c.push_back(coef_0[jd] + coef_1[jd]);
                        }
                        id_extra = s[7] + 1;
                    }
                    else
                    {
                        int s[6];
                        for(int jd = 5; jd >= 0; jd--)
                        {
                            s[jd] = id_extra + (jd - 4);
                        }

                        VectorXd coef(6); coef << 0, 1, 0, -1, -1, 1;
                        VectorXd coef_c(6); coef_c << 0, 0.5, 0, 0.5, 0.5, 0.5;
                        for(int jd = 0; jd < 6; jd++)
                        {
                            triplist.push_back(T(id_term, s[jd], coef[jd]));
                            c.push_back(coef_c[jd]);
                        }
                        Ac.push_back(0);
                        id_term++;
                        id_extra = s[5] + 1;
                    }
                }

                if(gap_pair)
                {
                    f_xk += (-mu) * f0;
                    mk_0 += (-mu) * f0;
                }
                f_xk += f0 > 0 ? 0 : -f0;
                mk_0 += f0 > 0 ? 0 : -f0;
            }

            //construction of linear programming
            VectorXd x;
            timer_solve_linear.begin();
            solve_linear(triplist, c, Ac, in_opt, x0, x, id_term, id_extra, dx, mk_pk);
            timer_solve_linear.pause();

            for(int id = 0; id < Conn_.size(); id++)
            {
                //clear
                p.clear();
                Ia = Conn_[id].first;
                Ib = Conn_[id].second;

                gap_pair = false;
                if(Ia == gap_.first && Ib == gap_.second) gap_pair = true;
                if(Ia == gap_.second && Ib == gap_.first) gap_pair = true;

                //inital
                p.push_back(P_[Ia]);
                p.push_back(P_[Ib]);

                tx = VectorXd::Zero(6);
                tx.segment(0, 3) = x.segment(3 * Ia, 3);
                tx.segment(3, 3) = x.segment(3 * Ib, 3);

                double f0 = signed_distance(p, tx);
                if(gap_pair) f_xkpk += (-mu) * f0;
                f_xkpk += f0 > 0? 0 : -f0;
            }

            timer_tot.pause();
            //output information
            double pho = f_xk - f_xkpk == 0 ? 0 : (f_xk - f_xkpk) / (mk_0 - mk_pk);

            std::cout //<< "dx:\t" << (x - x0).transpose() << std::endl
                    //<< "x:\t" <<   x0.transpose() << std::endl
                    << "time_tot\t" << timer_tot.total() << std::endl
                    << "time_get_a\t" << timer_get_a.total() << std::endl
                    << "time_solve\t" << timer_solve_linear.total() << std::endl
                    << "mu:\t" << mu <<std::endl
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
        }
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
    P_ORI[0].get_center(ct_ori[0]);
    P_ORI[1].get_center(ct_ori[1]);

    p[0]        = P[0].point(Ia);
    p[1]        = P[1].point(Ib);
    p_ori[0]    = P_ORI[0].point(Ia);
    p_ori[1]    = P_ORI[1].point(Ib);

    f_xk = n.dot(p[0] - p[1]);
    //f_xk = f_xk > 0 ? 0 : -f_xk;

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
