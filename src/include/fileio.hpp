#ifndef FILEIO_HPP
#define FILEIO_HPP

#include <string>
#include <vector>
#include <fstream>


namespace fileio {

    // enum to store the supported file types
    enum class ReadFileType {
        BINARY,
        ANY,
    };

    // class to encapsulate sequence reading
    class SequenceFileReader {
        private:
            std::ifstream _file;
            std::string _current_sequence;

            bool _getline(std::string& sequence);
            bool _getline(std::vector<char>& sequence);


        public:
            std::string filename;
            ReadFileType filetype;
            int skipped_lines = 0;
            int valid_sequences = 0;

            SequenceFileReader(const std::string& filename, ReadFileType filetype = ReadFileType::ANY);

            ~SequenceFileReader();

            void remove();

            void close();

            // move the file pointer to the start of the file
            void to_start();

            // check if the sequence is valid
            bool check_valid_sequence(const std::string& sequence);
            bool check_valid_sequence(std::vector<char>const& sequence_vector);

            // read the next valid sequence from the file as sequence vector
            bool get_sequence(std::vector<char>& sequence_vector);

            // function to count the number of sequences in the file
            int count_sequences();
    };


    // enum to store the supported file types
    enum class WriteFileType {
        BINARY,
        TXT,
        FASTA,
        FASTQ
    };

    // class to encapsulate sequence writing
    class SequenceFileWriter {
        private:
            std::ofstream _file;

            // write a single sequence for binary output
            void _write_sequence_as_binary(const std::vector<char>& sequence_vector);

            // write a single sequence for txt output
            void _write_sequence_as_txt(const std::string& sequence);

            // write a single sequence for fasta output
            void _write_sequence_as_fasta(const std::string& sequence);

            // write a single sequence for fastq output
            void _write_sequence_as_fastq(const std::string& sequence);

        public:
            std::string filename;
            WriteFileType filetype;
            int sequences_written = 0;

            SequenceFileWriter(const std::string& filename, WriteFileType filetype = WriteFileType::TXT);

            ~SequenceFileWriter();

            void remove();

            void close();

            // write a sequence vector to the file
            void write_sequence_vector(const std::vector<char>& sequence_vector);
    };
    
    


}

#endif