#pragma once
#include <memory>
#include <QJsonObject>
#include <vector>
#include <string>

#include "ibuildstep.h"
#include "syntaxrules.h"

namespace gs {

    class ILanguageProvider {
    public:
        virtual ~ILanguageProvider() = default;
        virtual std::string languageName() const = 0;
        virtual std::vector<std::shared_ptr<IBuildStep>> getBuildPipeline(
            const std::string& projectPath,
            const std::string& sourceFilePath) = 0;

        virtual std::vector<std::string> getSupportedExtensions() const = 0;
        virtual QJsonObject parseFile(const QString& filePath, const QString& content) = 0;
        virtual std::shared_ptr<ISyntaxProvider> getSyntaxProvider() = 0;
    };

} // namespace gs