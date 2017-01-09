#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <complex>

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
    std::complex<float> tx_adj[4] = {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}};

    tx_adj[0] = tx_adj[0] * 0.5f;
    tx_adj[1] = tx_adj[1] * 0.5f;
    tx_adj[2] = tx_adj[2] * 0.5f; // * 1.028818f;
    tx_adj[3] = tx_adj[3] * 0.5f;

    hw->cal_rx_channels(rx_cal, false);
    std::cout << "RX path calibrated." << std::endl;

    hw->send_tx_cal_tones_async(tx_adj);
    boost::this_thread::sleep(boost::posix_time::seconds(10));

    hw->cal_rx_channels(tx_cal, true);

    boost::this_thread::sleep(boost::posix_time::seconds(1));
    hw->end_tx_cal_tones_async();
    boost::this_thread::sleep(boost::posix_time::seconds(1));

    std::cout << "TX path calibrated." << std::endl;


    std::cout << "Transmitting calibrated Tx signal" << std::endl;
    float h_ref_02_mag = std::abs(rx_cal[1]);
    float h_tx_02_mag = std::abs(tx_cal[1]);
    float scale02 = (h_tx_02_mag / h_ref_02_mag / 0.9394f);
    std::cout << "h_ref = " << h_ref_02_mag << std::endl
              << "h_tx = " << h_tx_02_mag << std::endl
              << "scale = " << scale02 << std::endl;

    tx_adj[2] = tx_adj[2] / scale02;

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
