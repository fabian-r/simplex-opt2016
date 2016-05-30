#pragma once

#include <vector>
#include <iostream>

extern bool verbose;

enum Result
{
    UNBOUNDED,
    OPTIMAL,
    NONOPTIMAL
};

struct Matrix
{
private:
    std::vector<double> Contents_;

public:
    const double M; // number of rows
    const double N; // number of columns

public:

    Matrix(unsigned m, unsigned n) : Contents_(m*n, 0.0), M(m), N(n)
    {
    }

    double get(unsigned i, unsigned j) const
    {
        return Contents_.at(i*N + j);
    }

    void set(unsigned i, unsigned j, double x)
    {
        Contents_.at(i*N + j) = x;
    }

    void multiplyRowBy(unsigned a, double d)
    {
        for (unsigned y = 0; y < N; ++y) {
            this->set(a, y, this->get(a, y) * d);
        }
    }

    void addDTimesRowBToRowA(unsigned a, unsigned b, double d)
    {
        for (unsigned y = 0; y < N; ++y) {
            this->set(a, y, this->get(a, y) + this->get(b, y) * d);
        }
    }

    friend std::ostream& operator<< (std::ostream& stream, const Matrix& m)
    {
        stream << m.M << " " << m.N << std::endl;
        for (unsigned x = 0; x < m.M; ++x) {
            for (unsigned y = 0; y < m.N; ++y) {
                stream << m.get(x, y);
                if (y != m.N - 1)
                    stream << ' ';
            }
            if (x != m.M - 1)
                stream << '\n';
        }
        stream << std::endl;
        return stream;
    }
};


Result PerformPivot(Matrix& t);

double Phase2(Matrix& t);
