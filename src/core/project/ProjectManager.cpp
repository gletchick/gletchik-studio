#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include <QDirIterator>
#include <QJsonArray>

#include "core/execution/pluginmanager.h"
#include "core/project/filemanager.h"
#include "core/project/projectmanager.h"
#include "core/utils/utils.h"

namespace gs {

    ProjectManager& ProjectManager::instance() {
        static ProjectManager inst;
        return inst;
    }

    bool ProjectManager::openProject(const QString &path) {
        QMutexLocker locker(&m_dataMutex);

        QDir dir(path);
        if (!dir.exists() || !dir.exists(configDirName)) {
            qWarning() << "Invalid project directory:" << path;
            return false;
        }

        m_projectPath = path;
        QString fullPath = path + "/" + configDirName + "/" + indexFileName;

        QFile file(fullPath);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open index file";
            return false;
        }

        QWriteLocker indexLocker(&m_indexLock);
        m_projectIndex = QJsonDocument::fromJson(file.readAll()).object();
        rebuildSemanticCache();

        return true;
    }

    void ProjectManager::saveProject() {
        QReadLocker indexLocker(&m_indexLock);
        QMutexLocker dataLocker(&m_dataMutex);

        if (m_projectPath.isEmpty()) return;

        QString fullPath = m_projectPath + "/" + configDirName + "/" + indexFileName;
        QFile file(fullPath);

        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(m_projectIndex);
            file.write(doc.toJson());
            file.close();
        }
    }

    QJsonObject ProjectManager::getIndex() {
        QReadLocker locker(&m_indexLock);
        return m_projectIndex;
    }

    void ProjectManager::updateIndex(const QJsonObject &newIndex) {
        QWriteLocker locker(&m_indexLock);
        m_projectIndex = newIndex;
        rebuildSemanticCache();
    }

    QString ProjectManager::getProjectRoot() const {
        QMutexLocker locker(&m_dataMutex);
        return m_projectPath;
    }

    void ProjectManager::reindexAll() {
        QMutexLocker dataLocker(&m_dataMutex);
        if (m_projectPath.isEmpty()) return;

        // 1. Создаем итератор для рекурсивного обхода всех файлов
        QDirIterator it(m_projectPath, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

        while (it.hasNext()) {
            QString filePath = it.next();

            // Пропускаем нашу служебную папку .gs
            if (filePath.contains("/" + configDirName + "/")) {
                continue;
            }

            // 2. Обрабатываем файл
            processFile(filePath);
        }

        // 3. После того как все файлы пройдены, сохраняем итоговый JSON
        saveProject();
        qDebug() << "Reindexing finished.";
    }

    void ProjectManager::processFile(const QString &filePath) {
        // Считаем хэш, чтобы понять, нужно ли вообще открывать файл
        QString currentHash = Utils::calculateFileHash(filePath);

        // Захватываем индекс для проверки
        QWriteLocker indexLocker(&m_indexLock);
        QJsonObject filesObj = m_projectIndex["files"].toObject();

        // Если файл уже есть в индексе и хэш совпадает — ничего не делаем
        if (filesObj.contains(filePath)) {
            if (filesObj[filePath].toObject()["hash"].toString() == currentHash) {
                return;
            }
        }
        indexLocker.unlock(); // Освобождаем, чтобы не блокировать на время парсинга

        // Определяем расширение и ищем плагин
        std::string ext = QFileInfo(filePath).suffix().prepend(".").toStdString();
        auto provider = PluginManager::getProviderByExtension(ext);

        QJsonObject fileData;
        if (provider) {
            // Читаем файл и парсим его через плагин
            QString content = FileManager::readFile(filePath);
            fileData = provider->parseFile(filePath, content);
        }

        fileData["hash"] = currentHash;

        // Снова блокируем для записи результата
        QWriteLocker finalLocker(&m_indexLock);
        QJsonObject currentFiles = m_projectIndex["files"].toObject();
        currentFiles[filePath] = fileData;
        m_projectIndex["files"] = currentFiles;
    }

    TokenType ProjectManager::getSemanticType(const QString &identifier) const {
        QReadLocker locker(&m_indexLock);

        if (m_cachedClasses.contains(identifier)) {
            return TokenType::ProjectClass;
        }

        if (m_cachedMethods.contains(identifier)) {
            return TokenType::ProjectMethod;
        }

        return TokenType::Identifier;
    }

    void ProjectManager::rebuildSemanticCache() {
        m_cachedClasses.clear();
        m_cachedMethods.clear();

        QJsonObject globalIndex = m_projectIndex["index"].toObject();

        QJsonArray classes = globalIndex["classes"].toArray();
        for (const auto &v : classes) {
            m_cachedClasses.insert(v.toString());
        }

        QJsonArray methods = globalIndex["methods"].toArray();
        for (const auto &v : methods) {
            m_cachedMethods.insert(v.toString());
        }
    }
}
