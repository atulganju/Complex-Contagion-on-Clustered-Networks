//
// Created by Adith Ramachandran on 11/30/21.
//

#pragma once

#include <stdlib.h>
#include <random>

#define SEED_PROBABILITY 0.01
#define TOTAL_NODES 120000

// Bimodal Distribution Constants
#define BIN_PROBABILITY 0.9
#define BIN_THRESHOLD 5

// Uniform Distribution Constants
#define MIN_THRESHOLD 3
#define MAX_THRESHOLD 3

// Poisson Distribution Constants
#define PSN_PARAMETER 2

namespace klik
{
    typedef uint64_t uint64;

    int rand_minmax(const std::string d);
    bool prob_true();
}
