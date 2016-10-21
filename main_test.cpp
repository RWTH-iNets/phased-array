#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>

#include "hw_iface_impl.hpp"

void rx_cal_test()
{
    hw_iface *hw = new hw_iface_impl();

    float rx_phases[3] = {0.0, 0.0, 0.0};
    hw->cal_rx_phase(rx_phases, false);
}

void rx_tx_cal_test()
{
    hw_iface *hw = new hw_iface_impl();

    float rx_phases[3] = {0.0, 0.0, 0.0};
    float tx_phases[3] = {0.0, 0.0, 0.0};
    float tx_phase_adj[4] = {0.0, 0.0, 0.0, 0.0};

    hw->cal_rx_phase(rx_phases, false);
    std::cout << "RX path calibrated." << std::endl;

    boost::this_thread::sleep(boost::posix_time::seconds(5));

    hw->send_tx_cal_tones_async(tx_phase_adj);

    boost::this_thread::sleep(boost::posix_time::seconds(5));

    hw->cal_rx_phase(tx_phases, true);

    hw->end_tx_cal_tones_async();
    std::cout << "TX path calibrated." << std::endl;

    boost::this_thread::sleep(boost::posix_time::seconds(20));

    std::cout << "Transmitting calibrated Tx signal" << std::endl;
   
    tx_phase_adj[0] = 0.0; 
    tx_phase_adj[1] = tx_phases[0] - rx_phases[0];
    tx_phase_adj[2] = tx_phases[1] - rx_phases[1];
    tx_phase_adj[3] = tx_phases[2] - rx_phases[2];

    std::cout << "alphas: 01 = " << tx_phase_adj[1]
              << ", 02 = " << tx_phase_adj[2]
              << ", 03 = " << tx_phase_adj[3]
              << std::endl;

    hw->send_tx_cal_tones_async(tx_phase_adj);
    boost::this_thread::sleep(boost::posix_time::seconds(20));
    hw->end_tx_cal_tones_async();
    boost::this_thread::sleep(boost::posix_time::seconds(20));
}

int UHD_SAFE_MAIN(int argc, char *argv[]){
    uhd::set_thread_priority_safe();

    std::cout << "Running Test" << std::endl;
    rx_tx_cal_test();
}
