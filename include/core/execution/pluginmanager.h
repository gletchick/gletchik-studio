#pragma once

#include <string>
#include <memory>
#include <vector>
#include "core/languages/ilanguageprovider.h"
#include "core/processes/iprocess.h"

namespace gs {

    class PluginManager {
    public:
        static std::shared_ptr<ILanguageProvider> loadPlugin(const std::string& path,
                                                            std::shared_ptr<IProcess> process);

        static std::vector<std::shared_ptr<ILanguageProvider>> loadAllFromDir(const std::string& dirPath,
                                                                             std::shared_ptr<IProcess> process);
    };

} // namespace gs