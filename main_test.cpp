#include <stdio.h>
#include <iostream>
#include "tx_iface.hpp"
#include "sim_tx.hpp"
#include "usrp_tx.hpp"
#include "sig_gen.hpp"

int main() {
    std::cout << "Running Test" << std::endl;

    tx_iface *tx = new sim_tx();
    sig_gen *gen = new sig_gen();

    std::vector<std::vector<std::complex<float> > > buffers;
    std::vector<std::complex<float> > buffer0;

    gen->complex_sine(buffer0, 1024, 0.001, 0.0);

    buffers.push_back(buffer0);
    tx->send(buffers, 1024, false, false);
}
