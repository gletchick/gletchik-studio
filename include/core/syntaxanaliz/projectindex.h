#pragma once

#include <QString>
#include <QJsonObject>
#include <QMap>
#include <QSet>
#include <QDateTime>

namespace gs {

    struct FileData {
        QString hash;
        QSet<QString> classes;
        QSet<QString> methods;
    };

    class ProjectIndex {
    public:
        ProjectIndex() = default;

        // Загрузка из JSON (при открытии проекта)
        void loadFromJson(const QJsonObject &root);

        // Преобразование в JSON (для сохранения на диск)
        QJsonObject toJson() const;

        // Обновление данных одного файла
        void updateFile(const QString &filePath, const QString &hash,
                        const QSet<QString> &classes, const QSet<QString> &methods);

        // Быстрая проверка для подсветки
        bool isClass(const QString &name) const;
        bool isMethod(const QString &name) const;

    private:
        // Хранение данных по файлам: Путь -> Данные
        QMap<QString, FileData> m_fileEntries;

        // Глобальные кеши для мгновенного поиска (Sync с m_fileEntries)
        QSet<QString> m_globalClasses;
        QSet<QString> m_globalMethods;

        void rebuildGlobalIndex();

        // Константы для JSON
        const QString kFiles = "files";
        const QString kHash = "hash";
        const QString kClasses = "classes";
        const QString kMethods = "methods";
        const QString kGlobal = "global_index";
    };

} // namespace gs