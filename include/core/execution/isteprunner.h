#pragma once
#include <memory>
#include <vector>

#include "ibuildstep.h"
#include "../steptype.h"

namespace gs {

    class IStepRunner {
    public:
        virtual ~IStepRunner() = default;
        virtual void setSteps(std::vector<std::shared_ptr<IBuildStep>> steps) = 0;
        virtual bool runUntil(StepType type) = 0;
        virtual void stop() = 0;
    };

} // namespace gs