#include "core/execution/steprunner.h"
#include <iostream>
#include <algorithm>

namespace gs {

    void StepRunner::setSteps(std::vector<std::shared_ptr<IBuildStep>> steps) {
        m_steps = std::move(steps);

        std::sort(m_steps.begin(), m_steps.end(), [](const auto& a, const auto& b) {
            return static_cast<int>(a->type()) < static_cast<int>(b->type());
        });
    }

    bool StepRunner::runUntil(StepType type) {
        m_stopRequested = false;

        for (const auto& step : m_steps) {
            if (m_stopRequested || step->type() > type) {
                break;
            }

            if (!step->execute()) {
                return false;
            }
        }

        return !m_stopRequested;
    }

    void StepRunner::stop() {
        m_stopRequested = true;
    }

} // namespace gs