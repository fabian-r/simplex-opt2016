#include "impl.h"



int main(void)
{
    Matrix m(4, 7);
    m.set(0, 0, 0);
    m.set(0, 1, -10);
    m.set(0, 2, -12);
    m.set(0, 3, -12);
    m.set(0, 4, 0);
    m.set(0, 5, 0);
    m.set(0, 6, 0);

    m.set(1, 0, 20);
    m.set(1, 1, 1);
    m.set(1, 2, 2);
    m.set(1, 3, 2);
    m.set(1, 4, 1);
    m.set(1, 5, 0);
    m.set(1, 6, 0);

    m.set(2, 0, 20);
    m.set(2, 1, 2);
    m.set(2, 2, 1);
    m.set(2, 3, 2);
    m.set(2, 4, 0);
    m.set(2, 5, 1);
    m.set(2, 6, 0);

    m.set(3, 0, 20);
    m.set(3, 1, 2);
    m.set(3, 2, 2);
    m.set(3, 3, 1);
    m.set(3, 4, 0);
    m.set(3, 5, 0);
    m.set(3, 6, 1);

    std::cout << m;
    std::cout << "=====" << std::endl;
    auto res = PerformPivot(m);
    std::cout << m;
    std::cout << "=====" << std::endl << res << std::endl;
    return 0;
}
