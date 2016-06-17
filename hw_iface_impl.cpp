#include "hw_iface_impl.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/assign/list_of.hpp> // for 'list_of()'
#include <boost/assert.hpp> 
#include "filter.hpp"

hw_iface_impl::hw_iface_impl():ref_sig_num_samps_per_periode(1024), tx_cal_thread(0),
                                rx_cal_data{std::polar(1.0f, 0.0f),
                                            std::polar(1.0f, (float)M_PI*(57.6f / 180.0f)),
                                            std::polar(1.0f, (float)M_PI*(-6.6f / 180.0f)),
                                            std::polar(1.0f, (float)M_PI*(0.9f / 180.0f))},
                                rx_phase_alpha(0.85f),
                                // RC filter, tau = xx samples (alpha = exp(-1/tau))
                                ofs("4ChanRxPhaseStability.txt", std::ofstream::trunc)
{

    for( int i = 0; i < ref_sig_num_samps_per_periode; i++) {
        sine_table.push_back(std::polar(1.0f,
            (float)(((2.0f*M_PI) / (float)ref_sig_num_samps_per_periode) * i)));
    }
    std::cout << "RX cal data: " << rx_cal_data[1] << ", "
              << rx_cal_data[2] << ", " << rx_cal_data[3] << std::endl; 

    //create a tx_usrp device
    uhd::device_addr_t addr_args;
    addr_args["addr0"] = IP_USRP_0;
    addr_args["addr1"] = IP_USRP_1;

    this->tx_usrp = uhd::usrp::multi_usrp::make(addr_args);
    this->rx_usrp = uhd::usrp::multi_usrp::make(addr_args);

    //set subdev spec on all mboards.
    //use TX/RX ports on both dboards.
    this->tx_usrp->set_tx_subdev_spec(uhd::usrp::subdev_spec_t("A:0 B:0"));
    this->rx_usrp->set_rx_subdev_spec(uhd::usrp::subdev_spec_t("A:0 B:0"));
    this->rx_usrp->set_rx_antenna("RX2", 0);
    this->rx_usrp->set_rx_antenna("RX2", 1);
    this->rx_usrp->set_rx_antenna("RX2", 2);
    this->rx_usrp->set_rx_antenna("RX2", 3);
    this->tx_usrp->set_tx_antenna("TX/RX", 0);
    this->tx_usrp->set_tx_antenna("TX/RX", 1);
    this->tx_usrp->set_tx_antenna("TX/RX", 2);
    this->tx_usrp->set_tx_antenna("TX/RX", 3);

    this->tx_usrp->set_clock_source("external"); //applies to all mboards
    this->tx_usrp->set_time_source("external"); //applies to all mboards
    //this->rx_usrp->set_clock_source("external"); //applies to all mboards
    //this->rx_usrp->set_time_source("external"); //applies to all mboards

    this->tx_usrp->set_tx_rate(1e6);
    this->rx_usrp->set_rx_rate(1e6);

    this->tx_usrp->set_tx_gain(TX_GAIN, 0);
    this->tx_usrp->set_tx_gain(TX_GAIN, 1);
    this->tx_usrp->set_tx_gain(TX_GAIN, 2);
    this->tx_usrp->set_tx_gain(TX_GAIN, 3);

    this->rx_usrp->set_rx_gain(RX_GAIN, 0);
    this->rx_usrp->set_rx_gain(RX_GAIN, 1);
    this->rx_usrp->set_rx_gain(RX_GAIN, 2);
    this->rx_usrp->set_rx_gain(RX_GAIN, 3);

    double tx_freq = 3.7001e9;
    double rx_freq = 3.7e9;
    //Set time at next PPS to zero
    //Convenience function. Waits for PPS on one device and then
    //sets time at next PPS on all devices
    this->tx_usrp->set_time_unknown_pps(uhd::time_spec_t(0.0));
    boost::this_thread::sleep(boost::posix_time::seconds(2));
    
    //we will tune the frontends in 500ms from now
    std::cout << "tuning all frontends to " << tx_freq << std::endl;
    uhd::time_spec_t cmd_time = tx_usrp->get_time_now() +
        uhd::time_spec_t(0.2);
    //sets command time on all devices
    //the next commands are all timed
    this->tx_usrp->set_command_time(cmd_time);
    this->rx_usrp->set_command_time(cmd_time);

    this->tx_usrp->set_tx_freq(tx_freq, 0);
    this->tx_usrp->set_tx_freq(tx_freq, 1);
    this->tx_usrp->set_tx_freq(tx_freq, 2);
    this->tx_usrp->set_tx_freq(tx_freq, 3);
    this->rx_usrp->set_rx_freq(rx_freq, 0);
    this->rx_usrp->set_rx_freq(rx_freq, 1);
    this->rx_usrp->set_rx_freq(rx_freq, 2);
    this->rx_usrp->set_rx_freq(rx_freq, 3);

    //end timed commands
    this->tx_usrp->clear_command_time();
    this->tx_usrp->clear_command_time();

    boost::this_thread::sleep(boost::posix_time::seconds(2));

    std::cout << "devices initialized" << std::endl;

    uhd::stream_args_t stream_args_rx("fc32", "sc16");
    stream_args_rx.channels = {0ul,1ul,2ul,3ul};
    this->rx_streamer = this->rx_usrp->get_rx_stream(stream_args_rx);

    //this->tx_spp = this->tx_streamer->get_max_num_samps();
    //std::cout << "TX SPP = " << this->tx_spp << std::endl;

}

void hw_iface_impl::cal_rx_rf_phase(int num_samps)
{
    std::vector<void* > rx_buffers;
    rx_buffers.push_back(malloc(num_samps*sizeof(std::complex<float>)));
    rx_buffers.push_back(malloc(num_samps*sizeof(std::complex<float>)));
    rx_buffers.push_back(malloc(num_samps*sizeof(std::complex<float>)));
    rx_buffers.push_back(malloc(num_samps*sizeof(std::complex<float>)));

    uhd::rx_metadata_t meta;
    float rx_01_phi_avg = std::arg(rx_cal_data[1]);
    float rx_02_phi_avg = std::arg(rx_cal_data[2]);
    float rx_03_phi_avg = std::arg(rx_cal_data[3]);
    for(int j = 0; j < 64; j++) {
        uhd::stream_cmd_t command(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
        //uhd::stream_cmd_t command(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_MORE);

        command.num_samps = num_samps;
        command.stream_now = false;

        uhd::time_spec_t cmd_time = this->rx_usrp->get_time_now() +
            uhd::time_spec_t(0.1);
        command.time_spec = cmd_time;

        this->rx_streamer->issue_stream_cmd(command);
        this->rx_streamer->recv(rx_buffers, num_samps, meta, 1.0);
        //command.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
        //command.stream_now = true;
        //this->rx_streamer->issue_stream_cmd(command);
        //std::cout << "Got 1024 samples from each device" << std::endl;
        //std::cout << "Calculating phase offsets ..." << std::endl;

        float rx_01_phi = 0.0; 
        float rx_02_phi = 0.0; 
        float rx_03_phi = 0.0; 

        float rx_0_mag = 0.0; 
        float rx_1_mag = 0.0; 
        float rx_2_mag = 0.0; 
        float rx_3_mag = 0.0; 

        std::complex<float> rx_01_phasor_sum; 
        std::complex<float> rx_02_phasor_sum; 
        std::complex<float> rx_03_phasor_sum; 

        std::complex<float>* rx_0_buffer = (std::complex<float>*) rx_buffers[0];
        std::complex<float>* rx_1_buffer = (std::complex<float>*) rx_buffers[1];
        std::complex<float>* rx_2_buffer = (std::complex<float>*) rx_buffers[2];
        std::complex<float>* rx_3_buffer = (std::complex<float>*) rx_buffers[3];

        std::ofstream rx0("rx0.txt", std::ofstream::trunc);

        for(int i = 0; i < num_samps - 75; i++) {

            //Do FIR filtering to split RX ref and TX signals
            std::complex<double> lp0 = 0.0;
            std::complex<double> lp1 = 0.0;
            std::complex<double> lp2 = 0.0;
            std::complex<double> lp3 = 0.0;

            std::complex<double> hp0 = 0.0;
            std::complex<double> hp1 = 0.0;
            std::complex<double> hp2 = 0.0;
            std::complex<double> hp3 = 0.0;

            for(int j = 0; j < 75; j++) {
                lp0 += filter::fir_lp_014_75[j] * rx_0_buffer[i + j];
                lp1 += filter::fir_lp_014_75[j] * rx_1_buffer[i + j];
                lp2 += filter::fir_lp_014_75[j] * rx_2_buffer[i + j];
                lp3 += filter::fir_lp_014_75[j] * rx_3_buffer[i + j];

                hp0 += filter::fir_hp_016_75[j] * rx_0_buffer[i + j];
                hp1 += filter::fir_hp_016_75[j] * rx_1_buffer[i + j];
                hp2 += filter::fir_hp_016_75[j] * rx_2_buffer[i + j];
                hp3 += filter::fir_hp_016_75[j] * rx_3_buffer[i + j];
            }
            
            rx0 << i << ";" 
                << std::real(lp0) << ";" << std::imag(lp0)
                << std::real(hp0) << ";" << std::imag(hp0)
                << std::real(lp1) << ";" << std::imag(lp1)
                << std::real(hp1) << ";" << std::imag(hp1)
                << ";" << std::endl;
            /*
            rx0 << i << ";" << std::real(rx_0_buffer[i])
                << ";" << std::imag(rx_0_buffer[i])
                << ";" << std::endl;
            */ 
            //rx_01_phasor_sum += ((rx_1_buffer[i] / rx_cal_data[1]) / rx_0_buffer[i]);
            //rx_02_phasor_sum += ((rx_2_buffer[i] / rx_cal_data[2]) / rx_0_buffer[i]);
            //rx_03_phasor_sum += ((rx_3_buffer[i] / rx_cal_data[3]) / rx_0_buffer[i]);
            /*             
            rx_01_phasor_sum += ((rx_1_buffer[i]) / rx_0_buffer[i]);
            rx_02_phasor_sum += ((rx_2_buffer[i]) / rx_0_buffer[i]);
            rx_03_phasor_sum += ((rx_3_buffer[i]) / rx_0_buffer[i]);
            rx_0_mag += std::abs(rx_0_buffer[i]);
            rx_1_mag += std::abs(rx_1_buffer[i]);
            rx_2_mag += std::abs(rx_2_buffer[i]);
            rx_3_mag += std::abs(rx_3_buffer[i]);
            */
        }

        rx0.close();
        /*
        rx_0_mag /= num_samps;
        rx_1_mag /= num_samps;
        rx_2_mag /= num_samps;
        rx_3_mag /= num_samps;
     
        rx_01_phi = std::arg(rx_01_phasor_sum);
        rx_02_phi = std::arg(rx_02_phasor_sum);
        rx_03_phi = std::arg(rx_03_phasor_sum);

        if( rx_01_phi != rx_01_phi || rx_02_phi != rx_02_phi || rx_03_phi != rx_03_phi ) {
            std::cout << "NaN detected" << std::endl;
        } else {
            rx_01_phi_avg = rx_phase_alpha*rx_01_phi_avg + (1.0f - rx_phase_alpha) * rx_01_phi;
            rx_02_phi_avg = rx_phase_alpha*rx_02_phi_avg + (1.0f - rx_phase_alpha) * rx_02_phi;
            rx_03_phi_avg = rx_phase_alpha*rx_03_phi_avg + (1.0f - rx_phase_alpha) * rx_03_phi;
        }

        std::cout << "###### RX RF Phase Cal Report ##########" << std::endl;
        std::cout << "Phi 01: " << rx_01_phi << std::endl;
        std::cout << "Phi 02: " << rx_02_phi << std::endl;
        std::cout << "Phi 03: " << rx_03_phi << std::endl;
        std::cout << "###### RX RF Phase Cal Report End ######" << std::endl;

        ofs << tx_usrp->get_time_now().get_full_secs() + tx_usrp->get_time_now().get_frac_secs()
            << ";" << rx_01_phi
            << ";" << rx_02_phi
            << ";" << rx_03_phi
            << ";" << rx_01_phi_avg
            << ";" << rx_02_phi_avg
            << ";" << rx_03_phi_avg
            << ";" << rx_01_phi - rx_01_phi_avg
            << ";" << rx_02_phi - rx_02_phi_avg
            << ";" << rx_03_phi - rx_03_phi_avg
            << ";" << rx_0_mag
            << ";" << rx_1_mag
            << ";" << rx_2_mag
            << ";" << rx_3_mag
            << ";" << std::endl;
        */
        boost::this_thread::sleep(boost::posix_time::milliseconds(200));
    }

    rx_cal_data[1] = std::polar(1.0f, rx_01_phi_avg);
    rx_cal_data[2] = std::polar(1.0f, rx_02_phi_avg);
    rx_cal_data[3] = std::polar(1.0f, rx_03_phi_avg);

    //ofs.close();
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
    this->tx_streamer = this->tx_usrp->get_tx_stream(stream_args);

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
    meta.time_spec = this->tx_usrp->get_time_now() +
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
