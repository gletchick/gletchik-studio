#include "utils/JavaParser.h"
#include <fstream>
#include <regex>
#include <filesystem>
#include <stdexcept>

namespace gs {

    std::string JavaParser::getFullyQualifiedName(const std::string& filePath) {
        std::string packageName = extractPackage(filePath);
        std::string className = std::filesystem::path(filePath).stem().string();

        if (packageName.empty()) {
            return className;
        }
        return packageName + "." + className;
    }

    std::string JavaParser::extractPackage(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }

        std::string line;
        std::string packageName = "";
        int packageCount = 0;

        // Регулярное выражение для поиска: package имя.пакета;
        std::regex packageRegex(R"(^\s*package\s+([\w\.]+)\s*;)");
        std::smatch match;

        while (std::getline(file, line)) {
            if (std::regex_search(line, match, packageRegex)) {
                packageCount++;
                if (packageCount > 1) {
                    throw std::runtime_error("Multiple package declarations found in " + filePath);
                }
                packageName = match[1];
            }
        }

        return packageName;
    }

} // namespace gs