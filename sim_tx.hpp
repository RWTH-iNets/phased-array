#ifndef SIM_TX_HPP
#define SIM_TX_HPP

#include <complex>
#include <vector>

#include "tx_iface.hpp"

class sim_tx : public tx_iface
{
    public:
        virtual ~sim_tx(){};
        inline int send(
            std::vector<std::complex<float>* > &data,
            int num_samples,
            bool sob,
            bool eob) override
        {
            return 0;
        }
};

#endif
