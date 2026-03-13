#include "core/languages/java/javabuildbontroller.h"
#include "core/languages/java/javalanguageprovider.h"

namespace gs {

    std::shared_ptr<ilanguageprovider> JavaBuildController::createProvider(std::shared_ptr<iprocess> process) {
        return std::make_shared<javalanguageprovider>(process);
    }

} // namespace gs