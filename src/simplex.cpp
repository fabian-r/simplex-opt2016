#include <cstring>
#include "impl.h"

int main(int argc, char *argv[])
{

    bool do_experiments = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--experiments") == 0) {
            do_experiments = true;
        } else {
            std::cout << "Simplex tool - Usage:" << std::endl;
            std::cout << "  simplex [flags]" << std::endl;
            std::cout << "The following flags are supported:" << std::endl;
            std::cout << " -h, --help         show this help" << std::endl;
            std::cout << " -v, --verbose      show detailed steps" << std::endl;
            std::cout << " -e --experiments   perform experiments" << std::endl;
        }
    }


    if (do_experiments) {

    } else {
        Matrix m = Matrix::fromInput(std::cin);
        std::cout << "Input:" << std::endl << m << std::endl;
        Phase1(m);
        Phase2(m);
        std::cout << "Final tableau:" << std::endl << m << std::endl;
        m.printMapping(std::cout);
        std::cout << std::endl;
    }

    return 0;
}
