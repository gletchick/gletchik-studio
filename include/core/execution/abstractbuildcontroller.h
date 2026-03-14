#pragma once
#include "ibuildcontroller.h"
#include "core/execution/steprunner.h"
#include "../../../sdk/iprocess.h"
#include "../../../sdk/ilanguageprovider.h"
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

        void writeInput(const std::string& input) override;

    protected:
        virtual std::shared_ptr<ILanguageProvider> createProvider(std::shared_ptr<IProcess> process) = 0;

        std::shared_ptr<IProcess> m_process;
        StepRunner m_runner;
    };

} // namespace gs