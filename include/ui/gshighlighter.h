#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

namespace gs {

    class GSHighlighter : public QSyntaxHighlighter {
    public:
        GSHighlighter(QTextDocument* parent, std::shared_ptr<WordAnalyzier> analyzer);

    protected:
        void highlightBlock(const QString& text) override;

    private:
        std::shared_ptr<WordAnalyzier> m_analyzer;
        
        // Мапа для хранения стилей (цвет, жирность) для каждого типа токена
        QHash<TokenType, QTextCharFormat> m_formats;
        void setupFormats();
    };

} // namespace gs