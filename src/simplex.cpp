#include <cstring>
#include <ctime>
#include <csignal>

#include "impl.h"
#include "util.h"

void onAbort(int);

bool verbose = false;

int main(int argc, char *argv[])
{
    std::signal(SIGABRT, onAbort);

    // parse cmdline arguments
    bool do_experiments = false;
    long seed = 1; // RNG seed
    long test_factor = 1; // controls size of the experiments
    long num_runs = 100;
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
                    seed = time(NULL);
                }
            }
        } else if (strcmp(argv[i], "--runs") == 0) {
            if (argc > i+1) {
                num_runs = strtol(argv[i+1], nullptr, 10);
                if (num_runs != 0) {
                    i++;
                } else {
                    num_runs = 100;
                }
            }
        } else if (strcmp(argv[i], "--custom") == 0) {
            if (argc > i+1) {
                test_factor = strtol(argv[i+1], nullptr, 10);
                if (test_factor != 0) {
                    i++;
                } else {
                    test_factor = 0;
                }
            }
        } else if (strcmp(argv[i], "--large") == 0) {
            test_factor = 2;
        } else if (strcmp(argv[i], "--huge") == 0) {
            test_factor = 4;
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
            std::cout << std::endl;
            std::cout << "The experiment configurations can be"
                      << " influenced with the following additional flags:"
                      << std::endl;
            std::cout << " --large                       large input size"
                      << std::endl;
            std::cout << " --huge                        huge input size"
                      << std::endl;
            std::cout << " --custom [<s>]                custom input size"
                      << std::endl;
            std::cout << " --runs [<s>]                  number of runs"
                      << std::endl;
            std::cout << std::endl;
            std::cout << "If -e is not given, a tableau is expected from stdin."
                      << std::endl;
            exit(13);
        }
    }

    // actually do something
    if (do_experiments) {
        PerformExperiments(seed, test_factor, num_runs);
    } else {
        SolveFromStream(std::cin);
    }

    return 0;
}

void onAbort(int n)
{
    std::cerr << std::endl
              << "Great! Looks like you found a new feature!" << std::endl
              << std::endl
              << "           / .'   " << std::endl
              << "     .---. \\/     " << std::endl
              << "    (._.' \\()     " << std::endl
              << "     ^\"\"\"^\"       " << std::endl;
    exit(n);
}
