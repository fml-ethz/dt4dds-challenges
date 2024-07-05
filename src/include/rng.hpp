#ifndef RNG_HPP
#define RNG_HPP

#include <random>

namespace rng {
    extern std::mt19937 rng;

    void seed_rng(unsigned int seed);

    float random_float();

    int random_int(int min, int max);
}


#endif // RNG_HPP
