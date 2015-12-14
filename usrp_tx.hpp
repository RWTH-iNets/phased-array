#ifndef USRP_TX_HPP
#define USRP_TX_HPP

#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/stream.hpp>
#include <complex>
#include <vector>
#include "tx_iface.hpp"

#define IP_USRP_0 "192.168.10.2"
#define IP_USRP_1 "192.168.10.3"
#define TX_GAIN 5

class usrp_tx : public tx_iface
{
    public:
        virtual ~usrp_tx(){};
        usrp_tx(double tx_freq, double tx_rate);
        int send(
            std::vector<std::complex<float>* > &data,
            int num_samples,
            bool start_of_burst,
            bool end_of_burst) override;
    private:
        uhd::usrp::multi_usrp::sptr usrp;
        uhd::tx_streamer::sptr tx_streamer;
        std::vector<std::complex<float> > phase_cal;
};

#endif
