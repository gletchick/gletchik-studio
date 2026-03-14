#pragma once
#include <functional>
#include <string>
#include "steptype.h"

namespace gs {

    using LogCallback = std::function<void(const std::string& text, bool isError)>;

    class IBuildStep {
    public:
        virtual ~IBuildStep() = default;
        virtual std::string name() const = 0;
        virtual StepType type() const = 0;
        virtual bool execute(LogCallback logger) = 0;
    };

} // namespace gs