#include <vector>
#include <memory>

#include "oligocollector.hpp"
#include "fileio.hpp"
#include "conversion.hpp"
#include "mutator.hpp"
#include "logging.hpp"

Logger logger("oligocollector", "INFO");


namespace oligocollector {

    
    OligoCollector::OligoCollector(fileio::SequenceFileWriter& filewriter_fw) {
        this->filewriter_fw.reset(&filewriter_fw);
        _create_rv = false;
    }
    
    OligoCollector::OligoCollector(fileio::SequenceFileWriter& filewriter_fw, fileio::SequenceFileWriter& filewriter_rv) {
        this->filewriter_fw.reset(&filewriter_fw);
        this->filewriter_rv.reset(&filewriter_rv);
        _create_rv = true;
    }

    OligoCollector::~OligoCollector() {
        // clear all pointers
        filewriter_fw.release();
        filewriter_rv.release();
        _mutators.release();
    }

    // set up mutators
    void OligoCollector::set_mutators(std::vector<std::unique_ptr<mutator::BaseMutator>>& mutators) {
        _mutators.reset(&mutators);
    }


    // collect a sequence vector for writing
    std::vector<char> OligoCollector::apply_mutators(const std::vector<char>& sequence_vector) {
        // create vector with the sequence
        std::vector<std::vector<char>> mutated_sequences;
        mutated_sequences.push_back(sequence_vector);

        // apply mutators if there are any
        if (_mutators != nullptr) {
            for (std::unique_ptr<mutator::BaseMutator>& mutator : *_mutators) {
                mutator->process(mutated_sequences);
            }
        }
        return mutated_sequences[0];
    }
        

    // collect a sequence vector for writing
    void OligoCollector::collect_sequence_vector(const std::vector<char>& sequence_vector) {
        // write the forwards sequence
        std::vector<char> mutated_sequence_vector = apply_mutators(sequence_vector);
        filewriter_fw->write_sequence_vector(mutated_sequence_vector);
        if (_create_rv) {
            std::vector<char> rv_sequence_vector = conversion::reverse_complement(sequence_vector);
            filewriter_rv->write_sequence_vector(apply_mutators(rv_sequence_vector));
        }
    }

}