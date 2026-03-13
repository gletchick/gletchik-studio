#include "core/languages/java/JavaSteps.h"

namespace gs {

    std::vector<std::string> JavaCompileStep::getArguments() const {
        return { "-d", ".", m_sourceFilePath };
    }

    std::vector<std::string> JavaRunStep::getArguments() const {
        return { "-cp", ".", m_className };
    }

} // namespace gs