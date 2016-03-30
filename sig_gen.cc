#include "sig_gen.hpp"

void sig_gen::complex_sine(std::vector<std::complex<float> > & data,
                int num_samples, double freq,
                double initial_phase)
{
    data.clear();
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

void sig_gen::const_signal(std::complex<float>* data, int num_samples)
{
    //data.clear();
    for(int i = 0; i < num_samples; i++) {
        data[i] = (std::complex<float>(0.5,0.5));
        //data[i] = (std::complex<float>(0.0,0.0));
    }
}
