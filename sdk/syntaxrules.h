#pragma once
#include <string>
#include <vector>

namespace gs {

    enum class TextStyle {
        Keyword, Type, String, Comment, Number
    };

    struct HighlightRule {
        std::string regexPattern;
        TextStyle style;
    };

    class ISyntaxProvider {
    public:
        virtual ~ISyntaxProvider() = default;
        virtual std::vector<HighlightRule> getSyntaxRules() const = 0;
    };

} // namespace gs