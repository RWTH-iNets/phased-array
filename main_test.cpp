#include <stdio.h>
#include <stdlib.h>
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

    float rx_phases[3] = {0.0, 0.0, 0.0};
    hw->cal_rx_phase(rx_phases);
    std::cout << "Calibrated Rx phases. Now turn off ref signal." << std::endl;
    boost::this_thread::sleep(boost::posix_time::seconds(10));

    std::cout << "Calibrating Tx phases." << std::endl;
    float tx_phases[3] = {0.0, 0.0, 0.0};
    float tx_phase_adj[4] = {0.0, 0.0, 0.0, 0.0};

    hw->send_tx_cal_tones_async(tx_phase_adj);
    boost::this_thread::sleep(boost::posix_time::seconds(5));
    hw->cal_rx_phase(tx_phases);
    hw->end_tx_cal_tones_async();

    boost::this_thread::sleep(boost::posix_time::seconds(10));

    std::cout << "Transmitting calibrated Tx signal" << std::endl;
   
    tx_phase_adj[0] = 0.0; 
    tx_phase_adj[1] = -(tx_phases[0] - rx_phases[0]);
    tx_phase_adj[2] = -(tx_phases[1] - rx_phases[1]);
    tx_phase_adj[3] = -(tx_phases[2] - rx_phases[2]);

    std::cout << "alphas: 01 = " << tx_phase_adj[1]
              << ", 02 = " << tx_phase_adj[2]
              << ", 03 = " << tx_phase_adj[3]
              << std::endl;

    hw->send_tx_cal_tones_async(tx_phase_adj);

    int i = 0;
    while(i < 100) {
      boost::this_thread::sleep(boost::posix_time::seconds(10));
    }
}
