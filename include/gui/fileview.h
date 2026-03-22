#ifndef GLETCHIK_STUDIO_FILEVIEW_H
#define GLETCHIK_STUDIO_FILEVIEW_H

#include <QPlainTextEdit>
#include <QString>
#include <QWidget>

namespace gs {

    class FileView : public QPlainTextEdit {
        Q_OBJECT

    public:
        explicit FileView(QWidget *parent = nullptr);

        // Загрузка контента через FileManager[cite: 1, 2]
        bool loadFile(const QString &filePath);

        // Сохранение текущего контента через FileManager[cite: 1, 2]
        bool saveCurrentFile();

        QString currentFilePath() const { return m_filePath; }

    private:
        QString m_filePath;
    };

} // namespace gs

#endif //GLETCHIK_STUDIO_FILEVIEW_H