#include <random>
#include <stdexcept>
#include <vector>
#include <numeric>

#include "coverage.hpp"
#include "rng.hpp"
#include "logging.hpp"

static Logger logger("coverage", "INFO");

namespace coverage {

    // sample a specific number of oligos from a probability distribution with replacement
    std::vector<unsigned int> _sample_from_relative_coverage(std::vector<float> const &relative_coverage, const int n_oligos) {
        
        // check if the number of sampled oligos is less than 1
        if (n_oligos < 1) {
            logger.critical("Number of sampled oligos is less than 1. Please increase the coverage.");
            throw std::invalid_argument("Number of sampled oligos is less than 1. Please increase the coverage.");
        }

        // check if there are sequences to sample from
        if (relative_coverage.size() < 1) {
            logger.critical("There are not sequences to sample from. Please check the input file and coverage settings.");
            throw std::invalid_argument("There are not sequences to sample from. Please check the input file and coverage settings.");
        }

        // create a discrete distribution
        std::discrete_distribution<> sampler(relative_coverage.begin(), relative_coverage.end());

        // sample the distribution for n_oligos times
        std::vector<unsigned int> sampled_coverage(relative_coverage.size(), 0);
        for (int i = 0; i < n_oligos; i++) {
            sampled_coverage[sampler(rng::rng)]++;
        }

        // calculate total number of oligos
        int sampled_oligos_total = std::accumulate(sampled_coverage.begin(), sampled_coverage.end(), 0);
        logger.info("Sampled total of {} oligos from {} sequences for a mean coverage of {}", sampled_oligos_total, relative_coverage.size(), (float)sampled_oligos_total / (float)relative_coverage.size());
        return sampled_coverage;
    }


    // sample from an initial relative coverage assuming a log-normal distribution of coverage
    std::vector<unsigned int> get_initial_coverage(const int n_sequences, const float log_std, const int coverage) {
        
        // create lognormal distribution with a mean of 0 and a standard deviation of log_std
        std::lognormal_distribution<float> lognorm_dist(0.0, log_std);

        // generate a coverage for each sequence and normalize it
        std::vector<float> rel_cov(n_sequences, 0.0);
        for (int i = 0; i < n_sequences; i++) {
            rel_cov[i] = lognorm_dist(rng::rng);
        }
        float sum = std::accumulate(rel_cov.begin(), rel_cov.end(), 0.0);
        for (int i = 0; i < n_sequences; i++) {
            rel_cov[i] /= sum;
        }

        // convert to absolute coverage by sampling
        std::vector<unsigned int> sampled_oligos = _sample_from_relative_coverage(rel_cov, (int)n_sequences*coverage);
        return sampled_oligos;
    }


    // sample a specific number of oligos from a set of oligo counts representing abundance
    std::vector<unsigned int> sample_by_count(std::vector<unsigned int> const &oligo_counts, const int n_sampled_oligos) {

        // get number of unique oligos
        const int n_oligos = oligo_counts.size();

        // convert the absolute coverage to a relative coverage
        std::vector<float> rel_cov = std::vector<float>(n_oligos, 0.0);
        double total_oligos = std::accumulate(oligo_counts.begin(), oligo_counts.end(), 0.0);
        for (int i = 0; i < n_oligos; i++) {
            rel_cov[i] = (float)oligo_counts[i] / total_oligos;
        }

        // sample the oligos with replacement
        std::vector<unsigned int> sampled_oligos = _sample_from_relative_coverage(rel_cov, n_sampled_oligos);
        return sampled_oligos;
    }

} // namespace coverage