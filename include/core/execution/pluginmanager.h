#pragma once

#include <string>
#include <memory>
#include <shared_mutex>
#include <vector>
#include "../../../sdk/ilanguageprovider.h"
#include "../../../sdk/iprocess.h"

namespace gs {

    class PluginManager {
    public:
        static std::shared_ptr<ILanguageProvider> loadPlugin(const std::string& path,
                                                            std::shared_ptr<IProcess> process);

        static std::vector<std::shared_ptr<ILanguageProvider>> loadAllFromDir(const std::string& dirPath,
                                                                             std::shared_ptr<IProcess> process);

        static std::shared_ptr<ILanguageProvider> getProviderByExtension(const std::string& extension);
        static void registerBuiltInProvider(std::shared_ptr<ILanguageProvider> provider);

    private:
        static std::unordered_map<std::string, std::shared_ptr<ILanguageProvider>> m_providersMap;
        static std::shared_mutex m_mapMutex;

    };

} // namespace gs