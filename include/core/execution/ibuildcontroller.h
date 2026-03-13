#pragma once
#include <string>
#include "core/steptype.h"

namespace gs {

    class ibuildcontroller {
    public:
        virtual ~ibuildcontroller() = default;

        virtual bool runProject(const std::string& projectPath,
                                const std::string& sourceFilePath, 
                                steptype untilStep) = 0;

        virtual void stop() = 0;
    };

} // namespace gs