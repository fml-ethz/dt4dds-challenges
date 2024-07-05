#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <vector>
#include <memory>

#include "fileio.hpp"
#include "oligocollector.hpp"
#include "mutator.hpp"


namespace pipeline {

    void process(
        fileio::SequenceFileReader& reader, 
        oligocollector::OligoCollector& collector,
        std::vector<unsigned int> const& oligo_counts,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& mutators
    );

    void synthesis_and_sampling(
        fileio::SequenceFileReader& reader, 
        fileio::SequenceFileWriter& writer,
        float initial_coverage_bias,
        float mean_physical_coverage,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& mutators
    );

    void recovery_and_sequencing(
        fileio::SequenceFileReader& reader, 
        fileio::SequenceFileWriter& writer_fw,
        fileio::SequenceFileWriter& writer_rv,
        int n_sequences,
        float mean_sequencing_coverage,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& mutators
    );

    void run(
        std::string input_filename,
        std::string intermediate_filename,
        std::string output_filename_fw,
        std::string output_filename_rv,
        float initial_coverage_bias,
        float mean_physical_coverage,
        float mean_sequencing_coverage,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& initial_mutators,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& recovery_mutators,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& sequencing_mutators,
        fileio::WriteFileType write_file_type
    );

} // namespace pipeline


#endif // PIPELINE_HPP