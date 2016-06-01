#pragma once

#include "matrix.h"

/**
 * Result of a single pivot step.
 */
enum Result
{
    UNBOUNDED,
    OPTIMAL,
    NONOPTIMAL
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

/**
 * Perform phase 2 of the full tableau simplex method.
 *
 * Returns the achieved optimal objective value (can be -infinity).
 */
double Phase2(Matrix& t);

/**
 * Read a tableau from stream and solve it.
 */
void SolveFromStream(std::istream& stream);

/**
 * Perform the experiments described in exercise (d).
 * The test_factor determines the input size for the experiments.
 */
void PerformExperiments(long seed, long test_factor, unsigned long num_runs);
