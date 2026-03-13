#include "core/languages/java/JavaSteps.h"

namespace gs {

    std::vector<std::string> JavaCompileStep::getArguments() const {
        return { "-d", ".", "./../Main.java" };
    }

    std::vector<std::string> JavaRunStep::getArguments() const {
        return { "-cp", ".", "Main" };
    }

} // namespace gs