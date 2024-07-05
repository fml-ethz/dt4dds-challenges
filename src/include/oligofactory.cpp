#include <vector>
#include <numeric>
#include <memory>

#include "constants.hpp"
#include "mutator.hpp"
#include "logging.hpp"

static Logger logger("oligofactory", "INFO");

namespace oligofactory {

    // function to generate oligos from a sequence and a set of mutators
    void produce_from_sequence(
        std::vector<std::vector<char>> &oligo_vectors,
        std::vector<char> const &sequence_vector,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& mutators
        ) {

        // clear the oligo vectors and insert the original sequence
        oligo_vectors.clear();
        oligo_vectors.push_back(sequence_vector);

        // apply each mutator to the oligo vectors
        for (std::unique_ptr<mutator::BaseMutator> &mutator : mutators) {
            mutator->process(oligo_vectors);
        }
    }


    // function to generate #n_oligos different oligos from a sequence given a set of mutators
    void generate_oligos(
        std::vector<std::vector<char>> &generated_oligos,
        std::vector<char> const &sequence_vector, 
        unsigned int n_oligos,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& mutators
        ) {
        
        // short-circuit if there are no reads to generate
        if (n_oligos == 0) {
            return;
        }
        // short-circuit if there are no mutators
        if (mutators.size() == 0) {
            for (int i = 0; i < n_oligos; i++) {
                generated_oligos.push_back(sequence_vector);
            }
            return;
        }

        // this will hold all of the oligos while they move through the error pipeline
        std::vector<std::vector<char>> oligo_vectors;

        // loop through each oligo to be generated from this sequence
        for (int i_oligo = 0; i_oligo < n_oligos; i_oligo++) {

            // generate the oligos derived from the current sequence
            produce_from_sequence(
                oligo_vectors,
                sequence_vector,
                mutators
            );

            // copy the oligos to the generated oligos vector
            for (std::vector<char> &oligo_vector : oligo_vectors) {
                generated_oligos.push_back(oligo_vector);
            }
        }
    }




} // namespace oligofactory