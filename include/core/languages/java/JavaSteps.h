#pragma once
#include "core/execution/AbstractProcessStep.h"

namespace gs {

    class JavaCompileStep : public AbstractProcessStep {
    public:
        using AbstractProcessStep::AbstractProcessStep;

        std::string name() const override { return "Java Compiler"; }
        StepType type() const override { return StepType::Compile; }

    protected:
        std::string getCommand() const override { return "javac"; }
        std::vector<std::string> getArguments() const override; // Только объявление
    };

    class JavaRunStep : public AbstractProcessStep {
    public:
        using AbstractProcessStep::AbstractProcessStep;

        std::string name() const override { return "Java Runtime"; }
        StepType type() const override { return StepType::Run; }

    protected:
        std::string getCommand() const override { return "java"; }
        std::vector<std::string> getArguments() const override; // Только объявление
    };

} // namespace gs