#pragma once
#include "core/execution/abstractbuildcontroller.h"

namespace gs {

    class JavaBuildController : public AbstractBuildController {
    public:
        JavaBuildController() = default;
        ~JavaBuildController() override = default;

    protected:
        std::shared_ptr<ilanguageprovider> createProvider(std::shared_ptr<iprocess> process) override;
    };

} // namespace gs