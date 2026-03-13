#pragma once
#include "core/execution/IStepRunner.h"
#include <algorithm>

namespace gs {

    class StepRunner : public IStepRunner {
    public:
        StepRunner() = default;
        ~StepRunner() override = default;

        // Установка списка шагов (получаем их от LanguageProvider)
        void setSteps(std::vector<std::shared_ptr<IBuildStep>> steps) override;

        // Запуск цепочки до указанного типа (включительно)
        bool runUntil(StepType type) override;

        // Принудительная остановка (например, пользователь нажал кнопку Stop)
        void stop() override;

    private:
        std::vector<std::shared_ptr<IBuildStep>> m_steps;
        bool m_stopRequested = false;
    };

} // namespace gs