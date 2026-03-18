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

    inline uint qHash(TokenType key, uint seed) {
        return ::qHash(static_cast<uint>(key), seed);
    }

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