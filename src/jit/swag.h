#ifndef ATOVM_SWAG__H
#define ATOVM_SWAG__H

#include <xbyak/xbyak.h>

#include "prelude.h"
#include "codegen/instructions.h"

#include <vector>

namespace atovm {
    class SwagJIT : public Xbyak::CodeGenerator {
    public:
        SwagJIT(const std::vector<InstructionTypes>& prog);

    private:
        std::vector<InstructionTypes> m_program;
    };
}

#endif // !ATOVM_SWAG__H