#include "include/core/processes/NativeProcess.h"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <vector>

namespace gs {

    NativeProcess::~NativeProcess() {
        kill();
    }

    bool NativeProcess::start(const std::string& command, const std::vector<std::string>& args) {
        if (m_isRunning) return false;

        if (pipe(m_stdoutPipe) == -1 || pipe(m_stderrPipe) == -1) {
            return false;
        }

        m_pid = fork();

        if (m_pid < 0) {
            return false;
        }

        if (m_pid == 0) {
            close(m_stdoutPipe[0]);
            close(m_stderrPipe[0]);

            dup2(m_stdoutPipe[1], STDOUT_FILENO);
            dup2(m_stderrPipe[1], STDERR_FILENO);

            std::vector<char*> c_args;
            c_args.push_back(const_cast<char*>(command.c_str()));
            for (const auto& arg : args) {
                c_args.push_back(const_cast<char*>(arg.c_str()));
            }
            c_args.push_back(nullptr);

            execvp(command.c_str(), c_args.data());
            exit(1);
        }

        close(m_stdoutPipe[1]);
        close(m_stderrPipe[1]);

        fcntl(m_stdoutPipe[0], F_SETFL, O_NONBLOCK);
        fcntl(m_stderrPipe[0], F_SETFL, O_NONBLOCK);

        m_isRunning = true;
        return true;

    }

    void NativeProcess::kill() {
        if (m_isRunning && m_pid > 0) {
            ::kill(m_pid, SIGKILL);
            waitpid(m_pid, nullptr, 0);
            m_isRunning = false;
        }
    }

        bool NativeProcess::isRunning() const {
        if (!m_isRunning) return false;

        int status;
        pid_t result = waitpid(m_pid, &status, WNOHANG);
        if (result == 0) return true; // Все еще работает

        if (WIFEXITED(status)) {
            const_cast<NativeProcess*>(this)->m_exitCode = WEXITSTATUS(status);
        } else {
            const_cast<NativeProcess*>(this)->m_exitCode = -1;
        }

        const_cast<NativeProcess*>(this)->m_isRunning = false;
        return false;
    }

    std::string NativeProcess::readAllStdout() {
        return readFromPipe(m_stdoutPipe[0]);
    }

    std::string NativeProcess::readAllStderr() {
        return readFromPipe(m_stderrPipe[0]);
    }

    std::string NativeProcess::readFromPipe(int fd) {
        char buffer[4096];
        std::string result;
        ssize_t bytesRead;

        while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
            result.append(buffer, bytesRead);
        }

        return result;
    }

} // namespace gs