# 🧬🏆 dt4dds-challenges - Challenges for DNA Data Storage

- [Overview](#overview)
- [Challenge Definitions](#challenge-definitions)
- [System Requirements](#system-requirements)
- [Installation Guide](#installation-guide)
- [Usage Guide](#usage-guide)
- [License](#license)

# Overview
`dt4dds-challenges` is a digital representation of two current challenges in DNA Data Storage: Photolithographic DNA Synthesis and DNA Decay after Long-Term Storage. Implemented in C++, it processes the user-provided DNA sequences of an encoder to generate sequencing reads with accurate error and bias patterns for the decoder. As such, `dt4dds-challenges` represents a benchmark to test and compare error-correction codes in these two crucial, but challenging scenarios for DNA Data Storage. More information can be found in the following publications:

> Gimpel, A.L., Stark, W.J., Heckel, R., Grass R.N. Challenges for error-correction coding in DNA data storage: photolithographic synthesis and DNA decay. bioRxiv 2024.07.04.602085 (2024). https://doi.org/10.1101/2024.07.04.602085

> Gimpel, A.L., Stark, W.J., Heckel, R., Grass R.N. A digital twin for DNA data storage based on comprehensive quantification of errors and biases. Nat Commun 14, 6026 (2023). https://doi.org/10.1038/s41467-023-41729-1


The Jupyter notebooks and associated code used for generating the figures in the manuscript are found in the [dt4dds-challenges_notebooks repository](https://github.com/fml-ethz/dt4dds-challenges_notebooks).


## Leaderboard and web-based usage
A web-based version of `dt4dds-challenges` with an easy-to-use graphical user interface and the current leaderboard is available at [dt4dds.ethz.ch](https://dt4dds.ethz.ch).



# Challenge definitions
In the following subsections, the simulated error and bias patterns are explained. To enforce these settings during a run, use the `--strict` flag:
 ```shell
dt4dds-challenges <photolithography/decay> <input_file> <output_R1> <output_R2> --strict
```
You may still alter the optional `--format` and `--intermediate_file` arguments in `--strict` mode.

## Challenge `photolithography`
This challenge definition corresponds to Challenge 1: Photolithographic DNA Synthesis in the manuscript. It emulates the error patterns occurring during photolithographic synthesis and the application in a DNA-of-things storage architecture. As such, the main challenge lies in effectively utilizing the high physical coverage and sequencing depth to decrease the excessive error rates to reasonable levels (e.g. by clustering and merging).

| Parameter | Value |
| ------------- | -------------: |
| Substitution rate | 0.025 nt<sup>-1</sup> |
| Deletion rate | 0.075 nt<sup>-1</sup> |
| Insertion rate | 0.012 nt<sup>-1</sup> |
| Coverage bias | 0.44 |
| Physical redundancy | 200x |
| Sequencing depth | 50x |

In addition, the errors are biased to occur consecutively, and both the 5'- and 3'-ends of oligos are shortened, in-line with experimental observations from photolithographic DNA synthesis. For more information on the error patterns and biases, please refer to [the manuscript and the challenge homepage](https://dt4dds.ethz.ch/).


## Challenge `decay`
This challenge definition corresponds to Challenge 2: DNA Decay after Long-Term Storage in the manuscript. It emulates the error patterns occurring during high-fidelity synthesis and the long-term storage for around five half-lives. Contrary to usual workflows, the oligo fragments remaining after decay are also recovered and present in the sequencing data. As such, the main challenge lies in effectively utilizing these oligo fragments to reconstruct the data (e.g. by strand reconstruction or partial decoding), as only very few intact sequences remain.

| Parameter | Value |
| ------------- | -------------: |
| Substitution rate | 0.0049 nt<sup>-1</sup> |
| Deletion rate | 0.0007 nt<sup>-1</sup> |
| Insertion rate | negligible |
| Breakage rate | 0.023 nt<sup>-1</sup> |
| Coverage bias | 0.30 |
| Physical redundancy | 10x |
| Sequencing depth | 30x |

In addition, breakage occurs predominantly at G or A, and short oligo fragments are less likely to be recovered, in-line with experimental observations from decay experiments. Moreover, reads are 3'-tailed with a variable number of C and T nucleotides, due to the oligo recovery workflow. For more information on the error patterns and biases, please refer to [the manuscript and the challenge homepage](https://dt4dds.ethz.ch/).



# System requirements
## Hardware requirements
This program only requires a standard computer, with no special requirements on RAM or core count. The run time is mostly influenced by the write speed of the disk that is written to. Therefore, usage of a SSD is recommended.

## Software requirements
The pre-built binaries can be run on Linux or Windows without any further requirements. To build this project locally, GCC≥10.1 (i.e. with support for C++20) is required. The use of Docker as a build/run environment is optional.


# Installation guide
There are three options for installing and using `dt4dds-challenges`: using the pre-built binaries, running the Docker container, or compiling the source code locally.


## Pre-built binaries
Statically compiled, pre-built binaries for Linux and Windows are provided in the [bin](/bin) directory and the [release page](https://github.com/fml-ethz/dt4dds-challenges/releases). To run them, copy the appropriate binary to a directory and run the following command in a shell or the command line:
```shell 
./dt4dds-challenges         % Linux
./dt4dds-challenges.exe     % Windows
```


## Running a Docker container
A [Dockerfile](/Dockerfile) is provided in the repository. To build the container, run:
```shell 
docker build . -t dt4dds-challenges
```
To get shell-access to the Docker container, run the following from the main directory:
```shell 
docker run --mount type=bind,source=./files,target=/dt4dds-challenge/files --rm -it dt4dds-challenge sh
```
This will also link the local subfolder `./files` of your directory to the subfolder `./files` in the Docker container. You can then use the program from within the Docker container as described in the section [Usage Guide](#usage-guide), and exchange files between the container and your host machine via the `./files` subfolder.

Alternatively, you can issue full commands directly, e.g.:
```shell
docker run --mount type=bind,source=./files,target=/dt4dds-challenges/files --rm -it dt4dds-challenges <decay/photolithography> files/<input_file> files/<R1_file> files/<R2_file> -f <txt/fasta/fastq>
```
to run a simulation on the specified input file (see [Usage Guide](#usage-guide)), or
```shell
docker run --mount type=bind,source=./files,target=/dt4dds-challenges/files --rm -it dt4dds-challenges scripts/run_decay.sh
```
to run one of the supplied scripts (see [Usage Guide](#usage-guide)).



## Compiling locally
Given a recent version of GCC is installed, the Linux and Windows binaries can be built locally  using
```shell
make linux
make windows
```
from the main directory. The binaries will be placed in the [/bin](/bin/) subdirectory and can be used like the pre-built binaries described above.

Further tools that can be used to post-process the simulated sequencing data can be downloaded and installed using
```shell
make tools
```
For further information, please see the [Usage Guide](#usage-guide).



# Usage guide

The `dt4dds-challenges` program is used as follows:
```shell
dt4dds-challenges challenge input_file output_file_R1 output_file_R2 [--strict] [--intermediate_file VAR] [--format VAR] [--coverage_bias VAR] [--physical_redundancy VAR] [--sequencing_depth VAR] [--read_length VAR] [--seed VAR] [--no_adapter] [--no_padtrim] 
```

As an example, to run the photolithography challenge on the input file `./files/input_sequences.txt`, writing the sequencing data as FASTQ files to `./files/R1.fq` and `./files/R2.fq`:
```shell
dt4dds-challenges photolithography ./files/input_sequences.txt ./files/R1.fq ./files/R2.fq -f fastq
```


The arguments are briefly described in the following table, with more information in the subsections below.

| Argument | Description |
| ------------- | ------------- |
| challenge | name of the challenge to run (decay, photolithography) |
| input_file | path to the input file (text, fasta, or fastq) |
| output_file_R1 | path to the output file for sequencing read 1 |
| output_file_R2 | path to the output file for sequencing read 2 |
| --strict | enforce the default settings of the challenge |
| -f, --format | format of the output file (txt, fasta, fastq), default is txt |
| -b, --coverage_bias | coverage bias during synthesis, expressed as standard deviation of the lognormal distribution, default is set by challenge |
| -p, --physical_redundancy | mean physical coverage of the pool, expressed in oligos per design sequence, default is set by challenge |
| -s, --sequencing_depth | mean sequencing coverage of the pool, expressed in reads per design sequence, default is set by challenge |
| -l, --read_length | length of the sequencing reads, expressed in nucleotides, default is 150 nt |
| -i, --intermediate_file | path to the intermediate file, default will create temporary file |
| --seed | seed for the random number generator, default is to use the current time |
| --no_adapter | disable the adapter sequences in the output files |
| --no_padtrim | disable the padding and trimming of the sequences in the output files |


## Required arguments

### challenge
Can bei either `photolithography` to run the challenge on Photolithographic DNA Synthesis, or `decay` to run the challenge on DNA Decay. For full definitions of these challenges, see the section [Challenge Definitions](#challenge-definitions) and the manuscript. Besides error patterns and biases, this will also set the default physical coverage and sequencing depth.

### input_file
Relative or full path to the input file. Sequences in txt, fasta, and fastq format are supported. Make sure each sequence covers only one line (i.e. no line breaks in a sequence).

### output_file_R1 and output_file_R2
Relative or full path to the output files for read 1 and read 2. These files will be created by the program, in the format specified by the optional argument `--format`. These files should be used to attempt decoding. Additional tools can be installed from the convenience scripts in the [tools subfolder](/tools/) to help with merging and post-processing the sequencing reads (see below).


## Optional arguments

### --strict
This flag will prevent any changes to the simulation parameters by the other optional arguments (except for `--format` and `--intermediate_file`). As a result, setting this flag will guarantee that the challenge is run with the settings as defined in the section [Challenge Definitions](#challenge-definitions).

### --format [txt/fast/fastq]
By default, the reads will be written to the output files for read 1 and read 2 in the txt format (i.e., one read per line). Setting this argument to `fasta` or `fastq` will change the output to the FASTA or FASTQ format, respectively. This can be helpful if post-processing steps require specific file formats.

### --coverage_bias [float]
By default, the initial homogeneity of the number of oligos per design sequence - defined by the standard deviation of the lognormal fit to the coverage distribution - is defined by the selected challenge. This argument overrides the default of the challenge which can be useful to test decoding performance under more/less ideal coverage conditions.

### --physical_redundancy [float]
By default, the mean number of oligos per design sequence during storage/sampling (i.e., the physical redundancy) is defined by the selected challenge. This argument overrides the default of the challenge which can be useful to test decoding performance at lower/higher physical redundancy.

### --sequencing_depth [float]
By default, the mean number of reads per design sequence during sequencing (i.e., the sequencing depth) is defined by the selected challenge. This argument overrides the default of the challenge which can be useful to test decoding performance at lower/higher sequencing depth.

### --read_length [int]
By default, the length of the reads during sequencing is 150 nt. This argument overrides this default. Note that this is generally fixed by the sequencing platform used.

### --intermediate_file [path]
The program will create an intermediate file during processing. By default, a temporary folder provided by the OS is used and deleted after the program finishes. A custom path where the intermediate file should be placed can be supplied for debugging or performance reasons.

### --seed [int]
This will fix the initial seed for the simulation for reproducible results.

### --no_adapter
By default, the sequencing reads will include the sequencing adapter if the read length exceeds the sequence length. Setting this flag will prevent the addition of the adapter to the sequencing reads for debugging or troubleshooting purposes. Note that this flag is not available in the real challenges (i.e., as set by `--strict`), so do not depend on it for decoding. Instead, post-processing the reads with a read merger (e.g., [ngmerge](/tools/ngmerge/)) will remove the sequencing adapters.

### --no_padtrim
By default, the sequencing reads will be padded (with random nucleotides) or trimmed to the defined read length. Setting this flag will prevent the padding and trimming of the sequencing reads for debugging or troubleshooting purposes. Note that this flag is not available in the real challenges (i.e., as set by `--strict`), so do not depend on it for decoding. Instead, post-processing the reads with a read merger (e.g., [ngmerge](/tools/ngmerge/)) will also remove the padded bases.


## Convenience scripts and tools
The [tools subfolder](/tools/) contains scripts to install two external tools: [ngmerge](https://github.com/jsh58/NGmerge) and [bbmap](https://sourceforge.net/projects/bbmap/). These tools are suggestions to perform post-processing of the sequencing reads produced by `dt4dds-challenges` via read merging and filtering. For further information, please refer to their respective repositories. 

These two tools are also used by the convenience scripts provided in the [scripts subfolder](/scripts/). The following scripts can be used on the sequencing output:

To perform filtering for sequencing adapters on single- or paired-reads:
```shell
./scripts/filter_single_reads.sh <R1_input> <R1_output>
./scripts/filter_paired_reads.sh <R1_input> <R2_input> <R1_output> <R2_output>
```

To perform merging of paired sequencing reads:
```shell
./scripts/merge_reads.sh <R1_input> <R2_input> <merged_output>
```

In addition, the two scripts [`./scripts/run_photolithography.sh`](/scripts/run_photolithography.sh) and [`./scripts/run_decay.sh`](/scripts/run_decay.sh) present a full pipeline for the two challenges (e.g. including simulation, filtering, and merging). These scripts are set up to use the input file at `./files/input_sequences.txt` and deposit the generated reads at `./files/output.txt`.

# License
This project is licensed under the GPLv3 license, see [here](LICENSE). Note that the additional programs that can be installed via the convenience scripts in the [tools subdirectory](/tools/) are the property of their respective owners and have their own licenses.