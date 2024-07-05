#ifndef OLIGOCOLLECTOR_HPP
#define OLIGOCOLLECTOR_HPP

#include <vector>
#include <memory>

#include "fileio.hpp"
#include "mutator.hpp"


namespace oligocollector {

    // 
    class OligoCollector {
        private:
            bool _create_rv;
            std::unique_ptr<std::vector<std::unique_ptr<mutator::BaseMutator>>> _mutators;

        public:
            std::unique_ptr<fileio::SequenceFileWriter> filewriter_fw;
            std::unique_ptr<fileio::SequenceFileWriter> filewriter_rv;

            OligoCollector(fileio::SequenceFileWriter& filewriter_fw);
            OligoCollector(fileio::SequenceFileWriter& filewriter_fw, fileio::SequenceFileWriter& filewriter_rv);

            ~OligoCollector();

            // set up mutators
            void set_mutators(std::vector<std::unique_ptr<mutator::BaseMutator>>& mutators);

            // apply mutators
            std::vector<char> apply_mutators(const std::vector<char>& sequence_vector);

            // collect a sequence vector for writing
            void collect_sequence_vector(const std::vector<char>& sequence_vector);
    };
    
}

#endif // OLIGOCOLLECTOR_HPP