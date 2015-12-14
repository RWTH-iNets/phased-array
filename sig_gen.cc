#include "sig_gen.hpp"

void sig_gen::complex_sine(std::vector<std::complex<float> > & data,
                int num_samples, double freq,
                double initial_phase)
{
    double curr_phase = 0.0;
    for(int i = 0; i < num_samples; i++) {
        curr_phase = i * 2.0 * M_PI * freq + initial_phase +
            this->next_phase;

        std::complex<float> curr = std::polar(1.0f, (float)curr_phase);
        data.push_back(curr);
    }

    //advance phase to next point
    this->next_phase = curr_phase + 2.0 * M_PI * freq;
}
