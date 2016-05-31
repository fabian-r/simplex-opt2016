#pragma once

#include <vector>
#include <iostream>

// because floats suck...
#define EPSILON 0.000000001
#define LESS(a,b) ((a)-(b) < -EPSILON)
#define EQ(a,b) (((a)-(b))*((a)-(b)) < EPSILON*EPSILON)


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
    std::vector<unsigned> Mapping_;

public:
    const double M; // number of rows
    const double N; // number of columns

public:
    Matrix(unsigned m, unsigned n) :
        Contents_(m*n, 0.0), Mapping_(m, 0.0), M(m), N(n)
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

    void Canonicalize(void)
    {
        for (unsigned i = 0; i < M*N; ++i) {
            if (EQ(0, Contents_.at(i))) {
                Contents_.at(i) = 0.0;
            }
        }
    }

    void setMapping(unsigned row, unsigned var)
    {
        Mapping_.at(row) = var;
    }

    unsigned getMapping(unsigned row) const
    {
        return Mapping_.at(row);
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

    static Matrix fromInput(std::istream& stream)
    {
        unsigned m, n;
        stream >> m;
        stream >> n;
        Matrix res(m, n);
        for (unsigned x = 0; x < res.M; ++x) {
            for (unsigned y = 0; y < res.N; ++y) {
                double val;
                stream >> val;
                res.set(x, y, val);
            }
        }
        return res;
    }

    void printMapping(std::ostream& stream) const
    {
        stream << "objective value: " << -this->get(0, 0) << std::endl;
        for (unsigned x = 1; x < this->M; ++x) {
            stream << "x" << this->getMapping(x) << " = " << this->get(x, 0)
                   << std::endl;
        }
    }
};


/**
 * Perform one iteration of the simplex method.
 *
 * Returns the termination state of the simplex method.
 */
Result PerformPivot(Matrix& t);

/**
 * Perform phase 1 of the full tableau simplex method.
 * The input matrix is expected to contain the cost in the zeroth row, starting
 * from the first entry.
 *
 * Returns true if the problem is feasible and false otherwise.
 */
bool Phase1(Matrix& t);

double Phase2(Matrix& t);
