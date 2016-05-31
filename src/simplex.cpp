#include <cstring>
#include "impl.h"

int main(int argc, char *argv[])
{

    bool from_stdin = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--stdin") == 0) {
            from_stdin = true;
        } else {
            std::cout << "Simplex tool - Usage:" << std::endl;
            std::cout << "  simplex [flags]" << std::endl;
            std::cout << "The following flags are supported:" << std::endl;
            std::cout << " -h, --help      show this help" << std::endl;
            std::cout << " -v, --verbose   show detailed steps" << std::endl;
            std::cout << " --stdin         expect input full tableau from stdin"
                << std::endl;
        }
    }

    if (from_stdin) {
        Matrix m = Matrix::fromInput(std::cin);
        std::cout << m;
        Phase1(m);
        // Phase2(m);
        std::cout << m;
        std::cout << std::endl;
        m.printMapping(std::cout);
        std::cout << std::endl;
    }

    // std::cout << "Initial: {{{" << std::endl;
    // std::cout << m << std::endl;;
    // std::cout << "}}}" << std::endl;
    //
    // auto res = Phase2(m);
    //
    // std::cout << "Final: {{{" << std::endl;
    // std::cout << m;
    // std::cout << "}}}" << std::endl;
    // std::cout << "Result: " << std::endl << res << std::endl;
    //
    // Phase1(m);
    return 0;
}
