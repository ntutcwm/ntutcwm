#ifndef SISO2_WS_SERVER_H
#define SISO2_WS_SERVER_H

#include <ixwebsocket/IXHttpServer.h>
#include <stdint.h>
#include <memory>
#include <string>
#include <set>
#include <map>

#include "siso2/dev/iq-device.h"
#include "siso2/transmitter.h"
#include "siso2/receiver.h"

namespace SISO2
{

class WsServer
{
    uint16_t port;
    std::string rootDir;
    std::map<std::string, std::string> assets;

    std::unique_ptr<ix::HttpServer> server;
    std::set<ix::WebSocket*> connections;

    std::shared_ptr<IqDevice> iq_device;
    std::shared_ptr<Transmitter> transmitter;
    std::shared_ptr<Receiver> receiver; 

    ix::HttpResponsePtr fileResponse(std::string path);

    void onMessage(ix::WebSocket* connection, std::string message);
public:
    WsServer(uint16_t port, std::string rootDir, decltype(iq_device) iq_device, decltype(transmitter) transmitter, decltype(receiver) receiver);
    void start();
    void wait();
    void stop();
    void broadcast_device_info();
    void broadcast(Transmitter::Stats tx_stats);
    void broadcast(Receiver::Stats rx_stats);
    void broadcast_spectrum(std::complex<int32_t>* data, size_t len);
    void broadcast_constellation(std::vector<std::complex<float>> data);
};

}

#endif
