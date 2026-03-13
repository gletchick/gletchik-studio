#pragma once
#include <string>

namespace gs {

    class JavaParser {
    public:
        static std::string getFullyQualifiedName(const std::string& filePath);

    private:
        static std::string extractPackage(const std::string& filePath);
    };

} // namespace gs