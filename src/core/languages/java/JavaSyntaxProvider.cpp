#include "core/languages/java/javasyntaxprovider.h"

namespace gs {

    std::vector<HighlightRule> JavaSyntaxProvider::getSyntaxRules() const {
        std::vector<HighlightRule> rules;

        const std::vector<std::string> keywords = {
            "abstract", "continue", "for", "new", "switch", "assert", "default",
            "if", "package", "synchronized", "do", "goto", "private",
            "this", "break", "double", "implements", "protected", "throw",
            "else", "import", "public", "throws", "case", "enum", "instanceof",
            "return", "transient", "catch", "extends", "short", "try",
            "final", "interface", "static", "void", "finally", "strictfp",
            "volatile", "while", "native", "super"
        };

        for (const auto& word : keywords) {
            rules.push_back({"^" + word + "$", TokenType::Keyword});
        }

        // 2. Встроенные типы (Types)
        const std::vector<std::string> types = {
            "boolean", "byte", "char", "double", "float", "int", "long", 
            "short", "String", "var"
        };

        for (const auto& type : types) {
            rules.push_back({"\\b" + type + "\\b", TokenType::Type});
        }

        // 3. Литералы (null, true, false)
        rules.push_back({"\\b(null|true|false)\\b", TokenType::Keyword});

        // 4. Комментарии (однострочные)
        // Примечание: для многострочных нужна логика в QSyntaxHighlighter
        rules.push_back({"//[^\n]*", TokenType::Comment});

        // 5. Строковые литералы
        rules.push_back({"\"([^\"\\\\]|\\\\.)*\"", TokenType::String});

        return rules;
    }

} // namespace gs