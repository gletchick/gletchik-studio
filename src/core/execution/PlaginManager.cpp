#include "core/execution/pluginmanager.h"
#include <dlfcn.h>
#include <iostream>
#include <filesystem>
#include <mutex>

namespace fs = std::filesystem;

namespace gs {

    std::unordered_map<std::string, std::shared_ptr<ILanguageProvider>> PluginManager::m_providersMap;
    std::shared_mutex PluginManager::m_mapMutex;

    typedef ILanguageProvider* (*CreateProviderFunc)(IProcess*);

    std::shared_ptr<ILanguageProvider> PluginManager::loadPlugin(const std::string& path,
                                                                std::shared_ptr<IProcess> process) {
        void* handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "[PluginManager] Error loading " << path << ": " << dlerror() << std::endl;
            return nullptr;
        }

        dlerror();
        auto createFn = (CreateProviderFunc)dlsym(handle, "create_provider");

        const char* dlsym_error = dlerror();
        if (dlsym_error) {
            std::cerr << "[PluginManager] Symbol error in " << path << ": " << dlsym_error << std::endl;
            dlclose(handle);
            return nullptr;
        }

        ILanguageProvider* rawProvider = createFn(process.get());

        if (!rawProvider) {
            dlclose(handle);
            return nullptr;
        }

        return std::shared_ptr<ILanguageProvider>(rawProvider, [handle](ILanguageProvider* p) {
            delete p;
            dlclose(handle);
        });
    }

    std::vector<std::shared_ptr<ILanguageProvider>> PluginManager::loadAllFromDir(const std::string& dirPath,
                                                                                 std::shared_ptr<IProcess> process) {
        std::vector<std::shared_ptr<ILanguageProvider>> providers;

        if (!fs::exists(dirPath)) {
            fs::create_directories(dirPath);
            return providers;
        }

        std::unique_lock<std::shared_mutex> lock(m_mapMutex);

        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (entry.path().extension() == ".so") {
                auto p = loadPlugin(entry.path().string(), process);
                if (p) {
                    std::cout << "[PluginManager] Successfully loaded: " << entry.path().filename() << std::endl;
                    providers.push_back(p);

                    for (const std::string& ext : p->getSupportedExtensions()) {
                        m_providersMap[ext] = p;
                    }
                }
            }
        }
        return providers;
    }

    std::shared_ptr<ILanguageProvider> PluginManager::getProviderByExtension(const std::string& extension) {
        std::shared_lock<std::shared_mutex> lock(m_mapMutex);
        auto it = m_providersMap.find(extension);
        if (it != m_providersMap.end()) {
            return it->second;
        }
        return nullptr;
    }

} // namespace gs