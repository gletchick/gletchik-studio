#pragma once
#include <string>
#include "../steptype.h"

namespace gs {

    class ibuildstep {
    public:
        virtual ~ibuildstep() = default;
        virtual std::string name() const = 0;
        virtual steptype type() const = 0;
        virtual bool execute() = 0;
    };

} // namespace gs