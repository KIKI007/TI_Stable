//
// Created by ziqwang on 14.12.17.
//

#ifndef TI_STABLE_TIMER_EVALUATION_H
#define TI_STABLE_TIMER_EVALUATION_H
#include <ctime>
#include <cstring>
using std::string;
class Timer_evaluation{
public:
    Timer_evaluation()
    {
        tot_ = 0;
    }

public:
    void begin()
    {
        time_ = clock();
    }

    void pause()
    {
        tot_ += clock() - time_;
    }

    string total()
    {
        char str[100];
        sprintf(str, "%fs", tot_ /(CLOCKS_PER_SEC));
        return str;
    }

private:
    clock_t time_;

    double tot_;
};
#endif //TI_STABLE_TIMER_EVALUATION_H
