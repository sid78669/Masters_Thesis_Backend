#ifndef HELPER_H_
#define HELPER_H_

#include <random>
#include <cmath>

#define SEED 11041986

class Helper {
public:
    std::mt19937 rand_gen;
    std::random_device plant;
    unsigned long seed;

    Helper( ) {
        seed = SEED;
        rand_gen.seed(seed);
    }

    inline double randNum( ) {
        std::uniform_real_distribution<> random(0, 1);
        return random(rand_gen);
    }

    inline int randNum(int min, int max) {
        std::uniform_int_distribution<> random(min, max);
        return random(rand_gen);
    }

    inline double randDouble(double min, double max) {
        std::uniform_real_distribution<> random(min, max);
        return random(rand_gen);
    }

    inline bool randBoolean( ) {
        std::uniform_int_distribution<> random(0, 1);
        return (random(rand_gen) % 2) == 0;
    }

};

#endif