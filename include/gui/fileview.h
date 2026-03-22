#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QWidget>

#include "core/syntaxanaliz/syntaxhighlighter.h"

namespace gs {

    class FileView : public QPlainTextEdit {
        Q_OBJECT

    public:
        explicit FileView(QWidget *parent = nullptr);

        bool loadFile(const QString &filePath);

        void applySyntaxRules(const std::vector<HighlightRule>& rules);

        bool saveCurrentFile();

        QString currentFilePath() const { return m_filePath; }

    private:
        QString m_filePath;
        SyntaxHighlighter *m_highlighter;
    };

} // namespace gs