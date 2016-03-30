#include <stdio.h>
#include <iostream>
#include "tx_iface.hpp"
#include "sim_tx.hpp"
#include "usrp_tx.hpp"
#include "sig_gen.hpp"

int main() {
    std::cout << "Running Test" << std::endl;

    //tx_iface *tx = new sim_tx();
    tx_iface *tx = new usrp_tx(3.7e9, 1e6);
  
    sig_gen *gen = new sig_gen();

    std::vector<std::complex<float>* > buffers(4, 0);

    std::complex<float>* buf0 = new std::complex<float>[1024];
    std::complex<float>* buf1 = new std::complex<float>[1024];
    std::complex<float>* buf2 = new std::complex<float>[1024];
    std::complex<float>* buf3 = new std::complex<float>[1024];

    buffers[0] = buf0;
    buffers[1] = buf1;
    buffers[2] = buf2;
    buffers[3] = buf3;

    gen->const_signal(buffers[0], 1024);
    gen->const_signal(buffers[1], 1024);
    gen->const_signal(buffers[2], 1024);
    gen->const_signal(buffers[3], 1024);

    //adjust channel phases:
    for(int i = 0; i < 1024; i++) {
        buf1[i] = buf1[i] * std::polar(1.01f, (((2.0f*(float)M_PI) / 360.0f) * 89.0f));
        buf2[i] = buf2[i] * std::polar(1.1f, (((2.0f*(float)M_PI) / 360.0f) * -32.0f));
        buf3[i] = buf3[i] * std::polar(1.1f, (((2.0f*(float)M_PI) / 360.0f) * 19.0f));
    }

    for(int i = 0; i < 10e6; i++) {
        if(i == 0) {
          tx->send(buffers, 1024, true, false);
        } else if(i == (10e6 - 1)) { 
          tx->send(buffers, 1024, false, true);
        } else {
          int send = tx->send(buffers, 1024, false, false);
        }
    }
}
