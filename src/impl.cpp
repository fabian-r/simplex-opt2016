#include "impl.h"

#include <limits>

// because floats suck...
#define EPSILON 0.000000001
#define LESS(a,b) ((a)-(b) < -EPSILON)

bool verbose = false;


Result PerformPivot(Matrix& t)
{
    // TODO check soundness?
    unsigned j = 0;
    for (unsigned y = 1; y < t.N; ++y) {
        if (LESS(t.get(0, y), 0)) {
            j = y;
            break;
        }
    }

    if (j == 0) {
        if (verbose)
            std::cerr << " > optimal" << std::endl;
        return OPTIMAL;
    }

    if (verbose) {
        std::cerr << " > Choose j = " << j << std::endl;
    }

    unsigned l = 0;
    double min = std::numeric_limits<double>::infinity();

    for (unsigned x = 1; x < t.M; ++x) {
        double ui = t.get(x, j);
        if (LESS(0, ui)) {
            double val = t.get(x, 0) / ui;
            if (val < min) { // intentionally not LESS(val, min)
                min = val;
                l = x;
            }
        }
    }

    if (l == 0) {
        if (verbose)
            std::cerr << " > unbounded" << std::endl;
        return UNBOUNDED;
    }

    if (verbose) {
        std::cerr << " > Choose l = " << l << std::endl;
        std::cerr << " ~~> (" << l << ") = (" << l << ") * " << 1/t.get(l, j) << std::endl;
    }

    t.multiplyRowBy(l, 1/t.get(l, j));

    for (unsigned x = 0; x < t.M; ++x) {
        if (x == l)
            continue;
    if (verbose) {
        std::cerr << " ~~> (" << x << ") = (" << x << ") + (" << l << ") * "
                  << -t.get(x, j) << std::endl;
    }
        t.addDTimesRowBToRowA(x, l, -t.get(x, j));
    }

    if (verbose)
        std::cerr << " > non-optimal" << std::endl;
    return NONOPTIMAL;
}

double Phase2(Matrix& t)
{
    Result res;
    unsigned num = 0;
    do {
        if (verbose) {
            std::cerr << "Iteration " << ++num << ": {{{" << std::endl;
        }
        res = PerformPivot(t);
        if (verbose) {
            std::cerr << t;
            std::cerr << "}}}" << std::endl;
            std::cerr << std::endl;
        }
    } while (res == NONOPTIMAL);

    if (res == UNBOUNDED) {
        return - std::numeric_limits<double>::infinity();
    }
    return - t.get(0, 0);
}

