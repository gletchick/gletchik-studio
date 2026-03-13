#include "core/execution/abstractprocessstep.h"
#include <iostream>
#include <unistd.h>

namespace gs {

    abstractprocessstep::abstractprocessstep(std::shared_ptr<iprocess> process)
        : m_process(std::move(process)) {}

    bool abstractprocessstep::execute() {
        if (!m_process->start(getCommand(), getArguments())) {
            return false;
        }

        while (m_process->isRunning()) {
            std::cout << m_process->readAllStdout();
            std::cerr << m_process->readAllStderr();
            usleep(1000);
        }

        std::cout << m_process->readAllStdout();
        std::cerr << m_process->readAllStderr();

        return m_process->getExitCode() == 0;
    }

} // namespace gs