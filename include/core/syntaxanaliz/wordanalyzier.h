#pragma once

#include <vector>
#include <QString>
#include <QRegularExpression>
#include <QHash>

#include "sdk/syntaxrules.h"

namespace gs {

    struct Token {
        TokenType type;
        int start;
        int length;
        QString value;
    };

    struct CompiledRule {
        QRegularExpression regex;
        TokenType type;
    };

    class WordAnalyzier {
    public:
        WordAnalyzier(const std::vector<HighlightRule>& rules);
        std::vector<Token> analyzeLine(const QString& text);

    private:
        bool isStaticKeyword(const QString& word, TokenType& outType);

        QHash<QString, TokenType> m_staticKeywords;
        std::vector<CompiledRule> m_compiledRules;
        QRegularExpression m_wordElementRegex;
    };

} // namespace gs