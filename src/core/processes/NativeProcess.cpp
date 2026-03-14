#include "include/core/processes/nativeprocess.h"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>
#include <ostream>
#include <vector>

namespace gs {

    NativeProcess::~NativeProcess() {
        kill();
    }

    bool NativeProcess::start(const std::string& command, const std::vector<std::string>& args) {
        std::cout << "[DEBUG] Starting command: " << command;
        for (const auto& arg : args) std::cout << " " << arg;
        std::cout << std::endl;

        if (m_isRunning) return false;

        // Создаем 3 пайпа: stdout, stderr и теперь stdin
        if (pipe(m_stdoutPipe) == -1 || pipe(m_stderrPipe) == -1 || pipe(m_stdinPipe) == -1) {
            return false;
        }

        m_pid = fork();
        if (m_pid < 0) return false;

        if (m_pid == 0) { // Дочерний процесс
            // Настраиваем stdin: читаем из пайпа
            dup2(m_stdinPipe[0], STDIN_FILENO);

            // Настраиваем stdout/stderr: пишем в пайпы
            dup2(m_stdoutPipe[1], STDOUT_FILENO);
            dup2(m_stderrPipe[1], STDERR_FILENO);

            // Закрываем все ненужные дескрипторы в дочернем процессе
            close(m_stdinPipe[0]); close(m_stdinPipe[1]);
            close(m_stdoutPipe[0]); close(m_stdoutPipe[1]);
            close(m_stderrPipe[0]); close(m_stderrPipe[1]);

            // ... далее твой код с execvp ...
            std::vector<char*> c_args;
            c_args.push_back(const_cast<char*>(command.c_str()));
            for (const auto& arg : args) {
                c_args.push_back(const_cast<char*>(arg.c_str()));
            }
            c_args.push_back(nullptr);
            execvp(command.c_str(), c_args.data());
            exit(1);
        }

        // Родительский процесс
        // Закрываем стороны пайпов, которые нам не нужны
        close(m_stdinPipe[0]);  // Нам не нужно читать из stdin пайпа
        close(m_stdoutPipe[1]); // Нам не нужно писать в stdout пайп
        close(m_stderrPipe[1]); // Нам не нужно писать в stderr пайп

        // Делаем чтение неблокирующим (как у тебя и было)
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

        bool NativeProcess::isRunning() {
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

#include <poll.h>

    std::string NativeProcess::readFromPipe(int fd) {
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN;

        // Проверяем, есть ли данные, не блокируя поток надолго
        int ret = poll(&pfd, 1, 0);
        if (ret <= 0) return "";

        char buffer[4096];
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
        return (bytesRead > 0) ? std::string(buffer, bytesRead) : "";
    }

    bool NativeProcess::writeToStdin(const std::string& input) {
        if (!m_isRunning || m_pid <= 0) return false;

        ssize_t bytesWritten = write(m_stdinPipe[1], input.c_str(), input.size());
        return bytesWritten != -1;
    }

} // namespace gs