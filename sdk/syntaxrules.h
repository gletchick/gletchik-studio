#pragma once
#include <string>
#include <vector>

namespace gs {

    enum class TokenType {
        Keyword,
        Type,
        String,
        Comment,
        Number,
        Identifier,
        ProjectClass,
        ProjectMethod,
        ProjectField
    };

    struct HighlightRule {
        std::string regexPattern;
        TokenType style;
    };

    class ISyntaxProvider {
    public:
        virtual ~ISyntaxProvider() = default;
        virtual std::vector<HighlightRule> getSyntaxRules() const = 0;
    };

} // namespace gs