#include "usrp_tx.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/assign/list_of.hpp> // for 'list_of()'
#include <boost/assert.hpp> 
#include <boost/thread.hpp>

usrp_tx::usrp_tx(double tx_freq, double tx_rate) : phase_cal(4) {

    this->phase_cal[0] = std::polar(1.0f, 10.0f);
    this->phase_cal[1] = std::polar(1.0f, 0.0f);
    this->phase_cal[2] = std::polar(1.0f, 0.0f);
    this->phase_cal[3] = std::polar(1.0f, 10.0f);

    //create a usrp device
    uhd::device_addr_t addr_args;
    addr_args["addr0"] = IP_USRP_0;
    addr_args["addr1"] = IP_USRP_1;

    this->usrp = uhd::usrp::multi_usrp::make(addr_args);
    //set subdev spec on all mboards.
    //use TX/RX ports on both dboards.
    this->usrp->set_tx_subdev_spec(uhd::usrp::subdev_spec_t("A:0 B:0"));

    this->usrp->set_clock_source("external"); //applies to all mboards
    this->usrp->set_time_source("external"); //applies to all mboards

    this->usrp->set_tx_rate(tx_rate);

    this->usrp->set_tx_gain(TX_GAIN, 0);
    this->usrp->set_tx_gain(TX_GAIN, 1);
    this->usrp->set_tx_gain(TX_GAIN, 2);
    this->usrp->set_tx_gain(TX_GAIN, 3);

    //Set time at next PPS to zero
    //Convenience function. Waits for PPS on one device and then
    //sets time at next PPS on all devices
    this->usrp->set_time_unknown_pps(uhd::time_spec_t(0.0));
    boost::this_thread::sleep(boost::posix_time::seconds(2));

    //we will tune the frontends in 500ms from now
    uhd::time_spec_t cmd_time = usrp->get_time_now() +
        uhd::time_spec_t(0.5);
    //sets command time on all devices
    //the next commands are all timed
    std::cout << "tuning all frontends to " << tx_freq << std::endl;
    this->usrp->set_command_time(cmd_time);

    this->usrp->set_tx_freq(tx_freq, 0);
    this->usrp->set_tx_freq(tx_freq, 1);
    this->usrp->set_tx_freq(tx_freq, 2);
    this->usrp->set_tx_freq(tx_freq, 3);

    //end timed commands
    this->usrp->clear_command_time();

    std::cout << "devices initialized" << std::endl;

    uhd::stream_args_t stream_args("fc32", "sc16");
    stream_args.channels = {0ul,1ul,2ul,3ul};
    this->tx_streamer = this->usrp->get_tx_stream(stream_args);

}

int usrp_tx::send(
        std::vector<std::complex<float>* > &data,
        int num_samples,
        bool start_of_burst,
        bool end_of_burst)
{
    uhd::tx_metadata_t meta;
    if(start_of_burst) {
        std::cout << "starting burst" << std::endl;
        meta.start_of_burst = true;
        meta.has_time_spec = true;
        meta.time_spec = this->usrp->get_time_now() +
                            uhd::time_spec_t(0.1);
    } else if(end_of_burst) {
        std::cout << "ending burst" << std::endl;
        meta.end_of_burst = true;
    } else {

    }


    return this->tx_streamer->send(data, num_samples, meta);
}

