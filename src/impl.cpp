#include "impl.h"

#include <limits>
#include <cassert>

#include "matrix.h"
#include "util.h"

static size_t pivot_counter = 0;

Result PerformPivot(Matrix& t)
{
    ++pivot_counter;
    // choose first j with reduced cost < 0
    size_t j = 0;
    for (size_t y = 1; y < t.N; ++y) {
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

    // choose l that minimizes x_B(l) / u_l with u_l > 0
    size_t l = 0;
    std::vector<double> min(t.N, std::numeric_limits<double>::infinity());

    // implement lexicographic pivoting rule
    for (size_t x = 1; x < t.M; ++x) {
        double ui = t.get(x, j);
        if (LESS(0, ui)) {
            for (size_t y = 0; y < t.N; ++y) {
                double val = t.get(x, y) / ui;
                if (LESS(val, min[y])) {
                    // x is lexico-smaller
                    for (size_t z = 0; z < t.N; ++z) {
                        min[z] = t.get(x, z) / ui;
                    }
                    l = x;
                    goto outer;
                } else if (LESS(min[y], val)) {
                    // x is lexico-larger
                    goto outer;
                }
            }
        }
outer:;
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

    t.eliminate(l, j);

    if (verbose)
        std::cerr << " > non-optimal" << std::endl;
    return NONOPTIMAL;
}

bool Phase1(Matrix& t)
{
    t.canonicalize();
    t.reduceToRank();
    // create tableau for artificial problem
    Matrix a(t.M, t.N + t.M-1);

    for (size_t x = 1; x < t.M; ++x) {
        // factor for making every entry of b >= 0
        double sign_factor = (LESS(t.get(x, 0), 0.0)) ? -1.0 : 1.0;

        // set non-artificial entrys of the artificial tableau
        for (size_t y = 0; y < t.N; ++y) {
            a.set(x, y, sign_factor * t.get(x, y));
            a.set(0, y, a.get(0, y) - sign_factor * t.get(x, y));
        }

        // set entries corresponding to the artificial variables
        for (size_t y = t.N; y < a.N; ++y) {
            if (y - t.N == x - 1) {
                a.set(x, y, 1.0);
                a.setMapping(x, y);
                break;
            }
        }
    }

    if (verbose) {
        std::cerr << "Phase 1: {{{" << std::endl;
        std::cerr << " artificial LP :" << std::endl;
        std::cerr << a << std::endl;
    }

    // solve artificial LP
    double res = Phase2(a);
    if (! EQ(res, 0))
        return false;

    // create basis from solution
    for (size_t x = 1; x < a.M; ++x) {
        if (a.getMapping(x) < t.N)
            continue;
        // artificial basic variable, has to be eliminated
        if (verbose) {
            std::cerr << " Eliminate artificial variable x"
                      << a.getMapping(x) << "...";
        }

        bool success = false;

        // search for non-artificial variable to enter basis instead
        for (size_t y = 1; y < t.N; ++y) {
            if (EQ(a.get(x, y), 0))
                continue;

            if (verbose) {
                std::cerr << " and insert x" << y << " instead" << std::endl;
            }
            a.eliminate(x, y);
            success = true;
            break;
        }

        if (! success) {
            // we cannot eliminate the variable from the basis
            // => the constraint is redundant
            if (verbose) {
                std::cerr << " failed to eliminate!" << std::endl;
            }
            a.removeRow(x);
            t.removeRow(x);
        }
    }

    if (verbose) {
        std::cerr << std::endl << "Solution of artificial LP:" << std::endl;
        std::cerr << a << std::endl;
        a.printMapping(std::cerr);
    }

    // calculate inverted basis matrix AB^-1 using Gauss-Jordan Algorithm
    size_t rows = a.M-1;
    Matrix inv(rows, 2*rows);
    for (size_t x = 0; x < rows; ++x) {
        size_t var = a.getMapping(x+1);
        for (size_t y = 0; y < rows; ++y) {
            inv.set(y, x, t.get(y+1, var));
            if (y == x)
                inv.set(y, x+rows, 1.0);
        }
    }

    if (verbose) {
        std::cerr << std::endl << "Inversion matrix:" << std::endl;
        std::cerr << inv << std::endl;
    }
    inv.reducedRowEchelon();
    if (verbose) {
        std::cerr << std::endl << "Inversion matrix in reduced row echelon form:" << std::endl;
        std::cerr << inv << std::endl;
    }

    // inverted matrix is in the right half of `inv`

    inv.canonicalize();
    t.canonicalize();

    Matrix t_old = t;
    t.set(0, 0, 0.0);

    // calculate rows 1..M for phase 2 (AB^-1 * b|A)
    for (size_t x = 1; x < t.M; ++x) {
        for (size_t y = 0; y < t.N; ++y) {
            double val = 0.0;
            for (size_t z = 1; z < t.M; ++z) {
                val += inv.get(x-1, t.M-1+z-1) * t_old.get(z, y);
            }
            t.set(x, y, val);
        }
    }
    t.canonicalize();

    // set up correct mapping
    for (size_t x = 1; x < t.M; ++x) {
        t.setMapping(x, a.getMapping(x));
    }

    // compute cB
    std::vector<double> cb(t.M-1, 0.0);
    for (size_t x = 1; x < t.M; ++x) {
        cb.at(x-1) = t_old.get(0, a.getMapping(x));
    }

    // calculate zeroth row
    for (size_t y = 0; y < t.N; ++y) {
        double val = 0.0;
        for (size_t x = 1; x < t.M; ++x) {
            val += cb.at(x-1) * t.get(x, y);
        }
        t.set(0, y, t.get(0, y) - val);
    }
    t.canonicalize();

    if (verbose) {
        std::cerr << std::endl << "Tableau after phase 1:" << std::endl;
        std::cerr << t << std::endl;
        std::cerr << "}}}" << std::endl;
    }

    return true;
}

double Phase2(Matrix& t)
{
    t.canonicalize();
    if (verbose) {
        std::cerr << "Phase 2: {{{" << std::endl;
    }
    Result res;
    size_t num = 0;
    // perform pivot steps until termination
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

    t.canonicalize();

    if (res == UNBOUNDED) {
        return - std::numeric_limits<double>::infinity();
    }

    if (verbose) {
        std::cerr << "}}}" << std::endl;
    }
    return - t.get(0, 0);
}

void SolveFromStream(std::istream& stream)
{
    Matrix m = Matrix::fromInput(stream);
    std::cout << "Input:" << std::endl << m << std::endl;
    if (! Phase1(m)) {
        std::cout << "Infeasible" << std::endl;
    } else {
        Phase2(m);
        std::cout << "Final tableau:" << std::endl << m << std::endl;
        m.printMapping(std::cout);
        std::cout << std::endl;
    }
}

void PerformExperiments(long seed, long test_factor)
{
    std::srand(seed);

    static size_t M[] = { 4, 16, 32 };
    static size_t N[] = { 0, 16, 32 };
    static size_t Range[] = { 4, 256, 1024 };

    size_t counter = 0;

    for (size_t i : M) {
        i *= test_factor;
        for (size_t j : N) {
            j *= test_factor;
            for (size_t r : Range) {
                r *= test_factor;
                pivot_counter = 0;
                size_t infeasible = 0;
                size_t unbounded = 0;
                size_t finite = 0;
                std::cout << "Configuration " << ++counter;
                std::cout << " ( n = " << i << ", ";
                std::cout << " m = " << i+j << ", ";
                std::cout << " N = " << r << "), " << NUM_TESTS
                          << " runs:" << std::endl;
                for (size_t v = 0; v < NUM_TESTS; ++v) {
                    Matrix m = Matrix::fromRandom(i, i+j, r);
                    if (! Phase1(m)) {
                        infeasible ++;
                    } else {
                        double res = Phase2(m);
                        if (res == -std::numeric_limits<double>::infinity()) {
                            unbounded++;
                        } else {
                            finite++;
                        }
                    }
                }
                double res_finite = 100.0*((double)finite / (double)NUM_TESTS);
                double res_unbounded = 100.0*((double)unbounded / (double)NUM_TESTS);
                double res_infeasible = 100.0*((double)infeasible / (double)NUM_TESTS);
                double res_pivots = ((double)pivot_counter / (double)NUM_TESTS);
                std::cout << "  finite:      " << res_finite << "\%" << std::endl;
                std::cout << "  unbounded:   " << res_unbounded << "\%" << std::endl;
                std::cout << "  infeasible:  " << res_infeasible << "\%" << std::endl;
                std::cout << "  pivots(avg): " << res_pivots << std::endl;
                std::cout << std::endl;
            }
        }
    }
}
