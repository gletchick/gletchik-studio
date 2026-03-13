#pragma once

#include "core/processes/iprocess.h"
#include <string>
#include <vector>
#include <sys/types.h>

namespace gs {

    class nativeprocess : public iprocess {
    public:
        nativeprocess() = default;
        ~nativeprocess() override;

        nativeprocess(const nativeprocess&) = delete;
        nativeprocess& operator=(const nativeprocess&) = delete;

        bool start(const std::string& command, const std::vector<std::string>& args) override;
        void kill() override;
        bool isRunning() const override;

        std::string readAllStdout() override;
        std::string readAllStderr() override;

        int getExitCode() const override { return m_exitCode; }

    private:
        std::string readFromPipe(int fd);

        pid_t m_pid = -1;
        bool m_isRunning = false;

        int m_stdoutPipe[2] = {-1, -1};
        int m_stderrPipe[2] = {-1, -1};
        int m_exitCode = 0;
    };

} // namespace gs