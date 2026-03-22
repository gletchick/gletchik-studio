#include <filesystem>
#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

#include "core/execution/pluginmanager.h"
#include "core/project/filemanager.h"
#include "core/project/projectmanager.h"
#include "core/utils/utils.h"

namespace gs {
    // В core/project/FileManager.cpp
    QString FileManager::readFile(const QString &filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "CRITICAL: Could not open file for reading:" << filePath;
            return QString();
        }
        QTextStream in(&file);
        QString content = in.readAll();
        qDebug() << "SUCCESS: Read" << content.length() << "characters from" << filePath;
        return content;
    }

    bool FileManager::saveFile(const QString &filePath, const QString &content) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Failed to save file:" << filePath;
            return false;
        }
        QTextStream out(&file);
        out << content;
        file.close();

        ProjectManager::instance().processFile(filePath);
        ProjectManager::instance().saveProject();

        return true;
    }

}