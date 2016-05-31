#include "impl.h"

#include <limits>

// because floats suck...
#define EPSILON 0.000000001
#define LESS(a,b) ((a)-(b) < -EPSILON)
#define EQ(a,b) (((a)-(b))*((a)-(b)) < EPSILON)

bool verbose = false;


Result PerformPivot(Matrix& t)
{
    // TODO check soundness?

    // choose first j with reduced cost < 0
    unsigned j = 0;
    for (unsigned y = 1; y < t.N; ++y) {
        if (LESS(t.get(0, y), 0)) {
            j = y;
            break;
        }
    }

    if (j == 0) {
        // no such j exists => optimal solution found
        if (verbose)
            std::cerr << " > optimal" << std::endl;
        return OPTIMAL;
    }

    if (verbose) {
        std::cerr << " > Choose j = " << j << std::endl;
    }

    // choose l that minimizes x_B(l) < / u_l with u_l > 0
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
        // if no such l exists => problem is unbounded
        if (verbose)
            std::cerr << " > unbounded" << std::endl;
        return UNBOUNDED;
    }

    if (verbose) {
        std::cerr << " > Choose l = " << l << std::endl;
        std::cerr << " ~~> (" << l << ") = (" << l << ") * " << 1/t.get(l, j) << std::endl;
    }

    // update the mapping from columns to variables
    t.setMapping(l, j);

    // perform elementary row operations
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

bool Phase1(Matrix& t)
{
    // read cost vector
    std::vector<double> c(t.M-1);
    for (unsigned y = 1; y < t.M; ++y) {
        c.at(y-1) = t.get(0, y);
    }

    // create tableau for artificial problem
    Matrix a(t.M, t.N + t.M-1);

    // cT - cBT AB^-1 A = [0...01...1] - [1...1] I A
    // ...

    for (unsigned x = 1; x < t.M; ++x) {
        // factor for making every entry of b >= 0
        double sign_factor = (LESS(t.get(x, 0), 0.0)) ? -1.0 : 1.0;

        // set first N entrys of the artificial tableau
        for (unsigned y = 0; y < t.N; ++y) {
            a.set(x, y, sign_factor * t.get(x, y));
            a.set(0, y, a.get(0, y) - sign_factor * t.get(x, y));
        }

        // set entries corresponding to the artificial variables
        for (unsigned y = t.N; y < a.N; ++y) {
            if (y - t.N == x - 1) {
                a.set(x, y, 1.0);
                a.setMapping(x, y);
                break;
            }
        }
    }

    if (verbose) {
        std::cerr << "Solve artificial LP: {{{" << std::endl;
        std::cerr << a << std::endl;
    }
    double res = Phase2(a);

    if (verbose) {
        std::cerr << "Solution of artificial LP:" << std::endl;
        std::cerr << a << std::endl;
        a.printMapping(std::cerr);
        std::cerr << "}}}" << std::endl;
    }

    // TODO choose lin. independent set of columns containing the basis from sol
    // --> calc determinant
    // TODO set up t
    // --> invert matrix

    return EQ(res, 0);
}

