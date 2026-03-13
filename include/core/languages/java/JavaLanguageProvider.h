#pragma once
#include "core/languages/ILanguageProvider.h"
#include "core/processes/IProcess.h"
#include <memory>

namespace gs {

    class JavaLanguageProvider : public ILanguageProvider {
    public:
        // Мы передаем shared_ptr на процесс, чтобы провайдер мог
        // "раздать" его создаваемым шагам
        explicit JavaLanguageProvider(std::shared_ptr<IProcess> process);

        std::string languageName() const override;

        // Главный метод: возвращает вектор из JavaCompileStep и JavaRunStep
        std::vector<std::shared_ptr<IBuildStep>> getBuildPipeline(const std::string& projectPath) override;

    private:
        std::shared_ptr<IProcess> m_process;
    };

} // namespace gs