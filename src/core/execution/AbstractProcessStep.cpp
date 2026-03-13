#include "core/execution/abstractprocessstep.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace gs {

    AbstractProcessStep::AbstractProcessStep(std::shared_ptr<IProcess> process)
        : m_process(std::move(process)) {}

    bool AbstractProcessStep::execute() {
        if (!m_process->start(getCommand(), getArguments())) {
            std::cerr << "Failed to start process: " << getCommand() << std::endl;
            return false;
        }

        // Читаем вывод в главном потоке, пока процесс жив.
        // Это не даст пайплайну пойти дальше, пока текущий шаг (например, компиляция) не завершится.
        while (m_process->isRunning()) {
            std::string out = m_process->readAllStdout();
            std::string err = m_process->readAllStderr();

            if (!out.empty()) std::cout << out << std::flush;
            if (!err.empty()) std::cerr << err << std::flush;

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // Дочитываем остатки после завершения процесса
        std::string out = m_process->readAllStdout();
        std::string err = m_process->readAllStderr();
        if (!out.empty()) std::cout << out << std::flush;
        if (!err.empty()) std::cerr << err << std::flush;

        // Шаг успешен только если процесс завершился без ошибок (код 0)
        return m_process->getExitCode() == 0;
    }

} // namespace gs