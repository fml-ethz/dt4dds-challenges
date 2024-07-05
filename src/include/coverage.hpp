#ifndef COVERAGE_HPP
#define COVERAGE_HPP

#include <vector>

namespace coverage {

    std::vector<unsigned int> _sample_from_relative_coverage(std::vector<float> const &relative_coverage, const int n_oligos);

    std::vector<unsigned int> get_initial_coverage(const int n_sequences, const float log_std, const int coverage = 100);

    std::vector<unsigned int> sample_by_count(std::vector<unsigned int> const &oligo_counts, const int n_sampled_oligos);

} // namespace coverage


#endif