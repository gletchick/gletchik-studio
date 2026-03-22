#pragma once
#include <QString>

namespace gs {
    class FileManager {
    public:
        static QString readFile(const QString &filePath);

        static bool saveFile(const QString &filePath, const QString &content);
    };
}