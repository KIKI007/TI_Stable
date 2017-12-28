//
// Created by ziqwang on 28.12.17.
//

#include "PolyhedronsCollisionSolver.h"

PolyhedronsCollisionSolver::PolyhedronsCollisionSolver() {
    gap_ = std::pair<int, int>(-1, -1);
}

void PolyhedronsCollisionSolver::setPolyhedrons(vecPlyhdrons &P) {
    P_ = P;
}

void PolyhedronsCollisionSolver::setConnection(int a, int b) {
    Conn_.push_back(std::pair<int, int>(a, b));
}

void PolyhedronsCollisionSolver::setFixed(int a) {
    fixed_.push_back(a);
}

void PolyhedronsCollisionSolver::setGap(int a, int b) {
    gap_ = std::pair<int, int>(a, b);
}

void PolyhedronsCollisionSolver::collision_resolve(VectorXd &x0, double &dx) {
    const int MAX_ITER_TIMES = 1000;
    const double MAX_TRUST_REGION_SIZE  = 0.01;
    const double MIN_TRUST_REGION_SIZE  = 1e-6;
    const double ACCEPT_RATIO           = 0.5;
    //const double ACCEPT_F_XK            = 1e-6;
    const double INIT_TRUST_REGION_SIZE = 0.001;

    //variable definition for trust region algorithm
    int         iter_times, size_A, size_x;
    int         Ia, Ib;
    double      mk_0, mk_pk, f_xk, f_xkpk, f0;              //function and model's improvement
    double mu, gap_size;
    bool gap_pair;
    VectorXd tx0, tx;
    std::vector<T> triplist;
    std::vector<double> lc, c;
    std::vector<double> lx, ux;
    std::vector<bool> in_opt;                           // wethea a polygon is
    vecPlyhdrons p;                                     // p[0], p[1] are two polygons which has collision constrain
    vecVectorXd coeff_gs;                               // for constructing linear programming

    //initializing
    mu = 0.001;
    if(x0.isZero())
    {
        x0 = VectorXd::Zero(P_.size() * 7);
        for(int id = 0; id < P_.size(); id++) x0[id * 7] = 1;
        dx = INIT_TRUST_REGION_SIZE;
    }
    in_opt.resize(P_.size(), true);
    for(int id = 0; id < fixed_.size(); id++)
    {
        if(fixed_[id] >= 0 && fixed_[id] < P_.size())
            in_opt[fixed_[id]] = false;
    }


    for(; mu >= MIN_TRUST_REGION_SIZE; mu =  mu / 10.0)
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
                lc.clear();c.clear();
                ux.clear();lx.clear();

                size_A = 0;
                size_x = 6 * P_.size();
                c.resize(size_x);
            }

            for(int id = 0; id < Conn_.size(); id++)
            {
                //clear
                coeff_gs.clear();
                p.clear();
                Ia = Conn_[id].first;
                Ib = Conn_[id].second;
                f0 = 0;
                //inital & transformation
                p.push_back(P_[Ia]);
                p.push_back(P_[Ib]);
                p[0].transformation(vec4quat(x0.segment(7 * Ia, 4)), x0.segment(7 * Ia + 4, 3));
                p[1].transformation(vec4quat(x0.segment(7 * Ib, 4)), x0.segment(7 * Ib + 4, 3));

                //get
                timer_get_a.begin();
                get_g_coeff(coeff_gs, p, f0);
                timer_get_a.pause();

                //build Ax >= lc & c^Tx
                for(int kd = 0; kd < coeff_gs.size(); kd++)
                {
                    for(int jd = 0; jd < 6; jd++)
                    {
                        triplist.push_back(T(size_A, 6 * Ia + jd, coeff_gs[kd](1 + jd)));
                        triplist.push_back(T(size_A, 6 * Ib + jd, coeff_gs[kd](7 + jd)));
                    }
                    triplist.push_back(T(size_A, size_x, coeff_gs[kd][13]));
                    lc.push_back(-coeff_gs[kd](0));
                    size_A++;
                }
                c.push_back(1);
                size_x++;
                f_xk += f0;
                mk_0 += f0;
            }

            for(int id = 0; id < P_.size();id++)
            {
                c[id * 6 + 5] = mu;
                f_xk += mu * x0(7 * id + 6);
                mk_0 += mu * x0(7 * id + 6);
            }

            //build build lx, ux
            for(int id = 0; id < size_x; id++)
            {
                if(id < P_.size() * 6)
                {
                    if(in_opt[id / 6])
                    {
                        lx.push_back(-dx);
                        ux.push_back(dx);
                    }
                    else
                    {
                        lx.push_back(0);
                        ux.push_back(0);
                    }
                }
                else
                {
                    lx.push_back(0);
                    ux.push_back(std::numeric_limits<double>::infinity());
                }
            }

            //construction of linear programming
            VectorXd tx, x;
            timer_solve_linear.begin();
            solve_linear(size_x, size_A, triplist, c, lc, lx, ux, tx, mk_pk);
            timer_solve_linear.pause();

            //compose transformation
            x = VectorXd::Zero(7 * P_.size());
            for(int id = 0; id < P_.size(); id++)
            {
                Quaterniond q, q0, q1;
                q0 = vec4quat(x0.segment(id * 7, 4));
                q1 = vec3quat(tx.segment(id * 6, 3));
                q = q1 * q0;
                x.segment(id * 7, 4) = Vector4d(q.w(), q.x(), q.y(), q.z());
                x.segment(id * 7 + 4 , 3) = x0.segment(id * 7 + 4, 3) + tx.segment(id * 6 + 3, 3);
                f_xkpk += mu * x(id * 7 + 6);
                mk_pk += mu * x0(7 * id + 6);
            }

            //evaluate result
            for(int id = 0; id < Conn_.size(); id++)
            {
                //clear
                p.clear();
                Ia = Conn_[id].first;
                Ib = Conn_[id].second;

                //inital
                p.push_back(P_[Ia]); p.push_back(P_[Ib]);
                p[0].transformation(vec4quat(x.segment(7 * Ia, 4)), x.segment(7 * Ia + 4, 3));
                p[1].transformation(vec4quat(x.segment(7 * Ib, 4)), x.segment(7 * Ib + 4, 3));
                double f0; p[0].collision(p[1], f0);
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
                    << "dx:\t" << dx << std::endl
                    << "gap_size:\t" << gap_size << std::endl << std::endl;

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

void PolyhedronsCollisionSolver::get_g_coeff(vecVectorXd &coeff_g, vecPlyhdrons &P, double &f_xk) {
    assert(P.size() == 2);
    PolyhedraPoints &A = P[0], &B = P[1];

    Vector3d nA, nB, n;
    Vector3d cA, cB;
    Vector3d pA, pB;
    Vector3d pcA, pcB;
    vecVector3d pAs, pBs;
    VectorXd a(13);
    A.collision(B, nA, nB, pAs, pBs, f_xk); f_xk = f_xk > 0 ? 0 : - f_xk;
    A.get_center(cA);B.get_center(cB);
    n = A.norm_cross_from_polyhedrons(nA, nB);

    for(int id = 0; id < pAs.size(); id++)
    {
        for(int jd = 0; jd < pBs.size(); jd++)
        {
            a = VectorXd::Zero(14);
            pA = pAs[id]; pB = pBs[jd];
            pcA = pA - cA; pcB = pB - cB;
            a(0) += n.dot(pA - pB);
            a.segment(1, 3) += pcA.cross(n);
            a.segment(4, 3) += n;
            a.segment(7, 3) += -pcB.cross(n);
            a.segment(10, 3) += -n;
            a(13) = 1;

            if(nB.isZero())
            {
                a.segment(1, 3) += n.cross(pA - pB);
            }
            else if(nA.isZero())
            {
                a.segment(7, 3) += n.cross(pA - pB);
            }
            else
            {
                a.segment(1, 3) += nA.cross(pA - pB).cross(nB);
                a.segment(7, 3) += (pA - pB).cross(nB).cross(nA);
            }
            coeff_g.push_back(a);
        }
    }

    return;
}

bool PolyhedronsCollisionSolver::solve_linear(int n,
                                              int m,
                                              vector<T> &triplist,
                                              vector<double> &vec_c,
                                              vector<double> &vec_lc,
                                              vector<double> &vec_lx,
                                              vector<double> &vec_ux,
                                              VectorXd &x,
                                              double &mk_pk)
//solve linear programming
// min c^Tx
// lc <= Ax
// lx <= x <= ux

// n             number of variables
// m             number of rows of A
// triplist:     is sparse represetation of A
// vec_c:        is c but only for auxiliary variable (the variable theta, dx, dy term are all 0)
// vec_lc is:    the coefficient of lc <= Ax
// vec_lx is:    the coefficient of lx <= x
// vec_ux is:    the coefficient of x <= ux
// x0:           input orginal position and orientation
// x:            output optimal result
// mk_pk:        result of c^Tx
{
    /*               create c^Tx                  */
    Eigen::VectorXd c = VectorXd::Zero(n);
    for (int id = 0; id < n; id++) c(id) = vec_c[id];

    /*            create lc <= Ax            */
    Eigen::SparseMatrix<double> A(m, n);
    A.setFromTriplets(triplist.begin(), triplist.end());

    Eigen::VectorXd lc(m), uc(m);
    for (int id = 0; id < m; id++) {
        lc(id) = vec_lc[id];
        uc(id) = std::numeric_limits<double>::infinity();
    }

    /*            create lx <= x <= ux             */
    Eigen::VectorXd lx(n), ux(n);
    for (int id = 0; id < n; id++) {
        lx[id] = vec_lx[id];
        ux[id] = vec_ux[id];
    }

    /*            solve linear programming             */
    Eigen::VectorXd ans(n);
    x = VectorXd(P_.size() * 6);
    bool sucess = igl::mosek::mosek_linprog(c, A, lc, uc, lx, ux, ans);
    std::cout << sucess << std::endl;
    /*              output result                      */
    for (int id = 0; id < P_.size() * 6; id++) x(id) = ans(id);
    mk_pk = c.dot(ans);
    return sucess;
}

Quaterniond PolyhedronsCollisionSolver::vec4quat(Vector4d u) {
    return Quaterniond(u[0], u[1], u[2], u[3]);
}

Quaterniond PolyhedronsCollisionSolver::vec3quat(Vector3d u)
{
    double theta = u.norm();
    if(theta < 1e-8) return Quaterniond(1, 0, 0, 0);
    Vector3d n = u / theta;
    return Quaterniond(std::cos(theta/2),
                       std::sin(theta/2) * n[0],
                       std::sin(theta/2) * n[1],
                       std::sin(theta/2) * n[2]);
}