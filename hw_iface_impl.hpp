#ifndef HW_IFACE_IMPL_HPP
#define HW_IFACE_IMPL_HPP

#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/stream.hpp>
#include <complex>
#include <vector>
#include "hw_iface.hpp"
#include <boost/thread/thread.hpp>
#include <fstream> 

#define IP_USRP_0 "192.168.10.2"
#define IP_USRP_1 "192.168.10.3"

#ifndef TX_GAIN
#define TX_GAIN 5
#endif
#ifndef RX_GAIN
#define RX_GAIN 15
#endif

class hw_iface_impl : public hw_iface
{
    public:
        hw_iface_impl();
        void cal_rx_phase(float* delays_out) override;
        void send_tx_cal_tones_async(float* tx_phases) override;
        void end_tx_cal_tones_async() override;
        void send_tx_cal_tones(float* tx_phases) override;
        virtual ~hw_iface_impl(){};


    private:
        uhd::usrp::multi_usrp::sptr usrp;
        uhd::tx_streamer::sptr tx_streamer;
        uhd::rx_streamer::sptr rx_streamer;
        std::vector<std::complex<float> > sine_table;
        int ref_sig_num_samps_per_periode;
        bool run_tx_cal_tones_loop;
        boost::thread* tx_cal_thread;
        std::complex<float> rx_cal_data[4];
        std::complex<float> tx_cal_data[4];
        float rx_phase_alpha;
        std::ofstream ofs;

};

#endif
