#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include "fileio.hpp"
#include "conversion.hpp"
#include "constants.hpp"
#include "logging.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include "fmt/ostream.h"

static Logger logger("fileio", "INFO");

namespace fileio {

    // encapsulates the logic for reading sequences from a file
    SequenceFileReader::SequenceFileReader(const string& filename, ReadFileType filetype) {
        if (filetype == ReadFileType::BINARY) {
            _file.open(filename, std::ios::binary);
        } else {
            _file.open(filename);
        }
        if (!_file.is_open()) {
            logger.critical("Could not open file: " + filename);
            throw std::runtime_error("Could not open file: " + filename);
        }
        this->filename = filename;
        this->filetype = filetype;
    }

    SequenceFileReader::~SequenceFileReader() {
        close();
    }

    void SequenceFileReader::remove() {
        close();
        if (std::remove(filename.c_str()) != 0) {
            logger.critical("Could not remove file: " + filename);
        }
    }

    void SequenceFileReader::close() {
        if (_file.is_open()) {
            _file.close();
        }
    }

    // read a line from the file and store it in the sequence vector, vector overload
    bool SequenceFileReader::_getline(std::vector<char>& sequence) {
        sequence.clear();
        if (filetype != ReadFileType::BINARY) {
            logger.critical("Cannot read binary data from a non-binary file");
            throw std::runtime_error("Cannot read binary data from a non-binary file");
        }
        // read characters from the file until we reach a newline
        char c;
        while (_file.get(c)) {
            if (c == constants::NUCLEOTIDE_NEXTOLIGO) {
                return true;
            }
            sequence.push_back(c);
        }
        return !sequence.empty();
    }

    // read a line from the file and store it in the sequence string, txt overload
    bool SequenceFileReader::_getline(std::string& sequence) {
        if (filetype != ReadFileType::ANY) {
            logger.critical("Cannot read text data from a non-text file");
            throw std::runtime_error("Cannot read text data from a non-text file");
        }
        if (getline(_file, sequence)) {
            return true;
        } else {
            return false;
        }
    }

    // move the file pointer to the start of the file
    void SequenceFileReader::to_start() {
        _file.clear();
        _file.seekg(0);
        skipped_lines = 0;
        valid_sequences = 0;
    }

    // check if the sequence is valid, string overload
    bool SequenceFileReader::check_valid_sequence(const std::string& sequence) {
        // skip empty lines
        if (sequence.empty()) {
            skipped_lines++;
            return false;
        }
        // skip lines starting with '>'
        if (sequence[0] == '>' || sequence[0] == '@' || sequence[0] == '+') {
            skipped_lines++;
            return false;
        }
        // skip sequences that contain chars other than A, C, G and T
        for (char c : sequence) {
            if (c != 'A' && c != 'C' && c != 'G' && c != 'T') {
                skipped_lines++;
                return false;
            }
        }
        valid_sequences++;
        return true;
    }

    // check if the sequence vector is valid, vector overload
    bool SequenceFileReader::check_valid_sequence(std::vector<char>const& sequence_vector) {
        // skip empty sequences
        if (sequence_vector.empty()) {
            skipped_lines++;
            return false;
        }
        // skip sequences that contain chars other than A, C, G and T
        for (char c : sequence_vector) {
            if (c != constants::NUCLEOTIDE_A && c != constants::NUCLEOTIDE_C && c != constants::NUCLEOTIDE_G && c != constants::NUCLEOTIDE_T) {
                skipped_lines++;
                logger.warning("Skipping sequence with invalid nucleotide: {}", (int)c);
                return false;
            }
        }
        valid_sequences++;
        return true;
    }

    // read the next valid sequence from the file
    bool SequenceFileReader::get_sequence(std::vector<char>& sequence_vector) {
        if (filetype == ReadFileType::BINARY) {
            while (_getline(sequence_vector)) {
                if (check_valid_sequence(sequence_vector)) {
                    return true;
                }
            }
            return false;
        } else {
            while (_getline(_current_sequence)) {
                if (check_valid_sequence(_current_sequence)) {
                    conversion::sequence_to_vector(_current_sequence, sequence_vector);
                    return true;
                }
            }
            return false;
        }
    }

    // function that reads a file and returns the number of lines in it
    int SequenceFileReader::count_sequences() {
        to_start();
        std::vector<char> sequence;
        int count = 0;
        while (get_sequence(sequence)) {
            count++;
        }
        logger.debug("Read {} valid sequences ({} lines valid, {} lines skipped) from file {}", count, valid_sequences, skipped_lines, filename);
        to_start();
        return count;
    }



    // encapsulates the logic for writing sequences to a file
    SequenceFileWriter::SequenceFileWriter(const string& filename, WriteFileType filetype) {
        if (filetype == WriteFileType::BINARY) {
            _file.open(filename, std::ios::binary);
        } else {
            _file.open(filename);
        }
        if (!_file.is_open()) {
            logger.critical("Could not open file: " + filename);
            throw std::runtime_error("Could not open file: " + filename);
        }
        this->filename = filename;
        this->filetype = filetype;
    }

    SequenceFileWriter::~SequenceFileWriter() {
        close();
    }

    void SequenceFileWriter::remove() {
        close();
        if (std::remove(filename.c_str()) != 0) {
            logger.critical("Could not remove file: " + filename);
        }
    }

    void SequenceFileWriter::close() {
        if (_file.is_open()) {
            _file.close();
        }
    }

    // write a single sequence for binary output
    void SequenceFileWriter::_write_sequence_as_binary(const std::vector<char>& sequence_vector) {
        if (filetype != WriteFileType::BINARY) {
            logger.critical("Cannot write binary data to a non-binary file");
            throw std::runtime_error("Cannot write binary data to a non-binary file");
        }
        // write the vector of chars to the file
        _file.write((char*)&sequence_vector[0], sequence_vector.size() * sizeof(char));
        _file.put(constants::NUCLEOTIDE_NEXTOLIGO);
        sequences_written++;
    }

    // write a single sequence for txt output
    void SequenceFileWriter::_write_sequence_as_txt(const std::string& sequence) {
        fmt::print(_file, "{}\n", sequence);
        sequences_written++;
    }

    // write a single sequence for fasta output
    void SequenceFileWriter::_write_sequence_as_fasta(const std::string& sequence) {
        fmt::print(_file, ">Oligo_{:09}\n{}\n", sequences_written, sequence);
        sequences_written++;
    }

    // write a single sequence for fastq output
    void SequenceFileWriter::_write_sequence_as_fastq(const std::string& sequence) {
        fmt::print(_file, "@Oligo_{:09}\n{}\n+\n{}\n", sequences_written, sequence, std::string(sequence.size(), 'F'));
        sequences_written++;
    }

    // write a sequence vector to the file
    void SequenceFileWriter::write_sequence_vector(const std::vector<char>& sequence_vector) {
        if (filetype == WriteFileType::BINARY) {
            _write_sequence_as_binary(sequence_vector);
        } else {
            std::string sequence = conversion::vector_to_sequence(sequence_vector);
            switch (filetype) {
                case WriteFileType::TXT:
                    _write_sequence_as_txt(sequence);
                    break;
                case WriteFileType::FASTA:
                    _write_sequence_as_fasta(sequence);
                    break;
                case WriteFileType::FASTQ:
                    _write_sequence_as_fastq(sequence);
                    break;
            }
        }
    }


} // namespace fileio
