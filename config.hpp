#ifndef CONFIG_H
#define CONFIG_H

#include <complex>

#define IP_USRP_0 "192.168.10.2"
#define IP_USRP_1 "192.168.10.3"

#define TX_GAIN 15
#define RX_GAIN 15

// Enter the measured phase differences (in rad)
// between the TX outputs of the system
// when generating time aligned sine signals.
#define P_TX_01 0
#define P_TX_02 0
#define P_TX_03 0

//lp = remez(128, [0 0.04 0.1 1], [1.0, 1.0, 0.000001, 0.000001]);
extern const double tx_rx_cal_lp[];
extern const std::complex<float> ct_01;
extern const std::complex<float> ct_02;
extern const std::complex<float> ct_03;

#endif
