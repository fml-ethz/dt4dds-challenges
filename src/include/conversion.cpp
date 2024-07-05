#include <string>
#include <stdexcept>
#include <vector>

#include "conversion.hpp"
#include "constants.hpp"
#include "logging.hpp"

static Logger logger("conversion", "INFO");

namespace conversion {

    // function to convert a sequence to a vector of integers
    void sequence_to_vector(const std::string& sequence, std::vector<char>& sequence_vector) {
        // clear the sequence vector
        sequence_vector.clear();

        // iterate over the sequence and convert the characters to integers
        for (int i = 0; i < sequence.size(); i++) {
            switch (sequence[i]) {
                case 'A':
                    sequence_vector.push_back(constants::NUCLEOTIDE_A);
                    break;
                case 'C':
                    sequence_vector.push_back(constants::NUCLEOTIDE_C);
                    break;
                case 'G':
                    sequence_vector.push_back(constants::NUCLEOTIDE_G);
                    break;
                case 'T':
                    sequence_vector.push_back(constants::NUCLEOTIDE_T);
                    break;
                default:
                    logger.critical("Invalid character in sequence: {}", sequence[i]);
                    throw std::runtime_error("Invalid character in sequence: " + sequence[i]);
            }
        }
    }


    // function to convert a vector of integers to a sequence
    std::string vector_to_sequence(const std::vector<char>& sequence_vector) {
        // create a string to hold the sequence
        std::string sequence;
        sequence.reserve(sequence_vector.size());

        // iterate over the sequence and convert the integers to characters
        for (int i = 0; i < sequence_vector.size(); i++) {
            switch (sequence_vector[i]) {
                case constants::NUCLEOTIDE_A:
                    sequence.push_back('A');
                    break;
                case constants::NUCLEOTIDE_C:
                    sequence.push_back('C');
                    break;
                case constants::NUCLEOTIDE_G:
                    sequence.push_back('G');
                    break;
                case constants::NUCLEOTIDE_T:
                    sequence.push_back('T');
                    break;
                default:
                    logger.critical("Invalid integer in sequence: {}", sequence_vector[i]);
                    throw std::runtime_error("Invalid integer in sequence: " + sequence_vector[i]);
            }
        }
        return sequence;
    }


    // function to convert a vector sequence to its reverse complement
    std::vector<char> reverse_complement(const std::vector<char>& sequence_vector) {
        // create a vector to hold the reverse complement
        std::vector<char> reverse_complement_vector;
        reverse_complement_vector.reserve(sequence_vector.size());

        // iterate over the sequence and convert the characters to integers
        for (int i = sequence_vector.size() - 1; i >= 0; i--) {
            switch (sequence_vector[i]) {
                case constants::NUCLEOTIDE_A:
                    reverse_complement_vector.push_back(constants::NUCLEOTIDE_T);
                    break;
                case constants::NUCLEOTIDE_C:
                    reverse_complement_vector.push_back(constants::NUCLEOTIDE_G);
                    break;
                case constants::NUCLEOTIDE_G:
                    reverse_complement_vector.push_back(constants::NUCLEOTIDE_C);
                    break;
                case constants::NUCLEOTIDE_T:
                    reverse_complement_vector.push_back(constants::NUCLEOTIDE_A);
                    break;
                default:
                    logger.critical("Invalid character in sequence: {}", sequence_vector[i]);
                    throw std::runtime_error("Invalid character in sequence: " + sequence_vector[i]);
            }
        }
        return reverse_complement_vector;
    }



    
}
