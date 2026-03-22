#include "core/languages/java/javasyntaxprovider.h"

namespace {
    const char* const KEYWORDS_PATTERN = "\\b(abstract|assert|boolean|break|byte|case|catch|char|class|const|continue|default|do|double|else|enum|extends|final|finally|float|for|goto|if|implements|import|instanceof|int|interface|long|native|new|package|private|protected|public|return|short|static|strictfp|super|switch|synchronized|this|throw|throws|transient|try|void|volatile|while)\\b";
    const char* const TYPES_PATTERN = "\\b(String|Integer|Double|Boolean|Character|Byte|Short|Float|Long|Object)\\b";
    const char* const STRING_PATTERN = "\".*\"";
    const char* const COMMENT_SINGLE_PATTERN = "//[^\n]*";
    const char* const COMMENT_MULTI_PATTERN = "/\\*.*\\*/";
    const char* const NUMBER_PATTERN = "\\b\\d+(\\.\\d+)?\\b";
    const char* const IDENTIFIER_PATTERN = "\\b[A-Za-z_][A-Za-z0-9_]*\\b";
}

namespace gs {

    std::vector<HighlightRule> JavaSyntaxProvider::getSyntaxRules() const {
        std::vector<HighlightRule> rules;

        rules.push_back({KEYWORDS_PATTERN, TokenType::Keyword});
        rules.push_back({TYPES_PATTERN, TokenType::Type});
        rules.push_back({STRING_PATTERN, TokenType::String});
        rules.push_back({COMMENT_SINGLE_PATTERN, TokenType::Comment});
        rules.push_back({COMMENT_MULTI_PATTERN, TokenType::Comment});
        rules.push_back({NUMBER_PATTERN, TokenType::Number});
        rules.push_back({IDENTIFIER_PATTERN, TokenType::Identifier});

        return rules;
    }

} // namespace gs