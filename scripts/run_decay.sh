#!/bin/bash 
set -e
BASE_PATH="$(dirname -- "${BASH_SOURCE[0]}")"

# run the challenge to generate reads
"$BASE_PATH"/../bin/dt4dds-challenge decay --strict "$BASE_PATH"/../files/input_sequences.txt "$BASE_PATH"/../files/R1.fq "$BASE_PATH"/../files/R2.fq -f fastq

# zip the files
gzip "$BASE_PATH"/../files/R1.fq
gzip "$BASE_PATH"/../files/R2.fq

# filter the reads
"$BASE_PATH"/filter_paired_reads.sh "$BASE_PATH"/../files/R1.fq.gz "$BASE_PATH"/../files/R2.fq.gz "$BASE_PATH"/../files/R1_filtered.fq.gz "$BASE_PATH"/../files/R2_filtered.fq.gz

# merge the reads
"$BASE_PATH"/../tools/ngmerge/ngmerge -1 "$BASE_PATH"/../files/R1_filtered.fq.gz -2 "$BASE_PATH"/../files/R2_filtered.fq.gz -o "$BASE_PATH"/../files/output.fq.gz -m 10 -d -e 10 -z -n 4 -v

# unzip the file
gunzip "$BASE_PATH"/../files/output.fq.gz

# convert the file to txt by printing only every 4th line
awk 'NR % 4 == 2' "$BASE_PATH"/../files/output.fq > "$BASE_PATH"/../files/output.fq.select

# shuffle the lines in file
shuf "$BASE_PATH"/../files/output.fq.select > "$BASE_PATH"/../files/output.txt

# delete intermediate files
rm -f "$BASE_PATH"/../files/R1.fq.gz
rm -f "$BASE_PATH"/../files/R2.fq.gz
rm -f "$BASE_PATH"/../files/R1_filtered.fq.gz
rm -f "$BASE_PATH"/../files/R2_filtered.fq.gz
rm -f "$BASE_PATH"/../files/output.fq
rm -f "$BASE_PATH"/../files/output.fq.select

exit