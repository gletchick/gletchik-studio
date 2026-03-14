#include "core/execution/abstractprocessstep.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace gs {

    AbstractProcessStep::AbstractProcessStep(std::shared_ptr<IProcess> process)
        : m_process(std::move(process)) {}

    bool AbstractProcessStep::execute(LogCallback logger) {
        if (!m_process->start(getCommand(), getArguments())) {
            if (logger) logger("Failed to start process: " + getCommand() + "\n", true);
            return false;
        }

        while (m_process->isRunning()) {
            std::string out = m_process->readAllStdout();
            std::string err = m_process->readAllStderr();

            if (!out.empty() && logger) logger(out, false);
            if (!err.empty() && logger) logger(err, true);

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        std::string out = m_process->readAllStdout();
        std::string err = m_process->readAllStderr();
        if (!out.empty() && logger) logger(out, false);
        if (!err.empty() && logger) logger(err, true);

        return m_process->getExitCode() == 0;
    }

} // namespace gs