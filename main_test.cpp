#include <stdio.h>
#include <iostream>
#include "hw_iface.hpp"
#include "hw_iface_impl.hpp"
#include "sig_gen.hpp"
#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>

int UHD_SAFE_MAIN(int argc, char *argv[]){
    uhd::set_thread_priority_safe();

    std::cout << "Running Test" << std::endl;

    hw_iface *hw = new hw_iface_impl();
  
    sig_gen *gen = new sig_gen();


    hw->send_tx_cal_tones_async();

    for(int i = 0; i < 1024; i++) {
       hw->cal_rx_rf_phase(4096);
       boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
    }

    hw->end_tx_cal_tones_async();
}
