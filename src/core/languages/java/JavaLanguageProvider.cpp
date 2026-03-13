#include "core/languages/java/JavaLanguageProvider.h"
#include "core/languages/java/JavaSteps.h"

namespace gs {

    JavaLanguageProvider::JavaLanguageProvider(std::shared_ptr<IProcess> process)
        : m_process(std::move(process)) {}

    std::string JavaLanguageProvider::languageName() const {
        return "Java";
    }

    std::vector<std::shared_ptr<IBuildStep>> JavaLanguageProvider::getBuildPipeline(const std::string& projectPath) {
        std::vector<std::shared_ptr<IBuildStep>> pipeline;
        pipeline.push_back(std::make_shared<JavaCompileStep>(m_process));
        pipeline.push_back(std::make_shared<JavaRunStep>(m_process));
        return pipeline;
    }

} // namespace gs