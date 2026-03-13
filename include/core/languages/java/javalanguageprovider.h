#pragma once
#include "core/languages/ilanguageprovider.h"
#include "core/processes/iprocess.h"
#include <memory>

namespace gs {

    class javalanguageprovider : public ilanguageprovider {
    public:
        explicit javalanguageprovider(std::shared_ptr<iprocess> process);

        std::string languageName() const override;


        std::vector<std::shared_ptr<ibuildstep>> getBuildPipeline(
            const std::string& projectPath,
            const std::string& className) override;

    private:
        std::shared_ptr<iprocess> m_process;
    };

} // namespace gs