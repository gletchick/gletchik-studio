#pragma once

#include "core/processes/iprocess.h"
#include <string>
#include <vector>
#include <sys/types.h>

namespace gs {

    class NativeProcess : public IProcess {
    public:
        NativeProcess() = default;
        ~NativeProcess() override;

        NativeProcess(const NativeProcess&) = delete;
        NativeProcess& operator=(const NativeProcess&) = delete;

        bool start(const std::string& command, const std::vector<std::string>& args) override;
        void kill() override;
        bool isRunning() const override;

        std::string readAllStdout() override;
        std::string readAllStderr() override;
        bool writeToStdin(const std::string& input);

        int getExitCode() const override { return m_exitCode; }

    private:
        std::string readFromPipe(int fd);

        pid_t m_pid = -1;
        bool m_isRunning = false;

        int m_stdoutPipe[2] = {-1, -1};
        int m_stderrPipe[2] = {-1, -1};
        int m_stdinPipe[2] = {-1, -1};

        int m_exitCode = 0;
    };

} // namespace gs