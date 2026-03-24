#include "core/syntaxanaliz/syntaxhighlighter.h"
#include "core/syntaxanaliz/wordanalyzier.h"
#include "core/project/projectmanager.h"

namespace gs {

    namespace colors {
        // Основной текст
        const QColor IDENTIFIER = QColor(230, 230, 230);

        // Ключевые слова (if, while, class)
        const QColor KEYWORD = QColor(255, 121, 198);

        // Типы данных (int, String)
        const QColor TYPE = QColor(139, 233, 253);

        // Строки
        const QColor STRING = QColor(241, 250, 140);

        // Комментарии
        const QColor COMMENT = QColor(98, 114, 164);

        // Числа
        const QColor NUMBER = QColor(255, 184, 108);

        // Классы проекта
        const QColor PROJECT_CLASS = QColor(80, 250, 123);

        // Методы
        const QColor PROJECT_METHOD = QColor(189, 147, 249);

        // Поля проекта
        const QColor PROJECT_FIELD = QColor(137, 221, 255);
    }

    SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
        : QSyntaxHighlighter(parent) {
        setupFormats();
    }

    SyntaxHighlighter::~SyntaxHighlighter() = default;

    void SyntaxHighlighter::setRules(const std::vector<HighlightRule>& rules) {
        m_analyzer = std::make_shared<WordAnalyzer>(rules);
        rehighlight();
    }

    void SyntaxHighlighter::highlightBlock(const QString &text) {
        if (!m_analyzer || text.isEmpty()) {
            return;
        }

        const std::vector<Token> tokens = m_analyzer->analyzeLine(text);

        for (const auto& token : tokens) {
            TokenType actualType = token.type;
            qDebug() << "[Highlighter] Applying format for type:" << (int)token.type << "to word:" << token.value;
            if (actualType == TokenType::Identifier) {
                TokenType semanticType = ProjectManager::instance().getSemanticType(token.value);

                if (semanticType != TokenType::Identifier) {
                    actualType = semanticType;
                }
            }

            QTextCharFormat format = m_formats.value(actualType, m_formats[TokenType::Identifier]);
            setFormat(token.start, token.length, format);
        }
    }

    void SyntaxHighlighter::setupFormats() {
        auto createFormat = [](const QColor& color) {
            QTextCharFormat f;
            f.setForeground(color);
            return f;
        };

        m_formats[TokenType::Keyword] = createFormat(colors::KEYWORD);
        m_formats[TokenType::Type] = createFormat(colors::TYPE);
        m_formats[TokenType::String] = createFormat(colors::STRING);
        m_formats[TokenType::Comment] = createFormat(colors::COMMENT);
        m_formats[TokenType::Number] = createFormat(colors::NUMBER);
        m_formats[TokenType::Identifier] = createFormat(colors::IDENTIFIER);
        m_formats[TokenType::ProjectClass] = createFormat(colors::PROJECT_CLASS);
        m_formats[TokenType::ProjectMethod] = createFormat(colors::PROJECT_METHOD);
        m_formats[TokenType::ProjectField] = createFormat(colors::PROJECT_FIELD);
    }

} // namespace gs