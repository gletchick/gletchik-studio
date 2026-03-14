#pragma once
#include <memory>
#include <string>
#include <vector>

#include "../../../sdk/ibuildstep.h"
#include "../../../sdk/iprocess.h"

namespace gs {

    class AbstractProcessStep : public IBuildStep {
    public:
        explicit AbstractProcessStep(std::shared_ptr<IProcess> process);
        virtual ~AbstractProcessStep() = default;

        bool execute() override;

    protected:
        virtual std::string getCommand() const = 0;
        virtual std::vector<std::string> getArguments() const = 0;

        std::shared_ptr<IProcess> m_process;
    };

} // namespace gs