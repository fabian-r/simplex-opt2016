#pragma once

// For handling floating point imprecision.
#define EPSILON 0.0000001
#define LESS(a,b) ((a)-(b) < -EPSILON)
#define EQ(a,b) (((a)-(b))*((a)-(b)) < EPSILON*EPSILON)

// Number of experiments to perform with each configuration.
#define NUM_TESTS 100


extern bool verbose;


