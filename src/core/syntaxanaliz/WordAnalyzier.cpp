#include "core/syntaxanaliz/wordanalyzier.h"
#include <algorithm>
#include <QDebug>

namespace gs {

    WordAnalyzier::WordAnalyzier(const std::vector<HighlightRule>& rules) {
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

    std::vector<Token> WordAnalyzier::analyzeLine(const QString& text) {
        if (text.isEmpty()) return {};

        std::vector<Token> allFoundTokens;

        // 1. Собираем абсолютно все совпадения по всем правилам
        for (const auto& rule : m_compiledRules) {
            auto it = rule.regex.globalMatch(text);
            while (it.hasNext()) {
                auto match = it.next();
                allFoundTokens.push_back({
                    rule.type,
                    static_cast<int>(match.capturedStart()),
                    static_cast<int>(match.capturedLength()),
                    match.captured(0)
                });
            }
        }

        // 2. Сортируем токены по позиции начала, а при равенстве — по длине (длинные важнее)
        std::sort(allFoundTokens.begin(), allFoundTokens.end(), [](const Token& a, const Token& b) {
            if (a.start != b.start) return a.start < b.start;
            return a.length > b.length;
        });

        // 3. Фильтруем пересечения (алгоритм "жадного" выбора)
        std::vector<Token> finalTokens;
        int lastPos = 0;

        for (const auto& token : allFoundTokens) {
            // Если начало текущего токена больше или равно позиции конца предыдущего
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

    bool WordAnalyzier::isStaticKeyword(const QString& word, TokenType& outType) {
        // Оставляем как вспомогательный метод, если понадобится прямая проверка слова
        for (const auto& rule : m_compiledRules) {
            if (rule.regex.match(word).hasMatch()) {
                outType = rule.type;
                return true;
            }
        }
        return false;
    }

} // namespace gs