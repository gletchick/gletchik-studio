#pragma once
#include "isteprunner.h"
#include <algorithm>

namespace gs {

    class StepRunner : public IStepRunner {
    public:
        StepRunner() = default;
        ~StepRunner() override = default;

        void setSteps(std::vector<std::shared_ptr<IBuildStep>> steps) override;

        bool runUntil(StepType type) override;

        void stop() override;

    private:
        std::vector<std::shared_ptr<IBuildStep>> m_steps;
        bool m_stopRequested = false;
    };

} // namespace gs