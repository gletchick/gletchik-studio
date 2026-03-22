#include "core/syntaxanaliz/syntaxhighlighter.h"
#include "core/syntaxanaliz/wordanalyzier.h"

namespace gs {

    namespace colors {
        const QColor KEYWORD = QColor(86, 156, 214);       // Синий (VS Code style)
        const QColor TYPE = QColor(78, 201, 176);          // Бирюзовый
        const QColor STRING = QColor(206, 145, 120);       // Оранжевый
        const QColor COMMENT = QColor(106, 153, 85);       // Зеленый
        const QColor NUMBER = QColor(181, 206, 168);       // Салатовый
        const QColor IDENTIFIER = QColor(212, 212, 212);   // Базовый текст
        const QColor PROJECT_CLASS = QColor(78, 201, 176); 
        const QColor PROJECT_METHOD = QColor(220, 220, 170); // Желтоватый
        const QColor PROJECT_FIELD = QColor(156, 220, 254);  // Светло-голубой
    }

    SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
        : QSyntaxHighlighter(parent) {
        setupFormats();
    }

    SyntaxHighlighter::~SyntaxHighlighter() = default;

    void SyntaxHighlighter::setRules(const std::vector<HighlightRule>& rules) {
        m_analyzer = std::make_shared<WordAnalyzier>(rules);
        rehighlight();
    }

    void SyntaxHighlighter::highlightBlock(const QString &text) {
        // Копируем shared_ptr — теперь объект не умрет, пока мы работаем
        auto currentAnalyzer = m_analyzer;

        if (!currentAnalyzer) {
            return;
        }

        // Работаем с локальной копией
        std::vector<Token> tokens = currentAnalyzer->analyzeLine(text);

        for (const auto& token : tokens) {
            QTextCharFormat format = m_formats.value(token.type, m_formats[TokenType::Identifier]);
            setFormat(token.start, token.length, format);
        }
    }

    void SyntaxHighlighter::setupFormats() {
        QTextCharFormat format;

        format.setForeground(colors::KEYWORD);
        m_formats[TokenType::Keyword] = format;

        format.setForeground(colors::TYPE);
        m_formats[TokenType::Type] = format;

        format.setForeground(colors::STRING);
        m_formats[TokenType::String] = format;

        format.setForeground(colors::COMMENT);
        m_formats[TokenType::Comment] = format;

        format.setForeground(colors::NUMBER);
        m_formats[TokenType::Number] = format;

        format.setForeground(colors::IDENTIFIER);
        m_formats[TokenType::Identifier] = format;

        format.setForeground(colors::PROJECT_CLASS);
        m_formats[TokenType::ProjectClass] = format;

        format.setForeground(colors::PROJECT_METHOD);
        m_formats[TokenType::ProjectMethod] = format;

        format.setForeground(colors::PROJECT_FIELD);
        m_formats[TokenType::ProjectField] = format;
    }

} // namespace gs