#include "x86_state.h"

#include <stack>

namespace atovm {
    auto x86Interpreter::L() -> x86Label {
        return { .position = m_ctx->rsp };
    }

    auto x86Interpreter::jmp(const x86Label& lbl) -> void {
        m_ctx->rsp = lbl.position;
    }
    
    auto x86Interpreter::Execute() -> i32 {
        std::stack<x86Label> loop_stack;

        if (m_program.empty()) {
            fmt::println("[x86i] no prohgmfadnjkas 💔");
            return 0xDEADBEEF;
        }

        if (m_ctx->stack.empty()) {
            m_ctx->stack.resize(30000, 0);
        }

        bool has_exit = false;

        m_ctx->rsp = 0;
        while (m_ctx->rsp < m_program.size()) {
            const auto& it = m_program[m_ctx->rsp];

            switch (it.index()) {
                case AtoVM_Increment: {
                    m_ctx->stack[m_ctx->rax]++;
                    break;
                }
                
                case AtoVM_Add: {
                    auto* ato_add = std::get_if<Add>(&it);
                    m_ctx->stack[m_ctx->rax] += ato_add->count;
                    
                    break;
                }

                case AtoVM_Decrement: {
                    m_ctx->stack[m_ctx->rax]--;
                    break;
                }

                case AtoVM_Sub: {
                    auto* ato_sub = std::get_if<Sub>(&it);
                    m_ctx->stack[m_ctx->rax] -= ato_sub->count;

                    break;
                }

                case AtoVM_Mul: {
                    auto* ato_mul = std::get_if<Mul>(&it);
                    m_ctx->stack[m_ctx->rax] *= ato_mul->count;

                    break;
                }

                case AtoVM_ForwardCell: {
                    m_ctx->rax++;
                
                    if (m_ctx->rax >= m_ctx->stack.size()) {
                        m_ctx->stack.resize(m_ctx->rax + 1);
                    }

                    break;
                }

                case AtoVM_ForwardMany: {
                    auto* ato_fwd = std::get_if<ForwardMany>(&it);
                    m_ctx->rax += ato_fwd->count;
                
                    if (m_ctx->rax >= m_ctx->stack.size()) {
                        m_ctx->stack.resize(m_ctx->rax + 1);
                    }

                    break;
                }

                case AtoVM_BackwardCell: {
                    if (m_ctx->rax > 0) {
                        m_ctx->rax--;
                    } else {
                        fmt::println("[x86i] error: attempted to move out of bounds");
                        return 0xDEADBEEF;
                    }

                    break;
                }

                case AtoVM_BackwardMany: {
                    auto* ato_bwd = std::get_if<BackwardMany>(&it);

                    if (m_ctx->rax > 0) {
                        m_ctx->rax -= ato_bwd->count;
                    } else {
                        fmt::println("[x86i] error: attempted to move out of bounds");
                        return 0xDEADBEEF;
                    }

                    break;
                }

                case AtoVM_LoopLabel: {
                    if (m_ctx->stack[m_ctx->rax] == 0) {
                        i64 depth = 1;
                        while (depth > 0 && ++m_ctx->rsp < m_program.size()) {
                            if (std::get_if<LoopLabel>(&m_program[m_ctx->rsp])) depth++;
                            else if (std::get_if<JumpNotZero>(&m_program[m_ctx->rsp])) depth--;
                        }
                    } else {
                        loop_stack.push(L());
                    }

                    break;
                }

                case AtoVM_JumpNotZero: {
                    if (loop_stack.empty()) {
                        fmt::println("[x86i] error: no endloop?");
                        return 0xDEADBEEF;
                    }
    
                    if (m_ctx->stack[m_ctx->rax] != 0) {
                        jmp(loop_stack.top());
                    } else {
                        loop_stack.pop();
                    }

                    break;
                }

                case AtoVM_Print: {
                    putchar(m_ctx->stack[m_ctx->rax]);
                    break;
                }

                case AtoVM_Scan: {
                    m_ctx->stack[m_ctx->rax] = getchar();
                    break;
                }

                case AtoVM_Exit: {
                    return m_ctx->stack[m_ctx->rax];
                }
            }

            m_ctx->rsp++;
        }

        return m_ctx->stack[m_ctx->rax];
    }
}