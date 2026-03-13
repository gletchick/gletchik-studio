#pragma once
#include <memory>
#include <vector>
#include <string>

#include "core/execution/ibuildstep.h"

namespace gs {

    class ilanguageprovider {
    public:
        virtual ~ilanguageprovider() = default;
        virtual std::string languageName() const = 0;
        virtual std::vector<std::shared_ptr<ibuildstep>> getBuildPipeline(
            const std::string& projectPath,
            const std::string& sourceFilePath) = 0;
    };

} // namespace gs