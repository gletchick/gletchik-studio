#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDebug>

#include "core/project/projectmanager.h"

namespace gs {

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
        // Порядок захвата мьютексов теперь строго совпадает с openProject
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

} // namespace gs