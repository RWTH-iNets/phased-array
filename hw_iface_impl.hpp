#ifndef HW_IFACE_IMPL_HPP
#define HW_IFACE_IMPL_HPP

#include <complex>
#include <vector>
#include <fstream> 

#include <boost/thread/thread.hpp>

#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/stream.hpp>

#include "hw_iface.hpp"
#include "filter.hpp"

class hw_iface_impl : public hw_iface
{
    public:
        hw_iface_impl();
        void cal_rx_channels(std::complex<float>* cal_out, bool tx_ref) override;
        void send_tx_cal_tones_async(std::complex<float>* cal) override;
        void end_tx_cal_tones_async() override;
        void send_tx_cal_tones(std::complex<float>* cal) override;
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
        filter filter_lp;

};

#endif
