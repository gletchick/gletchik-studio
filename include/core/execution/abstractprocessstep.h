#pragma once
#include <memory>
#include <string>
#include <vector>

#include "ibuildstep.h"
#include "core/processes/iprocess.h"

namespace gs {

    class abstractprocessstep : public ibuildstep {
    public:
        explicit abstractprocessstep(std::shared_ptr<iprocess> process);
        virtual ~abstractprocessstep() = default;

        bool execute() override;

    protected:
        virtual std::string getCommand() const = 0;
        virtual std::vector<std::string> getArguments() const = 0;

        std::shared_ptr<iprocess> m_process;
    };

} // namespace gs