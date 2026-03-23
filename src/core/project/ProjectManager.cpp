#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QDebug>
#include <QJsonDocument>
#include <QDirIterator>

#include "core/project/projectmanager.h"
#include "core/execution/pluginmanager.h"
#include "core/project/filemanager.h"
#include "core/utils/utils.h"

namespace gs {

    // Инициализируем воркер в конструкторе, чтобы он создался ровно один раз
    ProjectManager::ProjectManager() {
        m_worker = std::make_unique<IndexWorker>();
    }

    // Синглтон возвращаем без пересоздания воркера
    ProjectManager& ProjectManager::instance() {
        static ProjectManager inst;
        return inst;
    }

    bool ProjectManager::cloneFromVCS(const QString &url, const QString &path) {
        qDebug() << "Cloning project from" << url << "to" << path;
        return true;
    }

    bool ProjectManager::createProject(const QString &path) {
        QMutexLocker dataLocker(&m_dataMutex);

        QDir dir(path);
        if (!dir.exists()) {
            if (!dir.mkpath(".")) {
                qWarning() << "Failed to create project directory:" << path;
                return false;
            }
        }

        if (!dir.exists(configDirName)) {
            if (!dir.mkdir(configDirName)) {
                qWarning() << "Failed to create .gs directory in:" << path;
                return false;
            }
        }

        QString fullPath = path + "/" + configDirName + "/" + indexFileName;

        if (!QFile::exists(fullPath)) {
            QJsonObject root;
            root["files"] = QJsonObject();

            QJsonDocument doc(root);
            if (!FileManager::saveFile(fullPath, doc.toJson())) {
                qWarning() << "Failed to create index file:" << fullPath;
                return false;
            }
        }

        dataLocker.unlock();
        return openProject(path);
    }

    bool ProjectManager::openProject(const QString &path) {
        QMutexLocker dataLocker(&m_dataMutex);

        QDir dir(path);
        if (!dir.exists() || !dir.exists(configDirName)) {
            qWarning() << "Invalid project directory or missing metadata (.gs):" << path;
            return false;
        }

        m_projectPath = path;
        QString fullPath = path + "/" + configDirName + "/" + indexFileName;

        QString fileContent = FileManager::readFile(fullPath);

        if (!fileContent.isEmpty()) {
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(fileContent.toUtf8(), &parseError);

            if (parseError.error == QJsonParseError::NoError) {
                QWriteLocker indexLocker(&m_indexLock);
                m_projectIndex.loadFromJson(doc.object());
            } else {
                qWarning() << "Failed to parse project index JSON, starting with empty index:"
                           << parseError.errorString();
            }
        } else {
            qDebug() << "Index file not found or empty, it will be created after indexing.";
        }

        dataLocker.unlock();
        reindexAll();

        qDebug() << "Project successfully opened and indexed:" << path;
        return true;
    }

    void ProjectManager::saveProject() {
        if (m_projectPath.isEmpty()) return;

        QString fullPath = m_projectPath + "/" + configDirName + "/" + indexFileName;

        QJsonDocument doc(m_projectIndex.toJson());
        FileManager::saveFile(fullPath, doc.toJson());
    }

    QJsonObject ProjectManager::getIndex() {
        QReadLocker indexLocker(&m_indexLock);
        return m_projectIndex.toJson();
    }

    void ProjectManager::updateIndex(const QJsonObject &newIndex) {
        QWriteLocker indexLocker(&m_indexLock);
        m_projectIndex.loadFromJson(newIndex);
    }

    void ProjectManager::reindexAll() {
        QString path;
        {
            QMutexLocker dataLocker(&m_dataMutex);
            path = m_projectPath;
        }

        if (path.isEmpty()) return;

        QStringList filesToIndex;
        QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            filesToIndex << it.next();
        }

        for (const QString& filePath : filesToIndex) {
            QFileInfo info(filePath);
            QString ext = "." + info.suffix();

            if (PluginManager::getProviderByExtension(ext.toStdString())) {
                processFile(filePath);
            }
        }

        qDebug() << "Full reindexing completed.";
    }

    QString ProjectManager::getProjectRoot() const {
        QMutexLocker dataLocker(&m_dataMutex);
        return m_projectPath;
    }

    TokenType ProjectManager::getSemanticType(const QString &identifier) const {
        QReadLocker indexLocker(&m_indexLock);

        if (m_projectIndex.isClass(identifier)) {
            return TokenType::ProjectClass;
        }
        if (m_projectIndex.isMethod(identifier)) {
            return TokenType::ProjectMethod;
        }

        return TokenType::Identifier;
    }

    void ProjectManager::processFile(const QString &filePath) {
        if (m_worker) {
            m_worker->enqueueFile(filePath);
        }
    }

    void ProjectManager::processFileInternal(const QString &filePath) {
        if (filePath.endsWith(indexFileName) || filePath.contains(configDirName)) {
            return;
        }

        QString currentHash = Utils::calculateFileHash(filePath);
        if (currentHash.isEmpty()) return;

        QFileInfo fileInfo(filePath);
        QString extension = "." + fileInfo.suffix();
        auto provider = PluginManager::getProviderByExtension(extension.toStdString());

        if (!provider) return;

        QString content = FileManager::readFile(filePath);
        QJsonObject fileData = provider->parseFile(filePath, content);

        QSet<QString> classes;
        QSet<QString> methods;

        QJsonArray clsArr = fileData["classes"].toArray();
        for(const auto &v : clsArr) classes.insert(v.toString());

        QJsonArray methArr = fileData["methods"].toArray();
        for(const auto &v : methArr) methods.insert(v.toString());

        {
            QWriteLocker indexLocker(&m_indexLock);
            m_projectIndex.updateFile(filePath, currentHash, classes, methods);
        }
    }

    void ProjectManager::rebuildSemanticCache() {
    }

} // namespace gs