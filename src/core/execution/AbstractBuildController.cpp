#include "core/execution/AbstractBuildController.h"

#include "core/processes/NativeProcess.h"
#include <iostream>

namespace gs {

    AbstractBuildController::AbstractBuildController() {
        // По умолчанию используем наш NativeProcess
        m_process = std::make_shared<NativeProcess>();
    }

    bool AbstractBuildController::runProject(const std::string& projectPath, 
                                             const std::string& sourceFilePath, 
                                             StepType untilStep) {
        // Вызываем метод, который реализуют наследники (Java, C++, и т.д.)
        auto provider = createProvider(m_process);

        if (!provider) {
            std::cerr << "Error: Language provider is null!" << std::endl;
            return false;
        }

        auto steps = provider->getBuildPipeline(projectPath, sourceFilePath);
        m_runner.setSteps(steps);

        std::cout << "--- Starting build via " << provider->languageName() << " Controller ---" << std::endl;
        
        return m_runner.runUntil(untilStep);
    }

    void AbstractBuildController::stop() {
        // Останавливаем раннер, чтобы он не запускал следующие шаги
        m_runner.stop();
        
        // Убиваем текущий процесс (например, если программа зациклилась)
        if (m_process) {
            m_process->kill();
        }
    }

} // namespace gs