#include <vector>
#include <stdexcept>
#include <numeric>

#include "conversion.hpp"
#include "mutator.hpp"
#include "rng.hpp"
#include "logging.hpp"

static Logger logger("mutator", "INFO");


namespace mutator {

    // handles the processing of a set of oligos
    void BaseMutator::process(std::vector<std::vector<char>> &oligos) {
        // loop through each oligo and process it
        if (this->get_manipulates_count()) {
            std::vector<std::vector<char>> new_oligos;
            for (std::vector<char> &oligo : oligos) {
                process_single_with_new(oligo, new_oligos);
            }
            // replace the new oligos in the results
            oligos.clear();
            if (new_oligos.size() > 0) {
                oligos = new_oligos;
            }
        } else {
            for (std::vector<char> &oligo : oligos) {
                process_single(oligo);
            }
        }
    }

    void BaseMutator::normalize_vector(std::vector<float> &vec) {
        // get the sum of the vector
        float sum = std::accumulate(vec.begin(), vec.end(), 0.0);

        // normalize the vector
        for (float &val : vec) {
            val /= sum;
        }
    }

    // check whether a mutation event occurs
    bool BaseMutator::is_mutation(float probability) {
        return rng::random_float() < probability;
    }

    // get the positions of events in a vector based on a probability distribution
    std::vector<int> BaseMutator::get_event_positions(std::vector<float>const &p_event_by_position) {
        std::vector<int> event_positions;
        for (int i = 0; i < p_event_by_position.size(); i++) {
            if (is_mutation(p_event_by_position[i])) {
                event_positions.push_back(i);
            }
        }
        return event_positions;
    }

    // get the positions of events in a vector based on a probability distribution
    void BaseMutator::draw_from_distribution(std::vector<int> &draws, std::discrete_distribution<> &sampler) {
        for (int &draw : draws) {
            draw = sampler(rng::rng);
        }
    }
    void BaseMutator::draw_from_distribution(std::vector<char> &draws, std::discrete_distribution<> &sampler) {
        for (char &draw : draws) {
            draw = sampler(rng::rng);
        }
    }

    // this function must be overwritten by a derived class
    void BaseMutator::process_single_with_new(std::vector<char> &oligo, std::vector<std::vector<char>> &new_oligos) {
        logger.critical("process_single_with_new() for new oligos must be overwritten by a derived class.");
        throw std::runtime_error("process_single_with_new() for new oligos must be overwritten by a derived class.");
    }
    void BaseMutator::process_single(std::vector<char> &oligo) {
        logger.critical("process_single() for existing oligos must be overwritten by a derived class.");
        throw std::runtime_error("process_single() for existing oligos must be overwritten by a derived class.");
    }



    //
    // INSERTION EVENTS
    //

    // constructor for the InsertionEvents class
    InsertionEvents::InsertionEvents(float rate, std::vector<float> p_base_preference, std::vector<float> p_event_lengths) {
        // initialize the rate of insertion events
        this->rate = rate;

        // initialize the preference for each base
        if (p_base_preference.size() != 4) {
            logger.critical("The base preference vector must have 4 elements.");
            throw std::runtime_error("The base preference vector must have 4 elements.");
        }
        this->p_base_preference = p_base_preference;
        normalize_vector(this->p_base_preference);
        this->_base_sampler = std::discrete_distribution<>(p_base_preference.begin(), p_base_preference.end());

        // initialize the probability of event lengths
        if (p_event_lengths.size() == 0) {
            this->_custom_event_lengths = false;
            this->p_event_lengths = {1.0};
        } else {
            this->_custom_event_lengths = true;
            this->p_event_lengths = p_event_lengths;
            normalize_vector(this->p_event_lengths);
            this->_event_lengths_sampler = std::discrete_distribution<>(p_event_lengths.begin(), p_event_lengths.end());
        }
    }

    // handles the insertion of a random base into a random position in the oligo
    void InsertionEvents::process_single(std::vector<char> &oligo) {
        // get the positions of the events, insertions are equally likely at each position
        std::vector<int> event_positions = get_event_positions(std::vector<float>(oligo.size(), rate));

        // short-circuit if there are no events
        if (event_positions.size() == 0) {
            return;
        }

        // generate event lengths
        std::vector<int> event_length(event_positions.size(), 1);
        if (this->_custom_event_lengths) {
            draw_from_distribution(event_length, _event_lengths_sampler);
            for (int &len : event_length) {
                len += 1;
            }
        }
        int total_insertions = std::accumulate(event_length.begin(), event_length.end(), 0);

        // generate new bases
        std::vector<char> new_bases(total_insertions, 1);
        draw_from_distribution(new_bases, _base_sampler);
        for (char &base : new_bases) {
            base += 1;
        }

        // insert the new bases into the oligo, starting from the back
        int length;
        int offset = 0;
        for (int i = event_positions.size() - 1; i >= 0; i--) {
            length = event_length[i];
            oligo.insert(oligo.begin() + event_positions[i] + 1, new_bases.begin() + offset, new_bases.begin() + offset + length);
            offset += length;
        }
    }


    //
    // DELETION EVENTS
    //

    // constructor for the DeletionEvents class
    DeletionEvents::DeletionEvents(float rate, std::vector<float> p_base_preference, std::vector<float> p_event_lengths) {
        // initialize the rate of deletion events
        this->rate = rate;

        // initialize the preference for each base
        if (p_base_preference.size() != 4) {
            logger.critical("The base preference vector must have 4 elements.");
            throw std::runtime_error("The base preference vector must have 4 elements.");
        }
        this->p_base_preference = p_base_preference;
        normalize_vector(this->p_base_preference);

        // initialize the probability of event lengths
        if (p_event_lengths.size() == 0) {
            this->_custom_event_lengths = false;
            this->p_event_lengths = {1.0};
        } else {
            this->_custom_event_lengths = true;
            this->p_event_lengths = p_event_lengths;
            normalize_vector(this->p_event_lengths);
            this->_event_lengths_sampler = std::discrete_distribution<>(p_event_lengths.begin(), p_event_lengths.end());
        }
    }

    // handles the deletion of a random base at a random position in the oligo
    void DeletionEvents::process_single(std::vector<char> &oligo) {
        // get the positions of the events, deletions are influenced by base type
        std::vector<float> p_event_by_position(oligo.size(), 4 * rate); // 4 is to go from probability to rate
        for (int i = 0; i < oligo.size(); i++) {
            p_event_by_position[i] *= p_base_preference[oligo[i] - 1];
        }
        std::vector<int> event_positions = get_event_positions(p_event_by_position);

        // short-circuit if there are no events
        if (event_positions.size() == 0) {
            return;
        }

        // generate event lengths
        std::vector<int> event_length(event_positions.size(), 1);
        if (this->_custom_event_lengths) {
            draw_from_distribution(event_length, _event_lengths_sampler);
            for (int &len : event_length) {
                len += 1;
            }
        }
        
        // delete the bases from the oligo, starting from the back
        int length;
        for (int i = event_positions.size() - 1; i >= 0; i--) {
            length = event_length[i];
            // make sure the deletion doesn't go past the end of the oligo
            if (event_positions[i] + length > oligo.size()) {
                length = oligo.size() - event_positions[i];
            }
            oligo.erase(oligo.begin() + event_positions[i], oligo.begin() + event_positions[i] + length);
        }
    }



    //
    // SUBSTITUTION EVENTS
    //

    // constructor for the InsertionEvents class
    SubstitutionEvents::SubstitutionEvents(float rate, std::vector<float> p_base_preference_list, std::vector<float> p_event_lengths) {
        // initialize the rate of insertion events
        this->rate = rate;

        // check the base preference list
        if (p_base_preference_list.size() != 4*3) {
            logger.critical("The base preference vector must have 4*3 elements.");
            throw std::runtime_error("The base preference vector must have 4*3 elements.");
        }
        normalize_vector(p_base_preference_list);

        // initialize the preference for an event occurring at each original base
        std::vector<float> p_base_preference(4, 0.0);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 3; j++) {
                p_base_preference[i] += p_base_preference_list[i*3 + j];
            }
        }
        this->p_base_preference = p_base_preference;
        normalize_vector(this->p_base_preference);
        
        // construct the sampler for the base preferences
        for (int i = 0; i < 4; i++) {
            std::vector<float> p_base_preference(3, 0.0);
            for (int j = 0; j < 3; j++) {
                p_base_preference[j] = p_base_preference_list[i*3 + j];
            }
            normalize_vector(p_base_preference);
            this->_base_sampler.push_back(std::discrete_distribution<>(p_base_preference.begin(), p_base_preference.end()));
        }

        // initialize the probability of event lengths
        if (p_event_lengths.size() == 0) {
            this->_custom_event_lengths = false;
            this->p_event_lengths = {1.0};
        } else {
            this->_custom_event_lengths = true;
            this->p_event_lengths = p_event_lengths;
            normalize_vector(this->p_event_lengths);
            this->_event_lengths_sampler = std::discrete_distribution<>(p_event_lengths.begin(), p_event_lengths.end());
        }
    }

    // handles the substitutions of bases at a random position in the oligo
    void SubstitutionEvents::process_single(std::vector<char> &oligo) {
        // get the positions of the events, substitutions are influenced by base type
        std::vector<float> p_event_by_position(oligo.size(), 4 * rate); // 4 is to go from probability to rate
        for (int i = 0; i < oligo.size(); i++) {
            p_event_by_position[i] *= p_base_preference[oligo[i] - 1];
        }
        std::vector<int> event_positions = get_event_positions(p_event_by_position);
    
        // short-circuit if there are no events
        if (event_positions.size() == 0) {
            return;
        }

        // generate event lengths
        std::vector<int> event_length(event_positions.size(), 1);
        if (this->_custom_event_lengths) {
            draw_from_distribution(event_length, _event_lengths_sampler);
            for (int &len : event_length) {
                len += 1;
            }
        }
        int total_substitutions = std::accumulate(event_length.begin(), event_length.end(), 0);
        int total_substitutions_by_base[4] = {0, 0, 0, 0};
        for (int i = 0; i < event_positions.size(); i++) {
            int pos = event_positions[i];
            int len = event_length[i];
            // ensure we don't go past the end of the oligo
            if (pos + len > oligo.size()) {
                len = oligo.size() - pos;
                event_length[i] = len;
            }
            for (int j = 0; j < len; j++) {
                total_substitutions_by_base[oligo[pos + j] - 1] += 1;
            }
        }

        // generate new bases for each base type
        std::vector<std::vector<char>> new_bases;
        new_bases.resize(4);
        for (int i = 0; i < 4; i++) {
            int n_bases = total_substitutions_by_base[i];
            if (n_bases == 0) {
                continue;
            }
            new_bases[i].clear();
            new_bases[i].resize(total_substitutions_by_base[i], 1);
            draw_from_distribution(new_bases[i], _base_sampler[i]);
            for (char &base : new_bases[i]) {
                base += 1; // go from 0-3 to 1-4
                if (base >= i + 1) { // make sure the base is not the same as the original
                    base += 1;
                }
            }
        }
        
        // substitute the new bases into the oligo
        int length;
        int offset[4] = {0, 0, 0, 0};
        for (int i = 0; i < event_positions.size(); i++) {
            int pos = event_positions[i];
            int len = event_length[i];
            for (int j = 0; j < len; j++) {
                int basetype = oligo[pos + j] - 1;
                // short-circuit if there is no substitution for this base because we've already substituted it
                if (offset[basetype] >= new_bases[basetype].size()) {
                    offset[basetype] = 0;
                    continue;
                }
                oligo[pos + j] = new_bases[basetype][offset[basetype]];
                offset[basetype] += 1;
            }
        }
    }



    //
    // BREAKAGE EVENTS
    //

    // constructor for the BreakageEvents class
    BreakageEvents::BreakageEvents(float rate, std::vector<float> p_base_preference) {
        // initialize the rate of insertion events
        this->rate = rate;

        // check the base preference list
        if (p_base_preference.size() != 4) {
            logger.critical("The base preference vector must have 4 elements.");
            throw std::runtime_error("The base preference vector must have 4 elements.");
        }
        this->p_base_preference = p_base_preference;
        normalize_vector(this->p_base_preference);
    }

    // handles the breakage of a random base at a random position in the oligo
    void BreakageEvents::process_single_with_new(std::vector<char> &oligo, std::vector<std::vector<char>> &new_oligos) {
        // get the positions of the breakage events, breaks are influenced by base type
        std::vector<float> p_event_by_position(oligo.size(), 4 * rate); // 4 is to go from probability to rate
        for (int i = 0; i < oligo.size(); i++) {
            p_event_by_position[i] *= p_base_preference[oligo[i] - 1];
        }
        std::vector<int> event_positions = get_event_positions(p_event_by_position);

        // short-circuit if there are no events
        if (event_positions.size() == 0) {
            new_oligos.push_back(oligo);
            return;
        }
        
        // save the fragments of the oligo
        int last_pos = 0;
        for (int i = 0; i < event_positions.size(); i++) {
            int pos = event_positions[i];
            // short-circuit if the breakage occurs at the end of the previous one
            if (pos == last_pos) {
                last_pos = pos + 1;
                continue;
            }
            new_oligos.push_back(std::vector<char>(oligo.begin() + last_pos, oligo.begin() + pos));
            last_pos = pos + 1;
        }
        // save the last fragment
        if (last_pos < oligo.size()) {
            new_oligos.push_back(std::vector<char>(oligo.begin() + last_pos, oligo.end()));
        }
    }



    //
    // SIZE SELECTION
    //

    // constructor for the SizeSelection class
    SizeSelection::SizeSelection(int lower_cutoff, int upper_threshold) {
        // initialize the cutoff and slope
        this->lower_cutoff = lower_cutoff;
        this->upper_threshold = upper_threshold;
    }

    // handles the size selection of a single oligo
    void SizeSelection::process_single_with_new(std::vector<char> &oligo, std::vector<std::vector<char>> &new_oligos) {
        // get the size of the oligo
        int size = oligo.size();

        // short-circuit if the oligo is below the cut-off
        if (size <= lower_cutoff) {
            return;
        }
        if (size >= upper_threshold) {
            new_oligos.push_back(oligo);
            return;
        }

        // calculate the probability of the oligo being selected
        float p_select = (double)(size - lower_cutoff)/(upper_threshold - lower_cutoff);

        // test if this oligo will be selected
        if (rng::random_float() < p_select) {
            new_oligos.push_back(oligo);
        }
    }



    //
    // ADD REVERSE COMPLEMENT
    //

    // constructor for the AddReverseComplement class
    AddReverseComplement::AddReverseComplement() {
    }

    // handles the addition of the reverse complement of a single oligo
    void AddReverseComplement::process_single_with_new(std::vector<char> &oligo, std::vector<std::vector<char>> &new_oligos) {
        // add the oligo to the new oligos
        new_oligos.push_back(oligo);

        // add the reverse complement to the new oligos
        new_oligos.push_back(conversion::reverse_complement(oligo));
    }



    //
    // TAILING
    //

    // constructor for the Tailing class
    Tailing::Tailing(std::string tail_bases, int n_min, int n_max) {
        // initialize the tail bases
        conversion::sequence_to_vector(tail_bases, this->_tail_bases);

        // initialize the minimum and maximum number of tails
        this->n_min = n_min;
        this->n_max = n_max;
        for (int i = n_min; i <= n_max; i++) {
            this->_tail_lengths.push_back(i);
        }

        // initialize the length and base samplers
        std::vector<float> p_tail_lengths(n_max - n_min + 1, 1.0);
        this->_length_sampler = std::discrete_distribution<>(p_tail_lengths.begin(), p_tail_lengths.end());

        std::vector<float> p_base_preference(this->_tail_bases.size(), 1.0);
        this->_base_sampler = std::discrete_distribution<>(p_base_preference.begin(), p_base_preference.end());
    }

    // handles the tailing of a single oligo
    void Tailing::process_single(std::vector<char> &oligo) {
        // get the length of the tail
        int length = _tail_lengths[_length_sampler(rng::rng)];

        // get the bases of the tail
        std::vector<char> tail(length, 1);
        draw_from_distribution(tail, _base_sampler);
        for (char &base : tail) {
            base = _tail_bases[base];
        }

        // append the tail to the oligo
        oligo.insert(oligo.end(), tail.begin(), tail.end());
    }



    //
    // END SHREDS
    //

    // constructor for the EndShreds class
    EndShreds::EndShreds(std::vector<float> p_removal_lengths) {
        // initialize the removal lengths
        this->p_removal_length = p_removal_lengths;
        normalize_vector(this->p_removal_length);

        // initialize the length and base samplers
        this->_length_sampler = std::discrete_distribution<>(this->p_removal_length.begin(), this->p_removal_length.end());
    }

    // handles the shredded ends of a single oligo
    void EndShreds::process_single(std::vector<char> &oligo) {
        // get the length to cut
        std::vector<int> lengths(2, 0);
        draw_from_distribution(lengths, _length_sampler);

        // remove the last bases from the oligo
        if (lengths[0] > 0) {
            oligo.resize(oligo.size() - lengths[0]);
        }

        // remove the first bases from the oligo
        if (lengths[1] > 0) {
            oligo.erase(oligo.begin(), oligo.begin() + lengths[1]);
        }
    }



    //
    // SEQUENCING ADD ADAPTER
    //

    // constructor for the SequencingAddAdapter class
    SequencingAddAdapter::SequencingAddAdapter(std::string adapter_sequence) {
        // initialize the adapter bases
        conversion::sequence_to_vector(adapter_sequence, this->_adapter_vector);
    }

    // handles the adapter addition to a single oligo
    void SequencingAddAdapter::process_single(std::vector<char> &oligo) {
        // append the adapter to the oligo
        oligo.insert(oligo.end(), _adapter_vector.begin(), _adapter_vector.end());
    }



    //
    // SEQUENCING PAD AND TRIM
    //

    // constructor for the SequencingAddAdapter class
    SequencingPadTrim::SequencingPadTrim(int read_length) {
        // initialize the read length
        this->read_length = read_length;

        // initialize the base sampler
        std::vector<float> p_base_preference(4, 1.0);
        this->_base_sampler = std::discrete_distribution<>(p_base_preference.begin(), p_base_preference.end());
    }

    // handles the adapter addition to a single oligo
    void SequencingPadTrim::process_single(std::vector<char> &oligo) {
        // get the length of the oligo
        int length = oligo.size();

        // pad the oligo if it is shorter than the read length
        if (length < read_length) {
            std::vector<char> padding(read_length - length, 1);
            draw_from_distribution(padding, _base_sampler);
            for (char &base : padding) {
                base += 1;
            }
            oligo.insert(oligo.end(), padding.begin(), padding.end());
        }

        // trim the oligo if it is longer than the read length
        if (length > read_length) {
            oligo.resize(read_length);
        }
    }



} // namespace mutator