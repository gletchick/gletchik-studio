#pragma once
#include "core/execution/isteprunner.h"
#include <algorithm>

namespace gs {

    class steprunner : public isteprunner {
    public:
        steprunner() = default;
        ~steprunner() override = default;

        void setSteps(std::vector<std::shared_ptr<ibuildstep>> steps) override;

        bool runUntil(steptype type) override;

        void stop() override;

    private:
        std::vector<std::shared_ptr<ibuildstep>> m_steps;
        bool m_stopRequested = false;
    };

} // namespace gs