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
    hw->cal_rx_phase(0);
}
