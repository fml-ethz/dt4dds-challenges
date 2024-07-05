#include <vector>
#include <numeric>
#include <memory>
#include <time.h>

#include "pipeline.hpp"
#include "constants.hpp"
#include "fileio.hpp"
#include "coverage.hpp"
#include "helpers.hpp"
#include "oligofactory.hpp"
#include "oligocollector.hpp"
#include "mutator.hpp"
#include "progressbar.hpp"
#include "logging.hpp"

static Logger logger("pipeline", "INFO");


namespace pipeline {

    void process(
        fileio::SequenceFileReader& reader, 
        oligocollector::OligoCollector& collector,
        std::vector<unsigned int> const& oligo_counts,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& mutators
        ) {

        // these vectors will hold the sequence and the oligos generated for it
        std::vector<char> sequence_vector;
        sequence_vector.reserve(constants::DEFAULT_SEQUENCE_LENGTH);
        std::vector<std::vector<char>> oligos = std::vector<std::vector<char>>();

        // create a progress bar and log the start of the process
        logger.info("Generating {} oligos from {} sequences", std::accumulate(oligo_counts.begin(), oligo_counts.end(), 0), oligo_counts.size());
        progressbar::ProgressBar progress_bar(oligo_counts.size(), "Generating oligos");
        time_t start,end;
        time(&start);

        // iterate over the sequences in the input file
        int i_seq = 0;
        while (reader.get_sequence(sequence_vector)) {

            // short-circuit if there are no oligos to generate for this sequence
            if (oligo_counts[i_seq] == 0) {
                i_seq++;
                progress_bar.update(i_seq);
                continue;
            }

            // clear the oligos vector and ensure sufficient space in it
            oligos.clear();
            oligos.reserve(oligo_counts[i_seq]);

            // generate the oligos for the current sequence
            oligofactory::generate_oligos(oligos, sequence_vector, oligo_counts[i_seq], mutators);

            // write the oligos to the output file
            for (std::vector<char>& oligo : oligos) {
                collector.collect_sequence_vector(oligo);
            }

            // we are done with the current sequence
            i_seq++;
            progress_bar.update(i_seq);
        }
        progress_bar.close();

        // check that we have processed all sequences
        if (i_seq != oligo_counts.size()) {
            logger.critical("Processed {} sequences, but expected {}", i_seq, oligo_counts.size());
            throw std::runtime_error("Processed " + std::to_string(i_seq) + " sequences, but expected " + std::to_string(oligo_counts.size()));
        }

        // log the end of the process and the duration it took
        time(&end);
        logger.info("Finished generating {} oligos from {} sequences in {} seconds", std::accumulate(oligo_counts.begin(), oligo_counts.end(), 0), i_seq, difftime(end, start));
    }


    void synthesis_and_sampling(
        fileio::SequenceFileReader& reader, 
        fileio::SequenceFileWriter& writer,
        float initial_coverage_bias,
        float mean_physical_coverage,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& mutators
        ) {

        // get the number of design sequences in the input file
        int n_seqs = reader.count_sequences();

        // get the initial coverage for the sequences based on the coverage bias
        logger.info("Generating synthesis coverage with bias {}", initial_coverage_bias);
        std::vector<unsigned int> initial_sequence_coverage = coverage::get_initial_coverage(n_seqs, initial_coverage_bias);

        // sample the initial coverage to get the actual physical oligo coverage
        int n_sampled_oligos = (int) n_seqs * mean_physical_coverage;
        logger.info("Sampling for a mean physical coverage of {}", mean_physical_coverage);
        std::vector<unsigned int> physical_coverage = coverage::sample_by_count(initial_sequence_coverage, n_sampled_oligos);

        // process the sequences and write them to the output file
        logger.info("Processing errors for synthesis and sampling");
        oligocollector::OligoCollector collector(writer);
        process(reader, collector, physical_coverage, mutators);
        logger.info("Finished synthesis and sampling");
    }


    void recovery_and_sequencing(
        fileio::SequenceFileReader& reader, 
        fileio::SequenceFileWriter& writer_fw,
        fileio::SequenceFileWriter& writer_rv,
        int n_sequences,
        float mean_sequencing_coverage,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& mutators,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& sequencing_mutators
        ) {

        // get the number of oligo sequences in the input file
        int n_seqs = reader.count_sequences();

        // sample the oligos uniformly to get the actual sequencing reads
        int n_reads = (int) mean_sequencing_coverage * n_sequences;
        logger.info("Sampling for a mean sequencing coverage of {}", mean_sequencing_coverage);
        std::vector<unsigned int> sequence_coverages(n_seqs, 1); // all oligos appear once
        std::vector<unsigned int> sequencing_coverage = coverage::sample_by_count(sequence_coverages, n_reads);

        // generate a sequencing file handler to take care of the paired-end reads
        oligocollector::OligoCollector collector(writer_fw, writer_rv);
        collector.set_mutators(sequencing_mutators);

        // process the oligos and write them to the output file
        logger.info("Processing errors for recovery and sequencing");
        process(reader, collector, sequencing_coverage, mutators);
        logger.info("Finished recovery and sequencing");
    }


        
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
    ) {

        // open the input and output files
        fileio::SequenceFileReader input_reader(input_filename);
        fileio::SequenceFileWriter intermediate_writer(intermediate_filename, fileio::WriteFileType::BINARY);

        // get the number of design sequences
        int n_sequences = input_reader.count_sequences();

        // run the synthesis and sampling process
        try {
            pipeline::synthesis_and_sampling(input_reader, intermediate_writer, initial_coverage_bias, mean_physical_coverage, initial_mutators);
        } catch (std::exception& e) {
            logger.critical("An error occurred during synthesis and sampling: {}", e.what());
            intermediate_writer.remove();
            throw;
        }

        // ensure files are closed
        input_reader.close();
        intermediate_writer.close();

        // open the intermediate file and the output files for the forward and reverse reads
        fileio::SequenceFileReader intermediate_reader(intermediate_filename, fileio::ReadFileType::BINARY);
        fileio::SequenceFileWriter writer_fw(output_filename_fw, write_file_type);
        fileio::SequenceFileWriter writer_rv(output_filename_rv, write_file_type);

        // run the recovery and sequencing process
        try {
            pipeline::recovery_and_sequencing(intermediate_reader, writer_fw, writer_rv, n_sequences, mean_sequencing_coverage, recovery_mutators, sequencing_mutators);
        } catch (std::exception& e) {
            logger.critical("An error occurred during recovery and sequencing: {}", e.what());
            intermediate_reader.remove();
            throw;
        }

        // ensure files are closed and intermediate file is deleted
        intermediate_reader.close();
        writer_fw.close();
        writer_rv.close();
        intermediate_reader.remove();
    }


} // namespace pipeline