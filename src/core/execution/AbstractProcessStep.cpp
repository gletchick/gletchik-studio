#include "core/execution/AbstractProcessStep.h"
#include <iostream>

namespace gs {

    AbstractProcessStep::AbstractProcessStep(std::shared_ptr<IProcess> process)
        : m_process(std::move(process)) {}

    bool AbstractProcessStep::execute() {
        if (!m_process->start(getCommand(), getArguments())) {
            return false;
        }

        while (m_process->isRunning()) {
            std::cout << m_process->readAllStdout();
            std::cerr << m_process->readAllStderr();
        }

        // Читаем оставшиеся данные после завершения процесса
        std::cout << m_process->readAllStdout();
        std::cerr << m_process->readAllStderr();

        return true;
    }

} // namespace gs