#include "gui/fileview.h"
#include "core/project/filemanager.h"
#include <QFont>
#include <QPalette>

namespace gs {

    FileView::FileView(QWidget *parent) : QPlainTextEdit(parent) {
        // 1. Настройка темной темы (палитра)
        QPalette p = this->palette();

        // Цвет фона — черный (или очень темно-серый для мягкости)
        p.setColor(QPalette::Base, QColor(30, 30, 30));

        // Цвет текста — светло-серый
        p.setColor(QPalette::Text, QColor(212, 212, 212));

        // Цвет выделения (когда ведешь мышкой)
        p.setColor(QPalette::Highlight, QColor(38, 79, 120));
        p.setColor(QPalette::HighlightedText, Qt::white);

        this->setPalette(p);

        // 2. Настройка шрифта
        QFont font("Monospace", 10);
        font.setStyleHint(QFont::Monospace);
        this->setFont(font);

        // Настройка табуляции
        this->setTabStopDistance(this->fontMetrics().horizontalAdvance(' ') * 4);

        // Убираем рамку, чтобы выглядело чище
        this->setFrameStyle(QFrame::NoFrame);
    }

    bool FileView::loadFile(const QString &filePath) {
        if (filePath.isEmpty()) return false;

        QString content = FileManager::readFile(filePath);
        this->setPlainText(content);
        m_filePath = filePath;

        this->document()->setModified(false);
        return true;
    }

    bool FileView::saveCurrentFile() {
        if (m_filePath.isEmpty()) return false;

        QString content = this->toPlainText();
        bool success = FileManager::saveFile(m_filePath, content);

        if (success) {
            this->document()->setModified(false);
        }

        return success;
    }

} // namespace gs