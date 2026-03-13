#pragma once
#include <string>
#include "core/StepType.h"

namespace gs {

    class IBuildController {
    public:
        virtual ~IBuildController() = default;

        virtual bool runProject(const std::string& projectPath,
                                const std::string& sourceFilePath, 
                                StepType untilStep) = 0;

        virtual void stop() = 0;
    };

} // namespace gs