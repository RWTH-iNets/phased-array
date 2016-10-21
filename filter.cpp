#include "filter.hpp"

filter::filter() : num_taps(0), taps()
{

}

void filter::load_taps(const double* data, int len)
{
    num_taps = len;
    taps.assign(data, data + len);
}

// omits initial and exit states
// where filter is not fully filled.
// Therefore n_out = n_in - (n_taps - 1)
void filter::apply(const float* in, const int len,
                   double* out, int* len_out)
{
    *len_out = len - (num_taps - 1);
    for(int i = 0; i < *len_out; i++) {
        out[i] = 0.0;
        for(int j = 0; j < num_taps; j++) {
            out[i] += taps[j] * in[(num_taps - 1) + i - j];
        }
    }
}

void filter::apply(std::complex<float>* in_out, const int len,
                   int* len_out)
{
    *len_out = len - (num_taps - 1);
    std::complex<float> tmp;
    for(int i = 0; i < *len_out; i++) {
        tmp = std::complex<float>(0.0f, 0.0f);
        for(int j = 0; j < num_taps; j++) {
            tmp += ((float)taps[j]) * in_out[(num_taps - 1) + i - j];
        }
        in_out[i] = tmp;
    }
}
