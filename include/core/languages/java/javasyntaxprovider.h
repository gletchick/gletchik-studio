#pragma once
#include "sdk/syntaxrules.h"

namespace gs {

    class JavaSyntaxProvider : public ISyntaxProvider {
    public:
        JavaSyntaxProvider() = default;
        ~JavaSyntaxProvider() override = default;

        std::vector<HighlightRule> getSyntaxRules() const override;
    };

} // namespace gs