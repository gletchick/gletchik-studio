#include "core/syntaxanaliz/wordanalyzier.h"
#include "core/project/projectmanager.h"
#include "sdk/syntaxrules.h"

namespace gs {

    WordAnalyzier::WordAnalyzier(const std::vector<HighlightRule>& rules) {
        for (const auto& rule : rules) {
            QString pattern = QString::fromStdString(rule.regexPattern);


            if (pattern.startsWith("\\b") && pattern.endsWith("\\b") && !pattern.contains("|")) {
                QString word = pattern.mid(2, pattern.length() - 4);
                m_staticKeywords.insert(word, rule.style);
            } else {
                QRegularExpression re(pattern);
                if (re.isValid()) {
                    m_compiledRules.push_back({std::move(re), rule.style});
                }
            }
        }

        m_wordElementRegex = QRegularExpression(R"(\b[A-Za-z_]\w*\b|\b\d+\b)");
    }

    std::vector<Token> WordAnalyzier::analyzeLine(const QString& text) {
        std::vector<Token> tokens;

        auto it = m_wordElementRegex.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QString captured = match.captured(0);

            TokenType type = TokenType::Identifier;

            if (isStaticKeyword(captured, type)) {
                tokens.push_back({type, match.capturedStart(), match.capturedLength(), captured});
                continue;
            }

            bool isNumber = false;
            captured.toInt(&isNumber);
            if (isNumber) {
                tokens.push_back({TokenType::Number, match.capturedStart(), match.capturedLength(), captured});
                continue;
            }

            // Внимание: этот вызов все еще является узким местом при многопоточном парсинге
            type = ProjectManager::instance().getSemanticType(captured);
            tokens.push_back({type, match.capturedStart(), match.capturedLength(), captured});
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
            if (rule.regex.match(word).hasMatch()) {
                outType = rule.type;
                return true;
            }
        }
        return false;
    }

} // namespace gs