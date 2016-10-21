#ifndef FILTER_HPP
#define FILTER_HPP

#include <vector>
#include <complex>

class filter
{
   private:
        std::vector<double> taps;
        int num_taps;
    public:
        filter();
        void load_taps(const double* data, int len);
        void apply(const float* in, const int len,
                   double* out, int* len_out);
        void apply(std::complex<float>* in_out, const int len,
                   int* len_out);
};
#endif
