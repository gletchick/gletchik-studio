#pragma once

#include <vector>
#include <QString>
#include <QRegularExpression>
#include <QHash>

#include "sdk/syntaxrules.h"

namespace gs {

    namespace constants {
        const QString WORD_AND_NUMBER_REGEX_PATTERN = R"(\b[A-Za-z_]\w*\b|\b\d+\b)";
    }

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

    class WordAnalyzer {
    public:
        WordAnalyzer(const std::vector<HighlightRule>& rules);
        ~WordAnalyzer() = default;

        std::vector<Token> analyzeLine(const QString& text) const;

    private:
        bool isStaticKeyword(const QString& word, TokenType& outType) const;

        QHash<QString, TokenType> m_staticKeywords;
        std::vector<CompiledRule> m_compiledRules;
        QRegularExpression m_wordElementRegex;
    };

} // namespace gs