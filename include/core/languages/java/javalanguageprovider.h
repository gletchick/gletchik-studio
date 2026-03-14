#pragma once
#include "../../../../sdk/ilanguageprovider.h"
#include "../../../../sdk/iprocess.h"
#include <memory>

#include "sdk/syntaxrules.h"

namespace gs {

    class JavaLanguageProvider : public ILanguageProvider, public ISyntaxProvider {
    public:
        explicit JavaLanguageProvider(std::shared_ptr<IProcess> process);

        std::string languageName() const override;

        std::vector<HighlightRule> getSyntaxRules() const override;

        std::vector<std::shared_ptr<IBuildStep>> getBuildPipeline(
            const std::string& projectPath,
            const std::string& className) override;

    private:
        std::shared_ptr<IProcess> m_process;
    };

} // namespace gs