#ifndef SIGMOIDFUNCTION_H
#define SIGMOIDFUNCTION_H

#include "function1d.h"

class SigmoidFunction : public Function1D
{
    public:
        SigmoidFunction() : Function1D() {}

        virtual ~SigmoidFunction() {}

        virtual double operator()(double x);

        virtual double gradient(double x);
};

#endif // SIGMOIDFUNCTION_1D_H
