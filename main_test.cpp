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

    std::vector<std::complex<float> > buffer0(10024);
    std::vector<std::complex<float> > buffer1(10024);
    std::vector<std::complex<float> > buffer2(10024);
    std::vector<std::complex<float> > buffer3(10024);
    std::vector<std::complex<float>* > buffers(4, &buffer0.front());
    buffers[0] = &buffer0.front();
    buffers[1] = &buffer1.front();
    buffers[2] = &buffer2.front();
    buffers[3] = &buffer3.front();

    gen->const_signal(buffer0, 10024);
    gen->const_signal(buffer1, 10024);
    gen->const_signal(buffer2, 10024);
    gen->const_signal(buffer3, 10024);

    //adjust channel phases:
    for(int i = 0; i < 1024; i++) {
        buffers[0][i] = buffers[0][i] * std::polar(1.0f, -0.8f * 2.0f * (float)M_PI);
        buffers[1][i] = buffers[1][i] * std::polar(1.0f);//this->phase_cal[1];
        buffers[2][i] = buffers[2][i] * std::polar(1.0f);//this->phase_cal[2];
        buffers[3][i] = buffers[3][i] * std::polar(1.0f);//this->phase_cal[3];
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
