#include "core/languages/java/JavaLanguageProvider.h"

#include <filesystem>

#include "core/languages/java/JavaSteps.h"

namespace gs {

    JavaLanguageProvider::JavaLanguageProvider(std::shared_ptr<IProcess> process)
        : m_process(std::move(process)) {}

    std::string JavaLanguageProvider::languageName() const {
        return "Java";
    }

    std::vector<std::shared_ptr<IBuildStep>> JavaLanguageProvider::getBuildPipeline(
    const std::string& projectPath,
    const std::string& sourceFilePath) {

        std::vector<std::shared_ptr<IBuildStep>> pipeline;

        // 1. Извлекаем имя файла без расширения (например, "HelloGleb")
        std::filesystem::path p(sourceFilePath);
        std::string className = p.stem().string();

        // 2. Шаг компиляции использует полный путь к .java
        pipeline.push_back(std::make_shared<JavaCompileStep>(m_process, sourceFilePath));

        // 3. Шаг запуска использует только имя класса
        pipeline.push_back(std::make_shared<JavaRunStep>(m_process, className));

        return pipeline;
    }

} // namespace gs