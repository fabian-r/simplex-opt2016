#include "matrix.h"

#include <cstdlib>
#include <cassert>

#include "util.h"


double Matrix::get(size_t i, size_t j) const
{
    return Contents_[i*N + j];
}

void Matrix::set(size_t i, size_t j, double x)
{
    Contents_[i*N + j] = x;
}

void Matrix::multiplyRowBy(size_t a, double d)
{
    for (size_t y = 0; y < N; ++y) {
        this->set(a, y, this->get(a, y) * d);
    }
}

void Matrix::addDTimesRowBToRowA(size_t a, size_t b, double d)
{
    double *p = Contents_.data();
    for (size_t y = 0; y < N; ++y) {
        p[a*N + y] += d*p[b*N + y];
    }
}

void Matrix::eliminate(size_t l, size_t j)
{
    // update the mapping from columns to variables
    this->setMapping(l, j);

    if (verbose) {
        std::cerr << " ~~> (" << l << ") = (" << l << ") * "
                  << 1/this->get(l, j) << std::endl;
    }

    // perform elementary row operations
    this->multiplyRowBy(l, 1/this->get(l, j));

    for (size_t x = 0; x < this->M; ++x) {
        if (x == l)
            continue;
        if (verbose) {
            std::cerr << " ~~> (" << x << ") = (" << x << ") + (" << l << ") * "
                      << -this->get(x, j) << std::endl;
        }
        this->addDTimesRowBToRowA(x, l, -this->get(x, j));
    }
}

void Matrix::reducedRowEchelon(bool fail_on_rank)
{
    this->canonicalize();
    for (size_t x = 0; x < this->M; ++x) {
        if (EQ(this->get(x, x), 0)) {
            bool success = false;
            for (size_t y = x; y < this->M; ++y) {
                if (EQ(this->get(y, x), 0))
                    continue;
                this->addDTimesRowBToRowA(x, y, 1.0);
                success = true;
            }
            if (fail_on_rank && ! success) {
                std::cerr << *this << std::endl;
            }
            assert(!fail_on_rank || (success &&
                   "invalid input for reduced row echelon algorithm!"));
            if (! success) {
                continue;
            }
        }
        this->eliminate(x, x);
    }
}

void Matrix::reduceToRank(void)
{
    Matrix other(*this);
    other.reducedRowEchelon(false);
    for (int x = M-1; x >= 0; --x) {
        if (EQ(0, other.get(x,x))) {
            this->removeRow(x);
        }
    }
}

void Matrix::removeRow(size_t row)
{
    for (size_t v = row * N; v < N*(M-1); ++v) {
        Contents_.at(v) = Contents_[v+N];
    }
    Contents_.resize(N*(M-1));
    for (size_t i = row; i < (M-1); ++i) {
        Mapping_.at(i) = Mapping_[i+1];
    }
    Mapping_.resize(M-1);
    M = M-1;
}

void Matrix::canonicalize(void)
{
    for (size_t i = 0; i < M*N; ++i) {
        if (EQ(0, Contents_[i])) {
            Contents_[i] = 0.0;
        }
    }
}

void Matrix::setMapping(size_t row, size_t var)
{
    Mapping_[row] = var;
}

size_t Matrix::getMapping(size_t row) const
{
    return Mapping_[row];
}

std::ostream& operator<< (std::ostream& stream, const Matrix& m)
{
    stream << m.M << " " << m.N << std::endl;
    for (size_t x = 0; x < m.M; ++x) {
        for (size_t y = 0; y < m.N; ++y) {
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

Matrix Matrix::fromInput(std::istream& stream)
{
    size_t m, n;
    stream >> m;
    stream >> n;
    Matrix res(m, n);
    for (size_t x = 0; x < res.M; ++x) {
        for (size_t y = 0; y < res.N; ++y) {
            double val;
            stream >> val;
            res.set(x, y, val);
        }
    }
    return res;
}

Matrix Matrix::fromRandom(size_t m, size_t n, size_t range)
{
    Matrix res(m, n);
    for (size_t x = 0; x < res.M; ++x) {
        for (size_t y = 0; y < res.N; ++y) {
            long val = (std::rand() % (2*(long)range+1)) - (long)range;
            res.set(x, y, val);
        }
    }
    return res;
}

void Matrix::printMapping(std::ostream& stream) const
{
    stream << "objective value: " << -this->get(0, 0) << std::endl;
    for (size_t x = 1; x < this->M; ++x) {
        stream << "x" << this->getMapping(x) << " = " << this->get(x, 0)
               << std::endl;
    }
}
