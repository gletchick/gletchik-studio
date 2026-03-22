#include "core/syntaxanaliz/wordanalyzier.h"
#include "core/project/projectmanager.h"

namespace gs {

    WordAnalyzier::WordAnalyzier(const std::vector<HighlightRule>& rules) {
        m_wordElementRegex = QRegularExpression(constants::WORD_AND_NUMBER_REGEX_PATTERN);
        m_wordElementRegex.optimize(); // Оптимизация для частых поисков

        for (const auto& rule : rules) {
            QRegularExpression re(QString::fromStdString(rule.regexPattern));
            if (re.isValid()) {
                re.optimize();
                m_compiledRules.push_back({std::move(re), rule.style});
            }
        }
    }

    std::vector<Token> WordAnalyzier::analyzeLine(const QString& text) {
        std::vector<Token> tokens;

        auto it = m_wordElementRegex.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QString captured = match.captured(0);

            TokenType type = TokenType::Identifier;

            if (isStaticKeyword(captured, type)) {
                tokens.push_back({type, (int)match.capturedStart(), (int)match.capturedLength(), captured});
                continue;
            }

            bool isNumber = false;
            captured.toInt(&isNumber);
            if (isNumber) {
                tokens.push_back({TokenType::Number, (int)match.capturedStart(), (int)match.capturedLength(), captured});
                continue;
            }

            // Если ProjectManager уже уничтожен, здесь будет SEGFAULT
            type = ProjectManager::instance().getSemanticType(captured);
            tokens.push_back({type, (int)match.capturedStart(), (int)match.capturedLength(), captured});
        }

        return tokens;
    }

    bool WordAnalyzier::isStaticKeyword(const QString& word, TokenType& outType) {
        auto it = m_staticKeywords.find(word);
        if (it != m_staticKeywords.end()) {
            outType = it.value();
            return true;
        }

        for (const auto& rule : m_compiledRules) {
            // Используем самую простую перегрузку match
            if (rule.regex.match(word).hasMatch()) {
                outType = rule.type;
                return true;
            }
        }
        return false;
    }

} // namespace gs