#include <cstring>
#include <cstdlib>
#include <limits>
#include "impl.h"

#define NUM_TESTS 100

int main(int argc, char *argv[])
{

    bool do_experiments = false;
    long seed = 1;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--experiments") == 0) {
            do_experiments = true;
            if (argc > i+1) {
                seed = strtol(argv[i+1], nullptr, 10);
                if (seed != 0) {
                    i++;
                } else {
                    seed = 1;
                }
            }
        } else {
            std::cout << "Simplex tool - Usage:" << std::endl;
            std::cout << "  simplex [flags]" << std::endl;
            std::cout << "The following flags are supported:" << std::endl;
            std::cout << " -h, --help                      show this help"
                      << std::endl;
            std::cout << " -v, --verbose                   show detailed steps"
                      << std::endl;
            std::cout << " -e [<s>], --experiments [<s>]   perform experiments"
                      << " with optional RNG seed <s>" << std::endl;
            std::cout << "If -e is not given, a tableau is expected from stdin."
                      << std::endl;
            exit(13);
        }
    }

    if (do_experiments) {
        std::srand(seed);

        unsigned M[] = { 4, 8, 16 };
        unsigned N[] = { 0, 4, 8 };
        unsigned Range[] = { 4, 16, 256 };

        for (unsigned i : M) {
            for (unsigned j : N) {
                unsigned infeasible = 0;
                unsigned unbounded = 0;
                unsigned finite = 0;

                for (unsigned r : Range) {
                    std::cout << i << " - " << i+j << " - " << r << std::endl;
                    for (unsigned v = 0; v < NUM_TESTS; ++v) {
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
                    std::cout << infeasible << " - " << unbounded << " - " << finite << std::endl;
                }
            }
        }
    } else {
        Matrix m = Matrix::fromInput(std::cin);
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

    return 0;
}
