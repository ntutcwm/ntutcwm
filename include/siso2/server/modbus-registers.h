#ifndef SISO2_MODBUS_REGISTERS_H
#define SISO2_MODBUS_REGISTERS_H

#include <stdint.h>

namespace SISO2
{

struct ModbusRegisters
{
    __attribute__((aligned(2))) bool tx_enabled;    // 0
    __attribute__((aligned(2))) int16_t tx_gain;    // 1
    __attribute__((aligned(2))) uint64_t tx_freq;   // 2-5
    __attribute__((aligned(2))) uint32_t tx_rate;   // 6-7
    __attribute__((aligned(2))) uint32_t tx_bw;     // 8-9
    __attribute__((aligned(2))) bool rx_enabled;    // 10
    __attribute__((aligned(2))) int16_t rx_gain;    // 11
    __attribute__((aligned(2))) uint64_t rx_freq;   // 12-15
    __attribute__((aligned(2))) uint32_t rx_rate;   // 16-17
    __attribute__((aligned(2))) uint32_t rx_bw;     // 18-19
    __attribute__((aligned(2))) int16_t rssi;       // 20
    __attribute__((aligned(2))) int16_t snr;        // 21
    __attribute__((aligned(2))) int16_t cfo;        // 22
    __attribute__((aligned(2))) uint16_t n_rs_error;// 23
    __attribute__((aligned(2))) uint16_t n_rs_ok;   // 24
    __attribute__((aligned(2))) uint16_t tx_duty_cycle; // 25
    __attribute__((aligned(2))) uint32_t n_cached;  // 26-27
    __attribute__((aligned(2))) uint16_t rx_agc;    // 28
    __attribute__((aligned(2))) uint16_t rx_decode_rate; // 29
    __attribute__((aligned(2))) int16_t temperature; //30
} __attribute__((packed, aligned(2)));

}

#endif