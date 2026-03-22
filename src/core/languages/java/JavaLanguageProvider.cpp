#include "core/languages/java/javalanguageprovider.h"

#include <filesystem>
#include <iostream>
#include <QJsonArray>
#include "core/languages/java/javasteps.h"
#include "../../../../include/core/utils/javaparser.h"
#include "core/languages/java/javasyntaxprovider.h"

namespace {
    const QString K_CLASSES = "classes";
    const QString K_METHODS = "methods";

    // Паттерн для классов, интерфейсов, перечней и рекордов
    const QString CLASS_PATTERN = "\\b(?:class|interface|enum|record)\\s+([A-Za-z_][A-Za-z0-9_]*)";

    // Упрощенный паттерн для методов: ищем сигнатуру (модификатор + тип + имя + скобка)
    // Группа 1 захватывает именно имя метода
    const QString METHOD_PATTERN = "(?:public|protected|private|static|\\s) +[\\w\\<\\>\\[\\]\\, ]+\\s+([A-Za-z_][A-Za-z0-9_]*)\\s*\\(";
}

namespace fs = std::filesystem;

namespace gs {

    JavaLanguageProvider::JavaLanguageProvider(std::shared_ptr<IProcess> process)
        : m_process(std::move(process)) {}

    std::vector<std::shared_ptr<IBuildStep>> JavaLanguageProvider::getBuildPipeline(
    const std::string& projectPath,
    const std::string& sourceFilePath) {

        std::vector<std::shared_ptr<IBuildStep>> pipeline;

        std::vector<std::string> allSources;
        try {
            for (const auto& entry : fs::recursive_directory_iterator(projectPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".java") {
                    allSources.push_back(entry.path().string());
                }
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "[File System Error]: " << e.what() << std::endl;
            std::cerr << "Path 1: " << e.path1() << std::endl;
            if (!e.path2().empty()) std::cerr << "Path 2: " << e.path2() << std::endl;
            std::cerr << "Error Code: " << e.code().value() << " (" << e.code().message() << ")" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[Standard Exception]: " << e.what() << std::endl;
        }

        std::string mainClassName;
        try {
            mainClassName = JavaParser::getFullyQualifiedName(sourceFilePath);
        } catch (const std::exception& e) {
            std::cerr << "Build Error: " << e.what() << std::endl;
            return {};
        }

        pipeline.push_back(std::make_shared<JavaCompileStep>(m_process, allSources));

        pipeline.push_back(std::make_shared<JavaRunStep>(m_process, mainClassName));

        return pipeline;
    }

    std::string JavaLanguageProvider::languageName() const {
        return "Java";
    }

    std::vector<std::string> JavaLanguageProvider::getSupportedExtensions() const {
        return { ".java" };
    }

    QJsonObject JavaLanguageProvider::parseFile(const QString& filePath, const QString& content) {
        QJsonObject result;
        QJsonArray classesArray;
        QJsonArray methodsArray;

        QRegularExpression classRegex(CLASS_PATTERN);
        QRegularExpression methodRegex(METHOD_PATTERN);

        // Поиск всех определений классов
        auto classIt = classRegex.globalMatch(content);
        while (classIt.hasNext()) {
            auto match = classIt.next();
            QString className = match.captured(1);
            if (!className.isEmpty()) {
                classesArray.append(className);
            }
        }

        // Поиск всех определений методов
        auto methodIt = methodRegex.globalMatch(content);
        while (methodIt.hasNext()) {
            auto match = methodIt.next();
            QString methodName = match.captured(1);

            // Фильтруем ключевые слова, которые могут попасть под паттерн (например, if, while)
            static const QSet<QString> excludeKeywords = {
                "if", "for", "while", "switch", "catch", "synchronized"
            };

            if (!methodName.isEmpty() && !excludeKeywords.contains(methodName)) {
                methodsArray.append(methodName);
            }
        }

        result[K_CLASSES] = classesArray;
        result[K_METHODS] = methodsArray;

        return result;
    }

    std::shared_ptr<ISyntaxProvider> JavaLanguageProvider::getSyntaxProvider() {
        static std::shared_ptr<ISyntaxProvider> instance = std::make_shared<JavaSyntaxProvider>();
        return instance;
    }

} // namespace gs