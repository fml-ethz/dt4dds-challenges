#include <vector>
#include <numeric>
#include <memory>
#include <time.h>

#include "include/argparse.hpp"
#include "include/constants.hpp"
#include "include/fileio.hpp"
#include "include/mutator.hpp"
#include "include/rng.hpp"
#include "include/pipeline.hpp"
#include "include/scenarios.hpp"
#include "include/logging.hpp"

static Logger logger("run", "INFO");


int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("dt4dds-challenges");

    // track time
    time_t start,end;
    time(&start);


    //
    // required arguments
    //
    std::string challenge_name;
    program.add_argument("challenge")
    .required()
    .help("name of the challenge to run (decay, photolithography)")
    .choices("decay", "photolithography")
    .store_into(challenge_name);

    std::string input_filename;
    program.add_argument("input_file")
    .required()
    .help("path to the input file (as text, fasta, or fastq)")
    .store_into(input_filename);

    std::string output_filename_fw;
    program.add_argument("output_file_R1")
    .required()
    .help("path to the output file for sequencing read 1")
    .store_into(output_filename_fw);

    std::string output_filename_rv;
    program.add_argument("output_file_R2")
    .required()
    .help("path to the output file for sequencing read 2")
    .store_into(output_filename_rv);


    //
    // optional arguments
    //
    program.add_argument("--strict")
    .help("enforce the default settings of the challenge")
    .flag();

    program.add_argument("-i", "--intermediate_file")
    .help("path to the intermediate file, default will create temporary file");

    program.add_argument("-f", "--format")
    .help("format of the output file (txt, fasta, fastq)")
    .choices("txt", "fasta", "fastq")
    .default_value("txt");
    
    program.add_argument("-b", "--coverage_bias")
    .help("coverage bias during synthesis, expressed as standard deviation of the lognormal distribution")
    .scan<'g', float>();

    program.add_argument("-p", "--physical_redundancy")
    .help("mean physical coverage of the pool, expressed in oligos per design sequence")
    .scan<'g', float>();

    program.add_argument("-s", "--sequencing_depth")
    .help("mean sequencing coverage of the pool, expressed in reads per design sequence")
    .scan<'g', float>();

    program.add_argument("-l", "--read_length")
    .help("length of the sequencing reads, expressed in nucleotides")
    .scan<'d', int>();

    program.add_argument("--seed")
    .help("seed for the random number generator, default is to use the current time")
    .scan<'d', int>();

    program.add_argument("--no_adapter")
    .help("disable the adapter sequences in the output files")
    .flag();

    program.add_argument("--no_padtrim")
    .help("disable the padding and trimming of the sequences in the output files")
    .flag();


    // parse the command line arguments
    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    // initialise the main parameters
    std::vector<std::unique_ptr<mutator::BaseMutator>> initial_mutators;
    std::vector<std::unique_ptr<mutator::BaseMutator>> recovery_mutators;
    std::vector<std::unique_ptr<mutator::BaseMutator>> sequencing_mutators;
    float initial_coverage_bias;
    float mean_physical_coverage;
    float mean_sequencing_coverage;
    int read_length;
    bool add_adapters = true;
    bool pad_and_trim = true;

    // seed the random number generator with current time
    rng::seed_rng(time(NULL));
    
    // get the parameters from the challenge
    if (challenge_name == "decay") {
        scenarios::challenge_decay(
            initial_coverage_bias,
            mean_physical_coverage,
            mean_sequencing_coverage,
            read_length,
            initial_mutators,
            recovery_mutators
        );
    }
    else if (challenge_name == "photolithography") {
        scenarios::challenge_photolithography(
            initial_coverage_bias,
            mean_physical_coverage,
            mean_sequencing_coverage,
            read_length,
            initial_mutators,
            recovery_mutators
        );
    }

    // change the parameters if not running in strict mode
    if (program["--strict"] == false) {
        logger.warning("Not running in strict mode, settings can be overridden from challenge definition.");
    
        if (auto fn = program.present<float>("--coverage_bias")) {
            initial_coverage_bias = *fn;
            logger.warning("Coverage bias changed from default to {}", initial_coverage_bias);
        }
        if (auto fn = program.present<float>("--physical_redundancy")) {
            mean_physical_coverage = *fn;
            logger.warning("Physical coverage changed from default to {}", mean_physical_coverage);
        }
        if (auto fn = program.present<float>("--sequencing_depth")) {
            mean_sequencing_coverage = *fn;
            logger.warning("Sequencing coverage changed from default to {}", mean_sequencing_coverage);
        }
        if (auto fn = program.present<int>("--read_length")) {
            read_length = *fn;
            logger.warning("Read length changed from default to {}", read_length);
        }
        if (auto fn = program.present<int>("--seed")) {
            rng::seed_rng(*fn);
            logger.warning("Used custom seed {}", *fn);
        }
        if (program["--no_adapter"] == true) {
            add_adapters = false;
            logger.warning("Adapters will not be added to the output files");
        }
        if (program["--no_padtrim"] == true) {
            pad_and_trim = false;
            logger.warning("Padding and trimming will not be applied to the output files");
        }
    } else {
        logger.warning("Running in strict mode, challenge definition will be enforced and other parameters ignored.");
    }

    // get the parameters for sequencing
    scenarios::sequencing(
        add_adapters,
        pad_and_trim,
        read_length,
        sequencing_mutators
    );

    // get the output file's handle and format
    fileio::WriteFileType write_file_type = fileio::WriteFileType::TXT;
    if (program.get<std::string>("--format") == "txt") {
        write_file_type = fileio::WriteFileType::TXT;
    }
    else if (program.get<std::string>("--format") == "fasta") {
        write_file_type = fileio::WriteFileType::FASTA;
    }
    else if (program.get<std::string>("--format") == "fastq") {
        write_file_type = fileio::WriteFileType::FASTQ;
    }

    // get the intermediate file's handle
    std::string intermediate_filename = std::tmpnam(nullptr);
    if (auto fn = program.present("--intermediate_file")) {
        intermediate_filename = program.get<std::string>("--intermediate_file");
        logger.info("Using intermediate file at {}", intermediate_filename);
    }

    // run the pipeline
    pipeline::run(
        input_filename,
        intermediate_filename,
        output_filename_fw,
        output_filename_rv,
        initial_coverage_bias,
        mean_physical_coverage,
        mean_sequencing_coverage,
        initial_mutators,
        recovery_mutators,
        sequencing_mutators,
        write_file_type
    );

    // log the end of the process and the duration it took
    time(&end);
    logger.info("Finished in {} seconds", difftime(end, start));
    return 0;
}

