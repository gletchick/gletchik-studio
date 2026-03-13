#pragma once
#include <memory>
#include <vector>

#include "ibuildstep.h"
#include "../steptype.h"

namespace gs {

    class isteprunner {
    public:
        virtual ~isteprunner() = default;
        virtual void setSteps(std::vector<std::shared_ptr<ibuildstep>> steps) = 0;
        virtual bool runUntil(steptype type) = 0;
        virtual void stop() = 0;
    };

} // namespace gs