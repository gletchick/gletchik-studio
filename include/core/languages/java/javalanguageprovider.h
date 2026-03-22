#pragma once
#include "../../../../sdk/ilanguageprovider.h"
#include "../../../../sdk/iprocess.h"
#include <memory>

namespace gs {

    class JavaLanguageProvider : public ILanguageProvider {
    public:
        explicit JavaLanguageProvider(std::shared_ptr<IProcess> process);

        std::string languageName() const override;

        std::vector<std::shared_ptr<IBuildStep>> getBuildPipeline(
            const std::string& projectPath,
            const std::string& className) override;
        bool isRunnable(const std::string& filePath, const std::string& content) const override;

        std::vector<std::string> getSupportedExtensions() const override;
        QJsonObject parseFile(const QString& filePath, const QString& content) override;
        std::shared_ptr<ISyntaxProvider> getSyntaxProvider() override;

    private:
        std::shared_ptr<IProcess> m_process;
    };

} // namespace gs