#pragma once
#include "core/execution/AbstractBuildController.h"

namespace gs {

    class JavaBuildController : public AbstractBuildController {
    public:
        JavaBuildController() = default;
        ~JavaBuildController() override = default;

    protected:
        std::shared_ptr<ILanguageProvider> createProvider(std::shared_ptr<IProcess> process) override;
    };

} // namespace gs