#pragma once

#include <QString>
#include <QJsonObject>
#include <QReadWriteLock>
#include <QMutex>
#include <QSet>

#include "sdk/syntaxrules.h"

namespace gs {
    class ProjectManager {
    public:
        static ProjectManager& instance();

        ProjectManager(const ProjectManager&) = delete;
        ProjectManager& operator=(const ProjectManager&) = delete;

        bool createProject(const QString &path);
        bool openProject(const QString &path);
        bool cloneFromVCS(const QString &url, const QString &path);
        void saveProject();

        QJsonObject getIndex();
        void updateIndex(const QJsonObject &newIndex);
        void reindexAll();

        QString getProjectRoot() const;
        TokenType getSemanticType(const QString &identifier) const;

        void processFile(const QString &filePath);

    private:
        ProjectManager() = default;
        ~ProjectManager() = default;

        void rebuildSemanticCache();

        QString m_projectPath;
        QJsonObject m_projectIndex;

        mutable QMutex m_dataMutex;
        mutable QReadWriteLock m_indexLock;

        QSet<QString> m_cachedClasses;
        QSet<QString> m_cachedMethods;

        const QString configDirName = ".gs";
        const QString indexFileName = "project_index.json";
    };

}