#ifndef SISO2_TESTBENCH_H
#define SISO2_TESTBENCH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace SISO2
{

class Testbench
{
    pid_t pid; // process id for child

protected:
    int in_pipe[2];
    int out_pipe[2];

public:
    Testbench(std::string ip_name, int* in_pipe, int* out_pipe) {
        this->in_pipe[0] = in_pipe[0];
        this->in_pipe[1] = in_pipe[1];
        this->out_pipe[0] = out_pipe[0];
        this->out_pipe[1] = out_pipe[1];
        // fork a child process
        pid = fork();
        if (pid == -1) {
            perror("fork");
        }
        if (pid == 0) { // child process
            dup2(in_pipe[0], STDIN_FILENO); // duplicate the read end of the pipe to stdin
            dup2(out_pipe[1], STDOUT_FILENO); // duplicate the write end of the pipe to stdout
            setenv("LD_LIBRARY_PATH", 
                "/tools/Xilinx/Vivado/2023.2/lib/lnx64.o/Default"
                ":/tools/Xilinx/Vitis_HLS/2023.2/lnx64/tools/fpo_v7_0"
                ":/tools/Xilinx/Vitis_HLS/2023.2/lnx64/tools/fpo_v7_1"
                ":/tools/Xilinx/Vitis_HLS/2023.2/lnx64/tools/fir_v7_0",
            false);
            execlp(("ip/" + ip_name + "/" + ip_name + ".prj/AXI4-Stream/csim/build/csim.exe").c_str(), "csim.exe", "-p", NULL);
            perror(("execlp: " + ip_name).c_str()); // if execlp returns, there was an error
            abort();
        }
    }

    int write(const void* in, size_t size) const
    {
        int n = ::write(in_pipe[1], in, size);
        return n;
    }

    int read(void* out, size_t size) const
    {
        size_t bytes_transferred = 0;
        while (bytes_transferred < size) {
            size_t n_bytes = ::read(out_pipe[0], (uint8_t*)out+bytes_transferred, size - bytes_transferred);
            if (n_bytes > 0) bytes_transferred += n_bytes;
        }
        return bytes_transferred;
    }
};

}

#endif

