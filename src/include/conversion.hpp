#ifndef CONVERSION_HPP
#define CONVERSION_HPP

#include <string>
#include <vector>
#include <fstream>


namespace conversion {

    // function to convert a sequence to a vector of integers
    void sequence_to_vector(const std::string& sequence, std::vector<char>& sequence_vector);

    // function to convert a vector of integers to a sequence
    std::string vector_to_sequence(const std::vector<char>& sequence_vector);

    // function to convert a vector sequence to its reverse complement
    std::vector<char> reverse_complement(const std::vector<char>& sequence_vector);

} 



#endif // CONVERSION_HPP
