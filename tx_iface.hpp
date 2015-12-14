#ifndef TX_IFACE_HPP
#define TX_IFACE_HPP

#include <complex>
#include <vector>

class tx_iface
{
    public:
        virtual ~tx_iface(){};
        virtual int send(
            std::vector<std::complex<float>* > &data,
            int num_samples,
            bool start_of_burst,
            bool end_of_burst) = 0;
};

#endif
