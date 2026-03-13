#pragma once
#include "ibuildcontroller.h"
#include "core/execution/steprunner.h"
#include "core/processes/iprocess.h"
#include "core/languages/ilanguageprovider.h"
#include <memory>

namespace gs {

    class AbstractBuildController : public ibuildcontroller {
    public:
        AbstractBuildController();
        ~AbstractBuildController() override = default;

        bool runProject(const std::string& projectPath, 
                        const std::string& sourceFilePath, 
                        steptype untilStep) override;

        void stop() override;

    protected:
        virtual std::shared_ptr<ilanguageprovider> createProvider(std::shared_ptr<iprocess> process) = 0;

        std::shared_ptr<iprocess> m_process;
        steprunner m_runner;
    };

} // namespace gs