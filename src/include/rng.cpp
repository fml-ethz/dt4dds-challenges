#include <random>

#include "rng.hpp"


namespace rng {

    std::mt19937 rng;

    void seed_rng(unsigned int seed) {
        rng.seed(seed);
    }

    float random_float() {
        std::uniform_real_distribution<float> dist(0.0, 1.0);
        return dist(rng);
    }

    int random_int(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }
}