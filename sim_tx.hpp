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
            std::vector<std::vector<std::complex<float> > > &buffers,
            int num_samples,
            bool sob,
            bool eob) override
        {
            std::cout << "sending data from " << buffers.size() <<
               " buffers" << std::endl;
            if(buffers.size() > 0) {
                std::vector<std::complex<float> > *buf = &buffers[0];
                for(int i = 0; i < buf->size(); i++) {
                    std::cout << (*buf)[i] << std::endl;
                }
            }
            return 0;
        }
};

#endif
