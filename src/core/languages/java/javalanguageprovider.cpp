#include "core/languages/java/javalanguageprovider.h"

#include <filesystem>
#include <iostream>

#include "core/languages/java/javasteps.h"
#include "../../../../include/core/utils/javaparser.h"

namespace fs = std::filesystem;

namespace gs {

    javalanguageprovider::javalanguageprovider(std::shared_ptr<iprocess> process)
        : m_process(std::move(process)) {}

    std::string javalanguageprovider::languageName() const {
        return "Java";
    }

    std::vector<std::shared_ptr<ibuildstep>> javalanguageprovider::getBuildPipeline(
    const std::string& projectPath,
    const std::string& sourceFilePath) {

        std::vector<std::shared_ptr<ibuildstep>> pipeline;

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

} // namespace gs