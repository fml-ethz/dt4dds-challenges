#ifndef OLIGOFACTORY_HPP
#define OLIGOFACTORY_HPP

#include <vector>
#include <memory>

#include "mutator.hpp"


namespace oligofactory {

    void produce_from_sequence(
        std::vector<std::vector<char>> &generated_oligos,
        std::vector<char> const &sequence_vector,
        std::vector<std::unique_ptr<mutator::BaseMutator>> &mutators
    );

    void generate_oligos(
        std::vector<std::vector<char>> &generated_oligos,
        std::vector<char> const &sequence_vector, 
        unsigned int n_oligos,
        std::vector<std::unique_ptr<mutator::BaseMutator>> &mutators
    );

} // namespace oligofactory


#endif // OLIGOFACTORY_HPP