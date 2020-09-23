#include "sigmoidfunction.h"

double SigmoidFunction::operator()(double x) {
    return 1 / (1 + std::exp(-1 * x));
}

double SigmoidFunction::gradient(double x) {
    return operator()(x) * (1 - operator()(x));
}
