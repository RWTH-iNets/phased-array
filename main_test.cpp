#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>

#include "hw_iface_impl.hpp"

void rx_cal_test()
{
    hw_iface *hw = new hw_iface_impl();

    std::complex<float> rx_cal[3];
    hw->cal_rx_channels(rx_cal, false);
}

void rx_tx_cal_test()
{
    hw_iface *hw = new hw_iface_impl();

    std::complex<float> rx_cal[3];
    std::complex<float> tx_cal[3];
    std::complex<float> tx_adj[4] = {{1.0f, 0.0}, {1.0f, 0.0f}, {1.0f, 0.0}, {1.0f, 0.0}};

    hw->cal_rx_channels(rx_cal, false);
    std::cout << "RX path calibrated." << std::endl;

    hw->send_tx_cal_tones_async(tx_adj);
    boost::this_thread::sleep(boost::posix_time::seconds(5));

    hw->cal_rx_channels(tx_cal, true);

    boost::this_thread::sleep(boost::posix_time::seconds(5));
    hw->end_tx_cal_tones_async();
    boost::this_thread::sleep(boost::posix_time::seconds(5));

    std::cout << "TX path calibrated." << std::endl;

    std::cout << "Transmitting calibrated Tx signal" << std::endl;

    tx_adj[1] = std::conj((tx_cal[0] / rx_cal[0]));
    tx_adj[2] = std::conj((tx_cal[1] / rx_cal[1]));
    tx_adj[3] = std::conj((tx_cal[2] / rx_cal[2]));

    hw->send_tx_cal_tones_async(tx_adj);
    boost::this_thread::sleep(boost::posix_time::seconds(60));
    hw->end_tx_cal_tones_async();
    boost::this_thread::sleep(boost::posix_time::seconds(5));
}

int UHD_SAFE_MAIN(int argc, char *argv[]){
    uhd::set_thread_priority_safe();

    std::cout << "Running Test" << std::endl;
    rx_tx_cal_test();
}
