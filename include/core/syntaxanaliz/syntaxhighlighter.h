#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QHash>
#include <vector>
#include <memory>

#include "sdk/syntaxrules.h"

namespace gs {

    class WordAnalyzier;

    class SyntaxHighlighter : public QSyntaxHighlighter {
        Q_OBJECT

    public:
        explicit SyntaxHighlighter(QTextDocument *parent = nullptr);
        ~SyntaxHighlighter() override;

        void setRules(const std::vector<HighlightRule>& rules);

    protected:
        void highlightBlock(const QString &text) override;

    private:
        void setupFormats();

        std::shared_ptr<WordAnalyzier> m_analyzer;
        QHash<TokenType, QTextCharFormat> m_formats;
    };

} // namespace gs