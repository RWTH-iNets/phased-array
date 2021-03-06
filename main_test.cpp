#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <complex>

#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>

#include "hw_iface_impl.hpp"
#include "config.hpp"

void rx_cal_test()
{
    hw_iface *hw = new hw_iface_impl();

    std::complex<float> rx_cal[3];
    hw->cal_rx(rx_cal, false);
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

    hw->cal_rx(rx_cal, false);
    std::cout << "RX path calibrated." << std::endl;

    hw->tx_const_async(tx_adj);
    boost::this_thread::sleep(boost::posix_time::seconds(10));

    hw->cal_rx(tx_cal, true);
    boost::this_thread::sleep(boost::posix_time::seconds(1));
    hw->end_tx_const_async();
    boost::this_thread::sleep(boost::posix_time::seconds(1));

    std::cout << "TX path calibrated." << std::endl;

    std::cout << "Transmitting calibrated Tx signal" << std::endl;
    
    std::complex<float> tx_scale_01;
    tx_scale_01 = tx_cal[0] / rx_cal[0] / ct_01;
    tx_adj[1] = (tx_adj[1] / tx_scale_01);
    std::complex<float> tx_scale_02;
    tx_scale_02 = tx_cal[1] / rx_cal[1] / ct_02;
    tx_adj[2] = (tx_adj[2] / tx_scale_02);
    std::complex<float> tx_scale_03;
    tx_scale_03 = tx_cal[2] / rx_cal[2] / ct_03;
    tx_adj[3] = (tx_adj[3] / tx_scale_03);

    hw->tx_const_async(tx_adj);
    boost::this_thread::sleep(boost::posix_time::seconds(30));
    hw->end_tx_const_async();
    boost::this_thread::sleep(boost::posix_time::seconds(5));

    //steer 9deg to boresight
    tx_adj[0] = (tx_adj[0] * std::polar(1.0f, (float)(-90.0f*(M_PI / 180.0f))));
    tx_adj[1] = (tx_adj[1] * std::polar(1.0f, (float)(-30.0f*(M_PI / 180.0f))));
    tx_adj[2] = (tx_adj[2] * std::polar(1.0f, (float)(30.0f*(M_PI / 180.0f))));
    tx_adj[3] = (tx_adj[3] * std::polar(1.0f, (float)(90.0f*(M_PI / 180.0f))));
    hw->tx_const_async(tx_adj);
    boost::this_thread::sleep(boost::posix_time::seconds(300));
    hw->end_tx_const_async();
}

int UHD_SAFE_MAIN(int argc, char *argv[]){
    uhd::set_thread_priority_safe();

    std::cout << "Running Test" << std::endl;
    rx_tx_cal_test();
}
