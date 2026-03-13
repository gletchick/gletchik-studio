#pragma once
#include "core/execution/abstractprocessstep.h"

namespace gs {
    class JavaCompileStep : public abstractprocessstep {
    public:
        JavaCompileStep(std::shared_ptr<iprocess> process, std::vector<std::string> sources)
        : abstractprocessstep(std::move(process)), m_sources(std::move(sources)) {}

        std::string name() const override { return "Java Compiler"; }
        steptype type() const override { return steptype::Compile; }

    protected:
        std::string getCommand() const override { return "javac"; }
        std::vector<std::string> getArguments() const override;

    private:
        std::vector<std::string> m_sources;
    };

    class JavaRunStep : public abstractprocessstep {
    public:
        JavaRunStep(std::shared_ptr<iprocess> process, std::string className)
            : abstractprocessstep(std::move(process)), m_className(std::move(className)) {}

        std::string name() const override { return "Java Runtime"; }
        steptype type() const override { return steptype::Run; }

    protected:
        std::string getCommand() const override { return "java"; }
        std::vector<std::string> getArguments() const override;

    private:
        std::string m_className;
    };

} // namespace gs