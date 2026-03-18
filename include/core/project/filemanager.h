#pragma once
#include <QString>

namespace gs {
    class FileManager {
    public:
        // Чтение файла для отображения в редакторе
        static QString readFile(const QString &filePath);

        // Сохранение файла с обновлением индекса
        static bool saveFile(const QString &filePath, const QString &content);
    };
}