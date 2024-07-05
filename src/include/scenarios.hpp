#ifndef SCENARIOS_HPP
#define SCENARIOS_HPP

#include <vector>
#include <numeric>
#include <memory>

#include "mutator.hpp"

namespace scenarios {


    void challenge_decay(
        float& initial_coverage_bias,
        float& mean_physical_coverage,
        float& mean_sequencing_coverage,
        int& read_length,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& initial_mutators,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& recovery_mutators
    ) {
        // bias and coverages
        initial_coverage_bias = 0.30;
        mean_physical_coverage = 10;
        mean_sequencing_coverage = 30;
        read_length = 150;

        // mutators for synthesis + aging
        initial_mutators.push_back(std::make_unique<mutator::SubstitutionEvents>(
            0.000109*15, // 15 cycles of PCR amplification with Taq polymerase
            std::vector<float>{0.0147, 0.3028, 0.0630, 0.0150, 0.0071, 0.0975, 0.0975, 0.0071, 0.0150, 0.0630, 0.3028, 0.0147}
            // base bias       A2C     A2G     A2T     C2A     C2G     C2T     G2A     G2C     G2T    T2A     T2C     T2G
        ));
        initial_mutators.push_back(std::make_unique<mutator::DeletionEvents>(
            0.0005695, // Twist synthesis deletion rate
            std::vector<float>{0.2468, 0.2362, 0.2669, 0.2500},
            // base bias       A       C       G       T
            std::vector<float>{0.8602, 0.0612, 0.0178, 0.0111, 0.0083, 0.0072, 0.0062, 0.0054, 0.0048, 0.0041, 0.0037, 0.0030, 0.0023, 0.0020, 0.0016, 0.0010}
            // length bias     1       2       3       4       5       6       7       8       9       10      11      12      13      14      15      16
        ));
        initial_mutators.push_back(std::make_unique<mutator::AddReverseComplement>());
        initial_mutators.push_back(std::make_unique<mutator::BreakageEvents>(
            0.023, // Aging for five half-lives at 150 nt is equivalent to this per-base rate
            std::vector<float>{0.3902, 0.0488, 0.4878, 0.0732}
            // base bias       A       C       G       T
        ));
        initial_mutators.push_back(std::make_unique<mutator::SizeSelection>(
            // Bead-based purification with bead ratio of 1.8, considering the adapter length of 33 nt + 8 nt tail
            60-33-8, // lower cutoff
            140-33-8 // upper threshold
        ));
        initial_mutators.push_back(std::make_unique<mutator::Tailing>(
            "CT", // Tailing of the single-stranded workflow introduces a CT adapter
            6, 8 // with between 6 and 8 nt in length
        ));

        // mutators for recovery
        recovery_mutators.push_back(std::make_unique<mutator::SubstitutionEvents>(
            0.000109*15, // 15 cycles of PCR amplification with Taq polymerase
            std::vector<float>{0.0147, 0.3028, 0.0630, 0.0150, 0.0071, 0.0975, 0.0975, 0.0071, 0.0150, 0.0630, 0.3028, 0.0147}
            // base bias       A2C     A2G     A2T     C2A     C2G     C2T     G2A     G2C     G2T    T2A     T2C     T2G
        ));
    }


    void challenge_photolithography(
        float& initial_coverage_bias,
        float& mean_physical_coverage,
        float& mean_sequencing_coverage,
        int& read_length,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& initial_mutators,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& recovery_mutators
    ) {
        // bias and coverages
        initial_coverage_bias = 0.44;
        mean_physical_coverage = 200;
        mean_sequencing_coverage = 50;
        read_length = 150;

        // mutators for photolithographic synthesis
        initial_mutators.push_back(std::make_unique<mutator::EndShreds>(
            std::vector<float>{0.4882, 0.1189, 0.0635, 0.0342, 0.0202, 0.0137, 0.0117, 0.0110, 0.0096, 0.0091}
            // length bias     1       2       3       4       5       6       7       8       9       10
        ));
        initial_mutators.push_back(std::make_unique<mutator::SubstitutionEvents>(
            0.0212, // synthesis substitution rate
            std::vector<float>{0.085, 0.058, 0.063, 0.088, 0.081, 0.063, 0.095, 0.073, 0.183, 0.081, 0.063, 0.094},
            // base bias       A2C    A2G    A2T    C2A    C2G    C2T    G2A    G2C    G2T    T2A    T2C    T2G
            std::vector<float>{0.8420, 0.1277, 0.0232, 0.0071}
            // length bias     1       2       3       4
        ));
        initial_mutators.push_back(std::make_unique<mutator::DeletionEvents>(
            0.0683, // synthesis deletion rate
            std::vector<float>{0.25, 0.25, 0.25, 0.25},
            // base bias       A     C     G     T
            std::vector<float>{0.8556, 0.1026, 0.0227, 0.0191}
            // length bias     1       2       3       4
        ));
        initial_mutators.push_back(std::make_unique<mutator::InsertionEvents>(
            0.0136, // synthesis insertion rate
            std::vector<float>{0.25, 0.25, 0.25, 0.25},
            // base bias       A     C     G     T
            std::vector<float>{0.9275, 0.0453, 0.0126, 0.0146}
            // length bias     1       2       3       4
        ));

        // mutators for recovery
        recovery_mutators.push_back(std::make_unique<mutator::SubstitutionEvents>(
            0.000109*15, // 15 cycles of PCR amplification with Taq polymerase
            std::vector<float>{0.0147, 0.3028, 0.0630, 0.0150, 0.0071, 0.0975, 0.0975, 0.0071, 0.0150, 0.0630, 0.3028, 0.0147}
            // base bias       A2C     A2G     A2T     C2A     C2G     C2T     G2A     G2C     G2T    T2A     T2C     T2G
        ));
    }



    void sequencing(
        bool add_adapters,
        bool pad_and_trim,
        int read_length,
        std::vector<std::unique_ptr<mutator::BaseMutator>>& mutators
    ) {
        if (add_adapters) {
            mutators.push_back(std::make_unique<mutator::SequencingAddAdapter>(
                "AGATCGGAAGAGC" // General Illumina read adapter (already rc'ed)
            ));
        }
        if (pad_and_trim) {
            mutators.push_back(std::make_unique<mutator::SequencingPadTrim>(
                read_length // Pad + trim to the read length
            ));
        }
        mutators.push_back(std::make_unique<mutator::SubstitutionEvents>(
            0.0018115, // iSeq 100 sequencing, error rate averaged over both reads
            std::vector<float>{0.0029, 0.2065, 0.1684, 0.0246, 0.0139, 0.1594, 0.1761, 0.0184, 0.0377, 0.0203, 0.1060, 0.0657}
            // base bias       A2C     A2G     A2T     C2A     C2G     C2T     G2A     G2C     G2T    T2A     T2C     T2G
        ));
    }

}


#endif // SCENARIOS_HPP