#ifndef SISO2_MODBUS_SERVER_H
#define SISO2_MODBUS_SERVER_H

#include <stdint.h>
#include <memory>
#include <string>
#include <set>
#include <map>
#include <future>
#include "siso2/dev/iq-device.h"
#include "siso2/transmitter.h"
#include "siso2/receiver.h"
#include "siso2/server/ws-server.h"
#include "siso2/server/modbus-registers.h"

namespace SISO2
{

class ModbusServer
{
    std::shared_ptr<WsServer> ws_server;
    std::shared_ptr<IqDevice> iq_device;
    std::shared_ptr<Transmitter> transmitter;
    std::shared_ptr<Receiver> receiver;

    std::future<void> tcp_task, rtu_task;

    ModbusRegisters get_registers();

    void write(const ModbusRegisters& new_registers);

    Transmitter::Stats tx_stats = {};
    Receiver::Stats rx_stats = {};

    uint32_t* io_out = nullptr;

public:
    ModbusServer(decltype(ws_server) ws_server, decltype(iq_device) iq_device, decltype(transmitter) transmitter, decltype(receiver) receiver);
    void start();
    void update(Transmitter::Stats tx_stats);
    void update(Receiver::Stats rx_stats);
};

}

#endif
