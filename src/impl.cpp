#include "impl.h"

#include <limits>
#include <cassert>

// because floats suck...
#define EPSILON 0.000000001
#define LESS(a,b) ((a)-(b) < -EPSILON)
#define EQ(a,b) (((a)-(b))*((a)-(b)) < EPSILON*EPSILON)

bool verbose = false;


/**
 * Perform the necessary operations such that B(l) leaves the basis and j enters
 * it.
 */
void Eliminate(Matrix& t, unsigned l, unsigned j)
{
    // update the mapping from columns to variables
    t.setMapping(l, j);

    if (verbose) {
        std::cerr << " ~~> (" << l << ") = (" << l << ") * " << 1/t.get(l, j) << std::endl;
    }

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
}

void ReducedRowEchelon(Matrix& m)
{
    for (unsigned x = 0; x < m.M; ++x) {
        if (EQ(m.get(x, x), 0)) {
            bool success = false;
            for (unsigned y = x; y < m.M; ++y) {
                if (EQ(m.get(y, x), 0))
                    continue;
                m.addDTimesRowBToRowA(x, y, 1.0);
                success = true;
            }
            assert(success && "invalid input for reduced row echelon algorithm!");
        }
        Eliminate(m, x, x);
    }

}

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
    }

    Eliminate(t, l, j);

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

    // solve artificial LP
    double res = Phase2(a);
    if (! EQ(res, 0))
        return false;

    for (unsigned x = 1; x < a.M; ++x) {
        if (a.getMapping(x) < t.N)
            continue;
        // artificial basic variable, has to be eliminated
        if (verbose) {
            std::cerr << " Eliminate artificial variable x"
                      << a.getMapping(x) << "...";
        }

        bool success = false;

        // search for non-artificial variable to enter basis instead
        for (unsigned y = 1; y < t.N; ++y) {
            if (EQ(a.get(x, y), 0))
                continue;

            if (verbose) {
                std::cerr << " and insert x" << y << " instead" << std::endl;
            }
            Eliminate(a, x, y);
            success = true;
            break;
        }

        if (! success) {
            assert(false && "failed to eliminate!");
            // we cannot eliminate the variable from the basis
            // => the constraint is redundant
            std::cerr << " failed to eliminate!" << std::endl;
            a.setMapping(x, 0); // TODO remove row
        }
    }

    if (verbose) {
        std::cerr << std::endl << "Solution of artificial LP:" << std::endl;
        std::cerr << a << std::endl;
        a.printMapping(std::cerr);
        std::cerr << "}}}" << std::endl;
    }

    // calculate inverted basic matrix AB^-1
    unsigned rows = a.M-1;
    Matrix inv(rows, 2*rows);
    for (unsigned x = 0; x < rows; ++x) {
        unsigned var = a.getMapping(x+1);
        for (unsigned y = 0; y < rows; ++y) {
            inv.set(y, x, t.get(y+1, var));
            if (y == x)
                inv.set(y, x+rows, 1.0);
        }

    }
    if (verbose) {
        std::cerr << std::endl << "Inversion matrix:" << std::endl;
        std::cerr << inv << std::endl;
        std::cerr << "}}}" << std::endl;
    }
    ReducedRowEchelon(inv);
    if (verbose) {
        std::cerr << std::endl << "Inversion matrix in reduced row echelon form:" << std::endl;
        std::cerr << inv << std::endl;
        std::cerr << "}}}" << std::endl;
    }


    Matrix t_old = t;
    t.set(0, 0, 0.0);

    std::cerr << std::endl << t << std::endl;

    // calculate rows 1..M for phase 2
    for (unsigned x = 1; x < t.M; ++x) {
        for (unsigned y = 0; y < t.N; ++y) {
            double val = 0.0;
            for (unsigned z = 1; z < t.M; ++z) {
                val += inv.get(x-1, t.M-1+z-1) * t_old.get(z, y);
            }
            t.set(x, y, val);
        }
    }

    std::cerr << std::endl << t << std::endl;


    std::vector<double> cb(t.M-1, 0.0);
    for (unsigned x = 1; x < t.M; ++x) {
        cb.at(x-1) = t_old.get(0, a.getMapping(x));
    }

    for (unsigned y = 0; y < t.N; ++y) {
        double val = 0.0;
        for (unsigned x = 1; x < t.M; ++x) {
            val += cb.at(x-1) * t.get(x, y);
        }
        t.set(0, y, t.get(0, y) - val);
    }

    std::cerr << std::endl << t << std::endl;

    return true;
}

