#include "core/syntaxanaliz/wordanalyzier.h"
#include "core/project/projectmanager.h"

namespace gs {

    WordAnalyzier::WordAnalyzier(const std::vector<HighlightRule>& rules) {
        m_wordElementRegex = QRegularExpression(constants::WORD_AND_NUMBER_REGEX_PATTERN);
        m_wordElementRegex.optimize();

        for (const auto& rule : rules) {
            QString pattern = QString::fromStdString(rule.regexPattern);

            if (pattern.startsWith("\\b") && pattern.endsWith("\\b")) {
                QString cleanWord = pattern.mid(2, pattern.length() - 4);
                m_staticKeywords[cleanWord] = rule.style;
            } else {
                QRegularExpression re(pattern);
                if (re.isValid()) {
                    re.optimize();
                    m_compiledRules.push_back({std::move(re), rule.style});
                }
            }
        }
    }

    std::vector<Token> WordAnalyzier::analyzeLine(const QString& text) {
        if (text.isEmpty()) {
            return {};
        }

        std::vector<Token> tokens;
        auto it = m_wordElementRegex.globalMatch(text);

        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QString captured = match.captured(0);

            qDebug() << "Found word:" << captured << "at" << match.capturedStart();

            TokenType type = TokenType::Identifier;

            if (isStaticKeyword(captured, type)) {
                tokens.push_back({type, static_cast<int>(match.capturedStart()), static_cast<int>(match.capturedLength()), captured});
                continue;
            }

            bool isNumber = false;
            captured.toInt(&isNumber);
            if (isNumber) {
                tokens.push_back({TokenType::Number, static_cast<int>(match.capturedStart()), static_cast<int>(match.capturedLength()), captured});
                continue;
            }

            // Безопасное обращение к синглтону
            type = ProjectManager::instance().getSemanticType(captured);
            tokens.push_back({type, static_cast<int>(match.capturedStart()), static_cast<int>(match.capturedLength()), captured});
            qDebug() << "Assigned type:" << static_cast<int>(type);
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
            // Здесь собака зарыта
            if (rule.regex.match(word).hasMatch()) {
                outType = rule.type;
                return true;
            }
        }
        return false;
    }

} // namespace gs