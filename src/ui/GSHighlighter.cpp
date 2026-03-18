
#include "core/syntaxanaliz/wordanalyzier.h"
#include "ui/gshighlighter.h"

namespace gs {

    GSHighlighter::GSHighlighter(QTextDocument* parent, std::shared_ptr<WordAnalyzier> analyzer)
        : QSyntaxHighlighter(parent), m_analyzer(analyzer) 
    {
        setupFormats();
    }

    void GSHighlighter::setupFormats() {
        // Настраиваем цвета (можно потом вынести в настройки темы)
        m_formats[TokenType::Keyword].setForeground(Qt::darkBlue);
        m_formats[TokenType::Keyword].setFontWeight(QFont::Bold);

        m_formats[TokenType::Type].setForeground(Qt::darkCyan);

        m_formats[TokenType::Number].setForeground(Qt::darkMagenta);

        m_formats[TokenType::ProjectClass].setForeground(Qt::darkRed);
        m_formats[TokenType::ProjectClass].setFontWeight(QFont::Bold);

        m_formats[TokenType::ProjectMethod].setForeground(Qt::darkYellow);
        m_formats[TokenType::ProjectMethod].setFontItalic(true);

        m_formats[TokenType::Identifier].setForeground(Qt::black);
    }

    void GSHighlighter::highlightBlock(const QString& text) {
        // Вызываем наш анализатор для текущей строки (блока)
        auto tokens = m_analyzer->analyzeLine(text);

        for (const auto& token : tokens) {
            if (m_formats.contains(token.type)) {
                setFormat(token.start, token.length, m_formats[token.type]);
            }
        }
    }

} // namespace gs