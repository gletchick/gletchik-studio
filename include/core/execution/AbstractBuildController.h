#pragma once
#include "IBuildController.h"
#include "core/execution/StepRunner.h"
#include "core/processes/IProcess.h"
#include "core/languages/ILanguageProvider.h"
#include <memory>

namespace gs {

    class AbstractBuildController : public IBuildController {
    public:
        AbstractBuildController();
        ~AbstractBuildController() override = default;

        bool runProject(const std::string& projectPath, 
                        const std::string& sourceFilePath, 
                        StepType untilStep) override;

        void stop() override;

    protected:
        virtual std::shared_ptr<ILanguageProvider> createProvider(std::shared_ptr<IProcess> process) = 0;

        std::shared_ptr<IProcess> m_process;
        StepRunner m_runner;
    };

} // namespace gs