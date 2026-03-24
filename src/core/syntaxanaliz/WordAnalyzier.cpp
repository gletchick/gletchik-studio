#include "core/syntaxanaliz/wordanalyzier.h"
#include <algorithm>
#include <QDebug>

namespace gs {

    WordAnalyzer::WordAnalyzer(const std::vector<HighlightRule>& rules) {
        m_wordElementRegex = QRegularExpression(constants::WORD_AND_NUMBER_REGEX_PATTERN);
        m_wordElementRegex.optimize();

        for (const auto& rule : rules) {
            QString pattern = QString::fromStdString(rule.regexPattern);
            QRegularExpression re(pattern);
            if (re.isValid()) {
                re.optimize();
                m_compiledRules.push_back({std::move(re), rule.style});
            }
        }
    }

    std::vector<Token> WordAnalyzer::analyzeLine(const QString& text) const {
        if (text.isEmpty()) return {};

        std::vector<Token> allFoundTokens;

        for (const auto& rule : m_compiledRules) {
            auto it = rule.regex.globalMatch(text);
            while (it.hasNext()) {
                auto match = it.next();
                allFoundTokens.push_back({
                    rule.type,
                    (match.capturedStart()),
                    (match.capturedLength()),
                    match.captured(0)
                });
            }
        }

        std::sort(allFoundTokens.begin(), allFoundTokens.end(), [](const Token& a, const Token& b) {
            if (a.start != b.start) return a.start < b.start;
            return a.length > b.length;
        });

        std::vector<Token> finalTokens;
        int lastPos = 0;

        for (const auto& token : allFoundTokens) {
            if (token.start >= lastPos) {
                finalTokens.push_back(token);
                lastPos = token.start + token.length;
            }
        }

        if (!finalTokens.empty()) {
            qDebug() << "[Analyzer] Line analyzed. Found tokens:" << finalTokens.size() << "Text context:" << text.left(20) << "...";
        }

        return finalTokens;
    }

    bool WordAnalyzer::isStaticKeyword(const QString& word, TokenType& outType) const {
        for (const auto& rule : m_compiledRules) {
            if (rule.regex.match(word).hasMatch()) {
                outType = rule.type;
                return true;
            }
        }
        return false;
    }

} // namespace gs