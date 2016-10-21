#ifndef CONFIG_H
#define CONFIG_H

#define IP_USRP_0 "192.168.10.2"
#define IP_USRP_1 "192.168.10.3"

#define TX_GAIN 5
#define RX_GAIN 15

//lp = remez(128, [0 0.04 0.1 1], [1.0, 1.0, 0.000001, 0.000001]);
extern const double tx_rx_cal_lp[];

#endif
