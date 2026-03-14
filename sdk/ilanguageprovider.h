#pragma once
#include <memory>
#include <vector>
#include <string>

#include "ibuildstep.h"

namespace gs {

    class ILanguageProvider {
    public:
        virtual ~ILanguageProvider() = default;
        virtual std::string languageName() const = 0;
        virtual std::vector<std::shared_ptr<IBuildStep>> getBuildPipeline(
            const std::string& projectPath,
            const std::string& sourceFilePath) = 0;
    };

} // namespace gs