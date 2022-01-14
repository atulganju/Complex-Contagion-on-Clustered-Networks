//
// Created by Adith Ramachandran on 12/3/21.
//

#include "global_defs.hpp"

int klik::rand_minmax(const std::string d)
{
    std::random_device rd;
    std::mt19937 mt(rd());

    if (d == "b")
    {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(mt) < BIN_PROBABILITY ? BIN_THRESHOLD : 1;
    }
    if (d == "u")
    {
        std::uniform_int_distribution<int> dist(MIN_THRESHOLD, MAX_THRESHOLD);
        return dist(mt);
    }
    if (d == "p")
    {
        std::poisson_distribution<int> dist(PSN_PARAMETER);
        return dist(mt);
    }
    return -1;
}

bool klik::prob_true()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    return dist(mt) < SEED_PROBABILITY;
}