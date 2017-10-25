#ifndef HW_IFACE_HPP
#define HW_IFACE_HPP

#include <complex>
#include <vector>

class hw_iface
{
    public:
        virtual void tx_const_async(std::complex<float>* cal) = 0;
        virtual void end_tx_const_async() = 0;
        virtual void tx_const(std::complex<float>* cal) = 0;
        virtual void cal_rx(std::complex<float>* cal_out, bool tx_ref) = 0;
};

#endif
