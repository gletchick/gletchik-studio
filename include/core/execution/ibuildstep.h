#pragma once
#include <string>
#include "../steptype.h"

namespace gs {

    class IBuildStep {
    public:
        virtual ~IBuildStep() = default;
        virtual std::string name() const = 0;
        virtual StepType type() const = 0;
        virtual bool execute() = 0;
    };

} // namespace gs