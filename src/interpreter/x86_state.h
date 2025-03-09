#ifndef ATOVM_X86_STATE__H
#define ATOVM_X86_STATE__H

#include "prelude.h"
#include "codegen/instructions.h"

#include <vector>

namespace atovm {
    struct x86State {
        u64 rax;
        u64 rcx;

        u64 rsp;
        u64 rbp;

        u64 sf;
        u64 zf;

        std::vector<u8> stack;
    };

    enum eRegisters {
        ATOVM_REG_RAX,
        ATOVM_REG_RCX,
    };

    struct x86Label {
        usize position;
    };

    class x86Interpreter {
    public:
        x86Interpreter(const std::vector<InstructionTypes>& prog) { 
            m_program = prog;
            m_ctx = new x86State { 
                .rax = 0, 
                .rcx = 0, 
                
                .rsp = 0, 
                .rbp = 0, 

                .sf = 0,
                .zf = 0,
                
                .stack = std::vector<u8>(128, 0) 
            }; 
        }
    
    public:
        auto Execute() -> i32;
    
    private:
        auto jmp(const x86Label& lbl) -> void;
        auto L() -> x86Label;

    private:
        x86State* m_ctx;
        std::vector<InstructionTypes> m_program;

        std::vector<x86Label> m_labels;
    };
}

#endif // !ATOVM_X86_STATE__H