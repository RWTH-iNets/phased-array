#include "hw_iface_impl.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/assign/list_of.hpp> // for 'list_of()'
#include <boost/assert.hpp>
#include "filter.hpp"
#include <string>
#include <time.h>
#include <algorithm>
#include <sstream>

hw_iface_impl::hw_iface_impl():ref_sig_num_samps_per_periode(1024), tx_cal_thread(0),
                                rx_cal_data{std::polar(1.0f, 0.0f),
                                            std::polar(1.0f, (float)M_PI*(57.6f / 180.0f)),
                                            std::polar(1.0f, (float)M_PI*(-6.6f / 180.0f)),
                                            std::polar(1.0f, (float)M_PI*(0.9f / 180.0f))},
                                rx_phase_alpha(0.85f),
                                // RC filter, tau = xx samples (alpha = exp(-1/tau))
                                ofs()
{

    for( int i = 0; i < ref_sig_num_samps_per_periode; i++) {
        sine_table.push_back(std::polar(1.0f,
            (float)(((2.0f*M_PI) / (float)ref_sig_num_samps_per_periode) * i)));
    }

    //create a tx_usrp device
    uhd::device_addr_t addr_args;
    addr_args["addr0"] = IP_USRP_0;
    addr_args["addr1"] = IP_USRP_1;

    this->usrp = uhd::usrp::multi_usrp::make(addr_args);

    //set subdev spec on all mboards.
    //use TX/RX ports on both dboards.
    this->usrp->set_tx_subdev_spec(uhd::usrp::subdev_spec_t("A:0 B:0"));
    this->usrp->set_rx_subdev_spec(uhd::usrp::subdev_spec_t("A:0 B:0"));
    this->usrp->set_rx_antenna("RX2", 0);
    this->usrp->set_rx_antenna("RX2", 1);
    this->usrp->set_rx_antenna("RX2", 2);
    this->usrp->set_rx_antenna("RX2", 3);
    this->usrp->set_tx_antenna("TX/RX", 0);
    this->usrp->set_tx_antenna("TX/RX", 1);
    this->usrp->set_tx_antenna("TX/RX", 2);
    this->usrp->set_tx_antenna("TX/RX", 3);

    this->usrp->set_clock_source("external"); //applies to all mboards
    this->usrp->set_time_source("external"); //applies to all mboards

    double tx_freq = 3.7001e9;
    double rx_freq = 3.7e9;
    //Set time at next PPS to zero
    //Convenience function. Waits for PPS on one device and then
    //sets time at next PPS on all devices
    this->usrp->set_time_unknown_pps(uhd::time_spec_t(0.0));
    boost::this_thread::sleep(boost::posix_time::seconds(3));

    //we will tune the frontends in 300ms from now
    std::cout << "tuning all tx frontends to " << tx_freq << std::endl;
    uhd::time_spec_t cmd_time = usrp->get_time_now() +
        uhd::time_spec_t(0.3);
    //sets command time on all devices
    //the next commands are all timed
    this->usrp->set_command_time(cmd_time);
    this->usrp->set_tx_freq(tx_freq, 0);
    this->usrp->set_tx_freq(tx_freq, 1);
    this->usrp->set_tx_freq(tx_freq, 2);
    this->usrp->set_tx_freq(tx_freq, 3);
    boost::this_thread::sleep(boost::posix_time::seconds(3));
    //end timed commands
    this->usrp->clear_command_time();

    std::cout << "tuning all rx frontends to " << rx_freq << std::endl;
    cmd_time = usrp->get_time_now() +
        uhd::time_spec_t(0.3);
    //sets command time on all devices
    //the next commands are all timed
    this->usrp->set_command_time(cmd_time);
    this->usrp->set_rx_freq(rx_freq, 0);
    this->usrp->set_rx_freq(rx_freq, 1);
    this->usrp->set_rx_freq(rx_freq, 2);
    this->usrp->set_rx_freq(rx_freq, 3);
    boost::this_thread::sleep(boost::posix_time::seconds(3));
    //end timed commands
    this->usrp->clear_command_time();

    uhd::stream_args_t stream_args_rx("fc32", "sc16");
    stream_args_rx.channels = {0ul,1ul,2ul,3ul};
    this->rx_streamer = this->usrp->get_rx_stream(stream_args_rx);

    this->usrp->set_tx_rate(1e6);
    this->usrp->set_rx_rate(1e6);

    this->usrp->set_tx_gain(TX_GAIN, 0);
    this->usrp->set_tx_gain(TX_GAIN, 1);
    this->usrp->set_tx_gain(TX_GAIN, 2);
    this->usrp->set_tx_gain(TX_GAIN, 3);

    this->usrp->set_rx_gain(RX_GAIN, 0);
    this->usrp->set_rx_gain(RX_GAIN, 1);
    this->usrp->set_rx_gain(RX_GAIN, 2);
    this->usrp->set_rx_gain(RX_GAIN, 3);

    boost::this_thread::sleep(boost::posix_time::seconds(2));

    std::cout << "devices initialized" << std::endl;
}


void hw_iface_impl::cal_rx_phase(float* delays_out)
{
    int num_avg = 32;
    int num_samps = 4096;
    int skip = 128;

    std::cout << "Rx Rate = " << this->usrp->get_rx_rate(0) << std::endl;
    std::cout << "Rx Freq = " << this->usrp->get_rx_freq(0) << std::endl;
    std::cout << "Rx Gain Ch. 0 = " << this->usrp->get_rx_gain(0) << std::endl;
    std::cout << "Rx Gain Ch. 1 = " << this->usrp->get_rx_gain(1) << std::endl;
    std::cout << "Rx Gain Ch. 2 = " << this->usrp->get_rx_gain(2) << std::endl;
    std::cout << "Rx Gain Ch. 3 = " << this->usrp->get_rx_gain(3) << std::endl;

    std::vector<void* > rx_buffers;
    rx_buffers.push_back(malloc(num_samps*sizeof(std::complex<float>)));
    rx_buffers.push_back(malloc(num_samps*sizeof(std::complex<float>)));
    rx_buffers.push_back(malloc(num_samps*sizeof(std::complex<float>)));
    rx_buffers.push_back(malloc(num_samps*sizeof(std::complex<float>)));

    uhd::rx_metadata_t meta;

    float rx_01_delay_avg = 0.0;
    float rx_02_delay_avg = 0.0;
    float rx_03_delay_avg = 0.0;

    for(int j = 0; j < num_avg; j++) {
        //very first ~50 samples are crap
        uhd::stream_cmd_t command(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
        command.stream_now = false;
        uhd::time_spec_t cmd_time = this->usrp->get_time_now() +
                                    uhd::time_spec_t(0.05);
        command.time_spec = cmd_time;
        command.num_samps = num_samps;
        this->rx_streamer->issue_stream_cmd(command);

        int ret = this->rx_streamer->recv(rx_buffers, num_samps, meta, 1.0);
        std::cout << "Run " << j << ". Got " << ret << " samples." << std::endl;

        std::complex<float>* rx_0_buffer =
          ((std::complex<float>*) rx_buffers[0]) + skip;
        std::complex<float>* rx_1_buffer =
          ((std::complex<float>*) rx_buffers[1]) + skip;
        std::complex<float>* rx_2_buffer =
          ((std::complex<float>*) rx_buffers[2]) + skip;
        std::complex<float>* rx_3_buffer =
          ((std::complex<float>*) rx_buffers[3]) + skip;

        int num = num_samps - skip;

        float xcorr_01[2*num - 1];
        float xcorr_02[2*num - 1];
        float xcorr_03[2*num - 1];

        std::stringstream filename;
        std::ofstream dump;
        filename << "./cal/rx_dump-" << j << ".csv";
        dump.open(filename.str());
        for(int i = 0; i < num; i++) {
          dump << std::real(rx_0_buffer[i]) << ";"
               << std::imag(rx_0_buffer[i]) << ";"
               << std::real(rx_1_buffer[i]) << ";"
               << std::imag(rx_1_buffer[i]) << ";"
               << std::real(rx_2_buffer[i]) << ";"
               << std::imag(rx_2_buffer[i]) << ";"
               << std::real(rx_3_buffer[i]) << ";"
               << std::imag(rx_3_buffer[i]) << ";"
               << std::endl;
        }
        dump.flush();
        dump.close();

        for(int n = 0; n < ((2*num) - 1); n++) {
          xcorr_01[n] = 0.0; 
          xcorr_02[n] = 0.0; 
          xcorr_03[n] = 0.0; 
          for(int m = 0; m < num; m++) {
            if((m+n >= num - 1) && (m+n < (2*num - 1))) {
              xcorr_01[n] += std::real(std::conj(rx_1_buffer[m]) * rx_0_buffer[(m + n)
                              - (num - 1)]);
              xcorr_02[n] += std::real(std::conj(rx_2_buffer[m]) * rx_0_buffer[(m + n)
                              - (num - 1)]);
              xcorr_03[n] += std::real(std::conj(rx_3_buffer[m]) * rx_0_buffer[(m + n)
                              - (num - 1)]);
            } 
          }
        }

        filename.str("");
        std::ofstream corr;
        filename << "./cal/corr-" << j << ".csv";
        corr.open(filename.str());
        for(int i = 0; i < 2*num-1; i++) {
          corr << xcorr_01[i] << ";"
               << xcorr_02[i] << ";"
               << xcorr_03[i] << ";"
               << std::endl;
        }
        corr.flush();
        corr.close();

        int rx_01_delay = std::max_element(xcorr_01, xcorr_01 + (2*num - 1))
                            - xcorr_01 - (num - 1);
        int rx_02_delay = std::max_element(xcorr_02, xcorr_02 + (2*num - 1))
                            - xcorr_02 - (num - 1);
        int rx_03_delay = std::max_element(xcorr_03, xcorr_03 + (2*num - 1))
                            - xcorr_03 - (num - 1);

        rx_01_delay_avg += rx_01_delay;
        rx_02_delay_avg += rx_02_delay;
        rx_03_delay_avg += rx_03_delay;
    }

    rx_01_delay_avg /= (float)num_avg;
    rx_02_delay_avg /= (float)num_avg;
    rx_03_delay_avg /= (float)num_avg;

    float f0 = 1e3;
    float fs = 1e6;
    std::cout << "###### RX Phase Cal Report ##########" << std::endl;
    std::cout << "Phase 01: " << ((rx_01_delay_avg * f0) / fs) * 360.0 << std::endl;
    std::cout << "Phase 02: " << ((rx_02_delay_avg * f0) / fs) * 360.0 << std::endl;
    std::cout << "Phase 03: " << ((rx_03_delay_avg * f0) / fs) * 360.0 << std::endl;
    std::cout << "###### RX Phase Cal Report End ######" << std::endl;
}

void hw_iface_impl::send_tx_cal_tones_async()
{
    if(this->tx_cal_thread) {
        std::cout << "Existing TX cal thread detected" << std::endl;
        return;
    }
    this->run_tx_cal_tones_loop = true;
    this->tx_cal_thread = new boost::thread(boost::bind(&hw_iface_impl::send_tx_cal_tones, this));
}

void hw_iface_impl::end_tx_cal_tones_async()
{
    if(!this->tx_cal_thread) {
        std::cout << "No TX cal thread to end" << std::endl;
        return;
    }

    this->run_tx_cal_tones_loop = false;
    this->tx_cal_thread->join();
    this->tx_cal_thread = 0;
}

void hw_iface_impl::send_tx_cal_tones()
{
    uhd::stream_args_t stream_args("fc32", "sc16");
    stream_args.channels = {0ul,1ul,2ul,3ul};
    this->tx_streamer = this->usrp->get_tx_stream(stream_args);

    std::vector<std::complex<float> > buffer(this->tx_streamer->get_max_num_samps() * 10);
    int buffer_len = this->tx_streamer->get_max_num_samps() * 10;
    for(int i = 0;i < buffer_len;i++) {
        buffer[i] = std::complex<float>(0.7f, 0.7f);
    }
    std::vector<std::complex<float>* > buffers(4, &buffer.front());

    uhd::tx_metadata_t meta;
    std::cout << "starting burst" << std::endl;
    meta.start_of_burst = true;
    meta.end_of_burst = false;
    meta.has_time_spec = true;
    meta.time_spec = this->usrp->get_time_now() +
                        uhd::time_spec_t(0.5);

    this->tx_streamer->send(buffers, buffer_len, meta);
    meta.start_of_burst = false;
    meta.end_of_burst = false;
    meta.has_time_spec = false;

    while(run_tx_cal_tones_loop) {
        this->tx_streamer->send(buffers, buffer_len, meta);
    }

    //send a mini EOB packet
    meta.end_of_burst = true;
    this->tx_streamer->send("", 0, meta);
}

/*
int hw_iface_impl::send(
        std::vector<std::complex<float>* > &data,
        int num_samples,
        bool start_of_burst,
        bool end_of_burst)
{
    uhd::tx_metadata_t meta;
    if(start_of_burst) {
        std::cout << this->tx_usrp->get_mboard_sensor("ref_locked", 0).to_pp_string().c_str() << std::endl;
        std::cout << this->tx_usrp->get_mboard_sensor("ref_locked", 1).to_pp_string().c_str() << std::endl;
        std::cout << "starting burst" << std::endl;
        //meta.start_of_burst = true;
        meta.start_of_burst = false;
        meta.has_time_spec = true;
        meta.end_of_burst = false;
        meta.time_spec = this->tx_usrp->get_time_now() +
                            uhd::time_spec_t(0.1);

    } else if(end_of_burst) {
        std::cout << "ending burst" << std::endl;
        meta.end_of_burst = true;
        meta.start_of_burst = true;
        meta.has_time_spec = true;
    } else {
        meta.end_of_burst = false;
        meta.start_of_burst = false;
        meta.has_time_spec = false;
    }

    return this->tx_streamer->send(data, num_samples, meta);
    //return this->tx_streamer->send(this->tx_buffers, this->tx_spp, meta);
}
*/
