#include "impl.h"

#include <limits>

// because floats suck...
#define EPSILON 0.000000001
#define LESS(a,b) ((a)-(b) < -EPSILON)


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

    if (j == 0)
        return OPTIMAL;

#ifdef DBG
    std::cerr << "Choose j = " << j << std::endl;
#endif

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

    if (l == 0)
        return UNBOUNDED;
#ifdef DBG
    std::cerr << "Choose l = " << l << std::endl;

    std::cerr << "=====" << std::endl;
    std::cerr << "-> (" << l << ") = (" << l << ") * " << 1/t.get(l, j) << std::endl;
#endif
    t.multiplyRowBy(l, 1/t.get(l, j));
#ifdef DBG
    std::cerr << t;
#endif

    for (unsigned x = 0; x < t.M; ++x) {
        if (x == l)
            continue;
#ifdef DBG
        std::cerr << "=====" << std::endl;
        std::cerr << "-> (" << x << ") = (" << x << ") + (" << l << ") * " << -t.get(x, j) << std::endl;
#endif
        t.addDTimesRowBToRowA(x, l, -t.get(x, j));
#ifdef DBG
        std::cerr << t;
#endif
    }

    return NONOPTIMAL;
}


