#ifndef HW_IFACE_HPP
#define HW_IFACE_HPP

#include <complex>
#include <vector>

class hw_iface
{
    public:
        virtual void send_tx_cal_tones_async(float* tx_phases) = 0;
        virtual void end_tx_cal_tones_async() = 0;
        virtual void send_tx_cal_tones(float* tx_phases) = 0;
        virtual void cal_rx_phase(float* phase_diffs_out, bool tx_ref) = 0;
};

#endif
