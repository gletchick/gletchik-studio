#include <QFileInfo>

#include "gui/fileview.h"
#include "core/project/filemanager.h"
#include <QFont>
#include <QPalette>

#include "core/execution/pluginmanager.h"

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

        m_highlighter = new SyntaxHighlighter(this->document());

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

    // FileView.cpp

    bool FileView::loadFile(const QString &filePath) {
        qDebug() << "[FileView] Loading file:" << filePath;

        QString content = FileManager::readFile(filePath);

        // Проверяем только наши маркеры ошибок, а не пустоту строки!
        if (content.startsWith("__ERROR_")) {
            qDebug() << "[FileView] Failed to load file due to FileManager error.";
            return false;
        }

        m_filePath = filePath;

        QFileInfo fileInfo(filePath);
        QString extension = "." + fileInfo.suffix();

        this->setPlainText(content);

        auto provider = PluginManager::getProviderByExtension(extension.toStdString());

        if (provider) {
            qDebug() << "[FileView] Provider found for extension:" << extension
                 << "Language:" << QString::fromStdString(provider->languageName());

            auto syntaxProvider = provider->getSyntaxProvider();

            if (syntaxProvider) {
                this->applySyntaxRules(syntaxProvider->getSyntaxRules());
            } else {
                qWarning() << "SyntaxProvider is null for extension:" << extension;
                this->applySyntaxRules({}); // Сбрасываем в пустые правила
            }
        } else {
            qWarning() << "[FileView] NO PROVIDER FOUND for extension:" << extension;

            this->applySyntaxRules({});
        }

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

    void FileView::rehighlight() {
        if (m_highlighter) {
            m_highlighter->rehighlight();
        }
    }

    void FileView::applySyntaxRules(const std::vector<HighlightRule>& rules) {
        if (m_highlighter) {
            m_highlighter->setRules(rules);
        }
    }
} // namespace gs