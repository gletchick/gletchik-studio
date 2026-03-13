#include "core/languages/java/JavaLanguageProvider.h"

#include <filesystem>
#include <iostream>

#include "core/languages/java/JavaSteps.h"
#include "utils/JavaParser.h"

namespace fs = std::filesystem;

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

        // 1. Сканируем проект на наличие ВСЕХ .java файлов
        std::vector<std::string> allSources;
        try {
            for (const auto& entry : fs::recursive_directory_iterator(projectPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".java") {
                    allSources.push_back(entry.path().string());
                }
            }
        } catch (...) {
            // Обработка ошибок доступа к директории
        }

        // 2. Определяем имя главного класса для запуска
        std::string mainClassName;
        try {
            // Используем наш новый статический метод
            mainClassName = JavaParser::getFullyQualifiedName(sourceFilePath);
        } catch (const std::exception& e) {
            // Если в файле две декларации пакета или файл не открылся
            std::cerr << "Build Error: " << e.what() << std::endl;
            return {}; // Возвращаем пустой вектор, чтобы выполнение прервалось
        }

        // 3. Создаем шаг компиляции, передавая список всех файлов
        pipeline.push_back(std::make_shared<JavaCompileStep>(m_process, allSources));

        // 4. Шаг запуска
        pipeline.push_back(std::make_shared<JavaRunStep>(m_process, mainClassName));

        return pipeline;
    }

} // namespace gs