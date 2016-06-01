#pragma once

#include <vector>
#include <iostream>

/**
 * Implementation of a matrix (with some additional information if used as full
 * tableau)
 *
 */
struct Matrix
{
private:
    std::vector<double> Contents_;
    std::vector<size_t> Mapping_;

public:
    size_t M; // number of rows
    size_t N; // number of columns

public:
    Matrix(size_t m, size_t n) :
        Contents_(m*n, 0.0), Mapping_(m, 0.0), M(m), N(n)
    {}

    /**
     *  Get entry in row i, column j (0 indexed).
     *
     *  Returns value.
     */
    double get(size_t i, size_t j) const;

    /**
     *  Set entry in row i, column j (0 indexed).
     */
    void set(size_t i, size_t j, double x);

    /**
     * Implementation of an elementary row operation.
     */
    void multiplyRowBy(size_t a, double d);

    /**
     * Implementation of an elementary row operation.
     */
    void addDTimesRowBToRowA(size_t a, size_t b, double d);

    /**
     * Perform the necessary operations such that B(l) leaves the basis and j
     * enters it.
     */
    void eliminate(size_t l, size_t j);

    /**
     * Remove row from matrix (0 indexed).
     */
    void removeRow(size_t row);

    /**
     * Use elementary row operations to transform into reduced row echelon form.
     */
    void reducedRowEchelon(bool fail_on_rank = true);

    /**
     * Throw away duplicate constraints.
     */
    void reduceToRank(void);

    /**
     * Set all matrix entries that are EQ to 0.0 to 0.0.
     * (Beautification because of floating point rounding errors)
     */
    void canonicalize(void);

    /**
     * Declare that the specified row corresponds to variable `var`.
     * (full tableau specific)
     */
    void setMapping(size_t row, size_t var);

    /**
     * Get the variable that the row corresponds to.
     * (full tableau specific)
     *
     * Returns the index of the variable.
     */
    size_t getMapping(size_t row) const;

    /**
     * Dump matrix.
     */
    friend std::ostream& operator<< (std::ostream& stream, const Matrix& m);

    /**
     * Factory method for creating a matrix from an istream.
     */
    static Matrix fromInput(std::istream& stream);

    /**
     * Factory method for creating a randomized mxn matrix with entries in
     * [-range, +range]
     */
    static Matrix fromRandom(size_t m, size_t n, size_t range);

    /**
     * Print objective value and values for variables.
     * (full tableau specific)
     */
    void printMapping(std::ostream& stream) const;
};

