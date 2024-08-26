#ifndef SISO2_XILINX_AXIDMA_H
#define SISO2_XILINX_AXIDMA_H

extern "C" {
#include <libaxidma.h>
}
#include <atomic>
#include <functional>

namespace SISO2
{

class XilinxAXIDMA
{
protected:
    int input_channel;
    int output_channel;

    std::atomic_flag transfer_done;

    void callback(int channel_id, void *data)
    {
        if (channel_id == output_channel) {
            transfer_done.test_and_set();
            transfer_done.notify_all();
        }
    }

    XilinxAXIDMA(int input_channel, int output_channel): input_channel(input_channel), output_channel(output_channel)
    {
        if (!device) device = axidma_init();
        axidma_set_callback(device, output_channel, [](int channel_id, void* data) {
            auto axidma = (XilinxAXIDMA*)data;
            if (channel_id == axidma->output_channel) {
                axidma->transfer_done.test_and_set();
                axidma->transfer_done.notify_all();
            }
        }, this);
    }

    ~XilinxAXIDMA()
    {
        // axidma_destroy(device);
    }

public:
    static inline axidma_dev_t device;
};

}

#endif