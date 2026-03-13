#pragma once
#include <string>
#include <vector>

namespace gs {

    class IProcess {
    public:
        virtual ~IProcess() = default;
        virtual bool start(const std::string& command, const std::vector<std::string>& args) = 0;
        virtual void kill() = 0;
        virtual bool isRunning() const = 0;

        virtual std::string readAllStdout() = 0;
        virtual std::string readAllStderr() = 0;
    };

} // namespace gs