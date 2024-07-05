#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <vector>
#include <string>


namespace helpers {

    std::string sequence_vector_to_int(const std::vector<char>& sequence_vector);

    std::string sequence_vector_to_ACGT(const std::vector<char>& sequence_vector);


} // namespace helpers


#endif