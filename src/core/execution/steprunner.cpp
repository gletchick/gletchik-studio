#include "core/execution/steprunner.h"
#include <iostream>
#include <algorithm>

namespace gs {

    void steprunner::setSteps(std::vector<std::shared_ptr<ibuildstep>> steps) {
        m_steps = std::move(steps);

        std::sort(m_steps.begin(), m_steps.end(), [](const auto& a, const auto& b) {
            return static_cast<int>(a->type()) < static_cast<int>(b->type());
        });
    }

    bool steprunner::runUntil(steptype type) {
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

    void steprunner::stop() {
        m_stopRequested = true;
    }

} // namespace gs