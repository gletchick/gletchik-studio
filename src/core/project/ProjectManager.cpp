#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QDebug>
#include <QJsonDocument>

#include "core/project/projectmanager.h"

namespace gs {

    ProjectManager& ProjectManager::instance() {
        static ProjectManager inst;
        return inst;
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
        QFile file(fullPath);

        if (!file.exists()) {
            if (file.open(QIODevice::WriteOnly)) {
                QJsonObject root;
                root["files"] = QJsonObject();
                QJsonObject indexMap;
                indexMap["classes"] = QJsonArray();
                indexMap["methods"] = QJsonArray();
                root["index"] = indexMap;

                QJsonDocument doc(root);
                file.write(doc.toJson());
                file.close();
            } else {
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
            qWarning() << "Invalid project directory:" << path;
            return false;
        }

        QString fullPath = path + "/" + configDirName + "/" + indexFileName;
        QFile file(fullPath);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open index file";
            return false;
        }

        QByteArray fileData = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(fileData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "Failed to parse project index JSON:" << parseError.errorString();
            return false;
        }

        QWriteLocker indexLocker(&m_indexLock);
        m_projectPath = path;
        m_projectIndex = doc.object();
        rebuildSemanticCache();

        return true;
    }

    void ProjectManager::saveProject() {
        QMutexLocker dataLocker(&m_dataMutex);
        QReadLocker indexLocker(&m_indexLock);

        if (m_projectPath.isEmpty()) return;

        QString fullPath = m_projectPath + "/" + configDirName + "/" + indexFileName;
        QFile file(fullPath);

        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(m_projectIndex);
            file.write(doc.toJson());
            file.close();
        } else {
            qWarning() << "Failed to open index file for writing:" << fullPath;
        }
    }

    bool ProjectManager::cloneFromVCS(const QString &url, const QString &path) {
        qDebug() << "Cloning project from" << url << "to" << path;
        // Пустая реализация для курсового проекта
        return true;
    }

    QJsonObject ProjectManager::getIndex() {
        QReadLocker indexLocker(&m_indexLock);
        return m_projectIndex;
    }

    void ProjectManager::updateIndex(const QJsonObject &newIndex) {
        QWriteLocker indexLocker(&m_indexLock);
        m_projectIndex = newIndex;
        rebuildSemanticCache();
    }

    void ProjectManager::reindexAll() {
        // Заготовка для полного обхода файлов проекта
    }

    QString ProjectManager::getProjectRoot() const {
        QMutexLocker dataLocker(&m_dataMutex);
        return m_projectPath;
    }

    TokenType ProjectManager::getSemanticType(const QString &identifier) const {
        QReadLocker indexLocker(&m_indexLock);

        if (m_cachedClasses.contains(identifier)) {
            return TokenType::Keyword; // Временный фоллбэк, сверь с syntaxrules.h
        }
        if (m_cachedMethods.contains(identifier)) {
            return TokenType::Identifier; // Временный фоллбэк, сверь с syntaxrules.h
        }

        return TokenType::Identifier;
    }

    void ProjectManager::processFile(const QString &filePath) {
        Q_UNUSED(filePath);
        // Заготовка для интеграции с парсером (обновление индекса по файлу)
    }

    void ProjectManager::rebuildSemanticCache() {
        // Блокировка не нужна, так как метод вызывается уже из-под QWriteLocker
        m_cachedClasses.clear();
        m_cachedMethods.clear();

        QJsonObject indexMap = m_projectIndex["index"].toObject();

        QJsonArray classesArray = indexMap["classes"].toArray();
        for (const QJsonValue &val : classesArray) {
            m_cachedClasses.insert(val.toString());
        }

        QJsonArray methodsArray = indexMap["methods"].toArray();
        for (const QJsonValue &val : methodsArray) {
            m_cachedMethods.insert(val.toString());
        }
    }

} // namespace gs