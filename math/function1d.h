#ifndef FUNCTION1D_H
#define FUNCTION1D_H

#include <cmath>
#include <vector>

class Function1D
{
    public:
        Function1D() {}

        virtual ~Function1D() {}

    public:
        virtual double operator()(double x) = 0;

        virtual double gradient(double x) = 0;
};

#endif // FUNCTION1D_H
