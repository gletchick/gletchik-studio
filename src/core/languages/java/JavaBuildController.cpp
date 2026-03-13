#include "core/languages/java/JavaBuildController.h"
#include "core/languages/java/JavaLanguageProvider.h"

namespace gs {

    std::shared_ptr<ILanguageProvider> JavaBuildController::createProvider(std::shared_ptr<IProcess> process) {
        return std::make_shared<JavaLanguageProvider>(process);
    }

} // namespace gs