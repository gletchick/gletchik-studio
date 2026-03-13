#include "core/languages/java/JavaSteps.h"

namespace gs {

    std::vector<std::string> JavaCompileStep::getArguments() const {
        std::vector<std::string> args = { "-d", "." };

        args.insert(args.end(), m_sources.begin(), m_sources.end());

        return args;
    }

    std::vector<std::string> JavaRunStep::getArguments() const {
        return { "-cp", ".", m_className };
    }

} // namespace gs