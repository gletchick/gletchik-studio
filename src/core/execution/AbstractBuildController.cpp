#include "core/execution/abstractbuildcontroller.h"

#include "core/processes/nativeprocess.h"
#include <iostream>

namespace gs {

    AbstractBuildController::AbstractBuildController() {
        m_process = std::make_shared<NativeProcess>();
    }

    bool AbstractBuildController::runProject(const std::string& projectPath, 
                                             const std::string& sourceFilePath, 
                                             StepType untilStep,
                                             LogCallback logger) {
        auto provider = createProvider(m_process);

        if (!provider) {
            std::cerr << "Error: Language provider is null!" << std::endl;
            return false;
        }

        auto steps = provider->getBuildPipeline(projectPath, sourceFilePath);
        m_runner.setSteps(steps);

        std::cout << "--- Starting build via " << provider->languageName() << " Controller ---" << std::endl;
        
        return m_runner.runUntil(untilStep, logger);
    }

    void AbstractBuildController::stop() {
        m_runner.stop();
        
        if (m_process) {
            m_process->kill();
        }
    }

    void AbstractBuildController::writeInput(const std::string& input) {
        if (m_process && m_process->isRunning()) {
            m_process->writeToStdin(input);
        }
    }

} // namespace gs