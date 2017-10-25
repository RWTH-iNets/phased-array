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

        void cal_rx(std::complex<float>* cal_out, bool tx_ref) override;

        void tx_const_async(std::complex<float>* cal) override;
        void end_tx_const_async() override;
        void tx_const(std::complex<float>* cal) override;

        virtual ~hw_iface_impl(){};

    private:
        uhd::usrp::multi_usrp::sptr usrp;
        uhd::tx_streamer::sptr tx_streamer;
        uhd::rx_streamer::sptr rx_streamer;

        bool run_tx_const_loop;
        boost::thread* tx_const_thread;

        filter filter_lp;

};

#endif
