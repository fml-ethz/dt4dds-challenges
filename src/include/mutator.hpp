#ifndef MUTATOR_HPP
#define MUTATOR_HPP

#include <vector>
#include <random>


namespace mutator {

    class BaseMutator {
        private:
            std::string name = "BaseMutator";
            bool manipulates_count = false;
            virtual void process_single_with_new(std::vector<char> &oligo, std::vector<std::vector<char>> &new_oligos);
            virtual void process_single(std::vector<char> &oligo);

        public:
            virtual std::string get_name() const { return name; }
            virtual bool get_manipulates_count() const { return manipulates_count; }
            virtual void process(std::vector<std::vector<char>> &oligos);
            void normalize_vector(std::vector<float> &vec);
            bool is_mutation(float probability);
            std::vector<int> get_event_positions(std::vector<float>const &p_event_by_position);
            void draw_from_distribution(std::vector<int> &draws, std::discrete_distribution<> &sampler);
            void draw_from_distribution(std::vector<char> &draws, std::discrete_distribution<> &sampler);
    };



    class InsertionEvents : public BaseMutator {
        private:
            std::string name = "InsertionEvents";
            bool manipulates_count = false;
            bool _custom_event_lengths = false;
            std::discrete_distribution<> _event_lengths_sampler;
            std::discrete_distribution<> _base_sampler;

            virtual void process_single(std::vector<char> &oligo) override;

        public:
            virtual std::string get_name() const { return name; }
            virtual bool get_manipulates_count() const { return manipulates_count; }
            float rate = 0.0;
            std::vector<float> p_event_lengths;
            std::vector<float> p_base_preference;

            InsertionEvents(float rate, std::vector<float> p_base_preference = {0.25, 0.25, 0.25, 0.25}, std::vector<float> p_event_lengths = {});
    };


    class DeletionEvents : public BaseMutator {
        private:
            std::string name = "DeletionEvents";
            bool manipulates_count = false;
            bool _custom_event_lengths = false;
            std::discrete_distribution<> _event_lengths_sampler;

            virtual void process_single(std::vector<char> &oligo) override;

        public:
            virtual std::string get_name() const { return name; }
            virtual bool get_manipulates_count() const { return manipulates_count; }
            float rate = 0.0;
            std::vector<float> p_event_lengths;
            std::vector<float> p_base_preference;

            DeletionEvents(float rate, std::vector<float> p_base_preference = {0.25, 0.25, 0.25, 0.25}, std::vector<float> p_event_lengths = {});
    };


    class SubstitutionEvents : public BaseMutator {
        private:
            std::string name = "SubstitutionEvents";
            bool manipulates_count = false;
            bool _custom_event_lengths = false;
            std::discrete_distribution<> _event_lengths_sampler;
            std::vector<std::discrete_distribution<>> _base_sampler;

            virtual void process_single(std::vector<char> &oligo) override;

        public:
            virtual std::string get_name() const { return name; }
            virtual bool get_manipulates_count() const { return manipulates_count; }
            float rate = 0.0;
            std::vector<float> p_event_lengths;
            std::vector<float> p_base_preference;

            SubstitutionEvents(float rate, std::vector<float> p_base_preference = {0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25}, std::vector<float> p_event_lengths = {});
    };


    class BreakageEvents : public BaseMutator {
        private:
            std::string name = "BreakageEvents";
            bool manipulates_count = true;
            std::vector<char> _adapter_vector;
            std::discrete_distribution<> _base_sampler;

            virtual void process_single_with_new(std::vector<char> &oligo, std::vector<std::vector<char>> &new_oligos) override;

        public:
            virtual std::string get_name() const { return name; }
            virtual bool get_manipulates_count() const { return manipulates_count; }
            float rate = 0.0;
            std::vector<float> p_base_preference;

            BreakageEvents(float rate, std::vector<float> p_base_preference = {0.25, 0.25, 0.25, 0.25});
    };


    class SizeSelection : public BaseMutator {
        private:
            std::string name = "SizeSelection";
            bool manipulates_count = true;
            virtual void process_single_with_new(std::vector<char> &oligo, std::vector<std::vector<char>> &new_oligos) override;

        public:
            virtual std::string get_name() const { return name; }
            virtual bool get_manipulates_count() const { return manipulates_count; }
            int lower_cutoff;
            int upper_threshold;

            SizeSelection(int lower_cutoff, int upper_threshold);
    };


    class AddReverseComplement : public BaseMutator {
        private:
            std::string name = "AddReverseComplement";
            bool manipulates_count = true;
            virtual void process_single_with_new(std::vector<char> &oligo, std::vector<std::vector<char>> &new_oligos) override;

        public:
            virtual std::string get_name() const { return name; }
            virtual bool get_manipulates_count() const { return manipulates_count; }

            AddReverseComplement();
    };


    class Tailing : public BaseMutator {
        private:
            std::string name = "Tailing";
            bool manipulates_count = false;
            std::vector<char> _adapter_vector;
            std::vector<char> _tail_bases;
            std::vector<int> _tail_lengths;
            std::discrete_distribution<> _base_sampler;
            std::discrete_distribution<> _length_sampler;

            virtual void process_single(std::vector<char> &oligo) override;

        public:
            virtual std::string get_name() const { return name; }
            virtual bool get_manipulates_count() const { return manipulates_count; }
            int n_min;
            int n_max;
            Tailing(std::string tail_bases, int n_min, int n_max);
    };


    class EndShreds : public BaseMutator {
        private:
            std::string name = "EndShreds";
            bool manipulates_count = false;
            std::discrete_distribution<> _length_sampler;

            virtual void process_single(std::vector<char> &oligo) override;

        public:
            virtual std::string get_name() const { return name; }
            virtual bool get_manipulates_count() const { return manipulates_count; }
            std::vector<float> p_removal_length;
            EndShreds(std::vector<float> p_removal_lengths);
    };


    class SequencingAddAdapter : public BaseMutator {
        private:
            std::string name = "SequencingAddAdapter";
            bool manipulates_count = false;
            std::vector<char> _adapter_vector;

            virtual void process_single(std::vector<char> &oligo) override;

        public:
            virtual std::string get_name() const { return name; }
            virtual bool get_manipulates_count() const { return manipulates_count; }
            SequencingAddAdapter(std::string adapter_sequence);
    };


    class SequencingPadTrim : public BaseMutator {
        private:
            std::string name = "SequencingPadTrim";
            bool manipulates_count = false;
            std::discrete_distribution<> _base_sampler;

            virtual void process_single(std::vector<char> &oligo) override;

        public:
            virtual std::string get_name() const { return name; }
            virtual bool get_manipulates_count() const { return manipulates_count; }
            int read_length;
            SequencingPadTrim(int read_length);
    };

} // namespace mutators

#endif // MUTATOR_HPP