#ifndef SISO2_PROCESS_H
#define SISO2_PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace SISO2
{

class Process
{
    pid_t pid; // process id for child

protected:
    int in_pipe[2];
    int out_pipe[2];
    int out2_pipe[2];

public:
    Process(std::string process_path, std::string process_name, int* in_pipe, int* out_pipe, int* out2_pipe) {
        this->in_pipe[0] = in_pipe[0];
        this->in_pipe[1] = in_pipe[1];
        this->out_pipe[0] = out_pipe[0];
        this->out_pipe[1] = out_pipe[1];
        this->out2_pipe[0] = out2_pipe[0];
        this->out2_pipe[1] = out2_pipe[1];
        // fork a child process
        pid = fork();
        if (pid == -1) {
            perror("fork");
        }
        if (pid == 0) { // child process
            dup2(in_pipe[0], STDIN_FILENO); // duplicate the read end of the pipe to stdin
            dup2(out_pipe[1], STDOUT_FILENO); // duplicate the write end of the pipe to stdout
            dup2(out2_pipe[1], 3); // duplicate the write end of the pipe to stdout
            execlp(process_path.c_str(), process_name.c_str(), NULL);
            perror(("execlp: " + process_path).c_str()); // if execlp returns, there was an error
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

