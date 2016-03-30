#ifndef SIG_GEN_HPP
#define SIG_GEN_HPP
#include <vector>
#include <complex>
class sig_gen
{
    public:
        virtual ~sig_gen(){};
        void complex_sine(std::vector<std::complex<float> > &data,
                int num_samples, double freq, double initial_phase);
        void const_signal(std::complex<float>* data, int num_samples);
    private:
        double next_phase;
};

#endif
