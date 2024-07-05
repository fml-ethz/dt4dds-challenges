#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace constants {
    inline constexpr char NUCLEOTIDE_A = 1; // integer representation of nucleotide A
    inline constexpr char NUCLEOTIDE_C = 2; // integer representation of nucleotide C
    inline constexpr char NUCLEOTIDE_G = 3; // integer representation of nucleotide G
    inline constexpr char NUCLEOTIDE_T = 4; // integer representation of nucleotide T
    inline constexpr char NUCLEOTIDE_NEXTOLIGO = 127; // flag to indicate the next oligo in a binary sequence file
    
    inline constexpr int DEFAULT_SEQUENCE_LENGTH { 500 }; // default length of a sequence for vector allocation
}

#endif // CONSTANTS_HPP