#pragma once
#include "core/languages/ilanguageprovider.h"
#include "core/processes/iprocess.h"
#include <memory>

namespace gs {

    class JavaLanguageProvider : public ILanguageProvider {
    public:
        explicit JavaLanguageProvider(std::shared_ptr<IProcess> process);

        std::string languageName() const override;


        std::vector<std::shared_ptr<IBuildStep>> getBuildPipeline(
            const std::string& projectPath,
            const std::string& className) override;

    private:
        std::shared_ptr<IProcess> m_process;
    };

} // namespace gs