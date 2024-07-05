#include <vector>
#include <stdexcept>

#include "helpers.hpp"


namespace helpers {

    std::string sequence_vector_to_int(const std::vector<char>& sequence_vector) {
        std::vector<char> sequence_int;
        sequence_int.push_back('[');
        for (char c : sequence_vector) {
            sequence_int.push_back(c + '0');
            sequence_int.push_back(',');
        }
        sequence_int.push_back(']');
        std::string sequence(sequence_int.begin(), sequence_int.end());
        return sequence;
    }

    std::string sequence_vector_to_ACGT(const std::vector<char>& sequence_vector) {
        std::vector<char> sequence_ACGT;
        for (int i : sequence_vector) {
            switch (i) {
                case 0:
                    break;
                case 1:
                    sequence_ACGT.push_back('A');
                    break;
                case 2:
                    sequence_ACGT.push_back('C');
                    break;
                case 3:
                    sequence_ACGT.push_back('G');
                    break;
                case 4:
                    sequence_ACGT.push_back('T');
                    break;
                default:
                    throw std::invalid_argument("Invalid character in sequence" + std::to_string(i));
            }
        }
        std::string sequence(sequence_ACGT.begin(), sequence_ACGT.end());
        return sequence;
    }


} // namespace helpers
