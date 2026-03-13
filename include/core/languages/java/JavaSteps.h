#pragma once
#include "core/execution/AbstractProcessStep.h"

namespace gs {
    class JavaCompileStep : public AbstractProcessStep {
    public:
        JavaCompileStep(std::shared_ptr<IProcess> process, std::vector<std::string> sources)
        : AbstractProcessStep(std::move(process)), m_sources(std::move(sources)) {}

        std::string name() const override { return "Java Compiler"; }
        StepType type() const override { return StepType::Compile; }

    protected:
        std::string getCommand() const override { return "javac"; }
        std::vector<std::string> getArguments() const override;

    private:
        std::vector<std::string> m_sources;
    };

    class JavaRunStep : public AbstractProcessStep {
    public:
        JavaRunStep(std::shared_ptr<IProcess> process, std::string className)
            : AbstractProcessStep(std::move(process)), m_className(std::move(className)) {}

        std::string name() const override { return "Java Runtime"; }
        StepType type() const override { return StepType::Run; }

    protected:
        std::string getCommand() const override { return "java"; }
        std::vector<std::string> getArguments() const override;

    private:
        std::string m_className;
    };
} // namespace gs