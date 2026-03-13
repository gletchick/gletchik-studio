#include "core/execution/abstractbuildcontroller.h"

#include "core/processes/nativeprocess.h"
#include <iostream>

namespace gs {

    AbstractBuildController::AbstractBuildController() {
        m_process = std::make_shared<nativeprocess>();
    }

    bool AbstractBuildController::runProject(const std::string& projectPath, 
                                             const std::string& sourceFilePath, 
                                             steptype untilStep) {
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
        m_runner.stop();
        
        if (m_process) {
            m_process->kill();
        }
    }

} // namespace gs