#include "swag.h"

#include <stack>

namespace atovm {
    // Based on https://github.com/herumi/xbyak/blob/master/sample/bf.cpp
    SwagJIT::SwagJIT(const std::vector<InstructionTypes>& prog)
    : CodeGenerator(100000) {
        bool has_exit = false;
        m_program = prog; 

        using namespace Xbyak;
        #if defined(XBYAK64_WIN)
		    const Reg64& pPutchar(rsi);
		    const Reg64& pGetchar(rdi);
		    const Reg64& stack(rbp);

		    const Address cur = byte [stack];

		    push(rsi);
		    push(rdi);
		    push(rbp);

		    mov(pPutchar, rcx);
		    mov(pGetchar, rdx);

		    mov(stack, r8);
        #else
		    const Reg64& pPutchar(rbx);
		    const Reg64& pGetchar(rbp);
		    const Reg64& stack(r12);

		    const Address cur = byte [stack];

		    push(rbx);
		    push(rbp);
		    push(r12);

		    mov(pPutchar, rdi);
		    mov(pGetchar, rsi);

		    mov(stack, rdx);
        #endif
        
        std::stack<Label> labelF, labelB;
        for (const auto& it : m_program) {
            switch (it.index()) {
                case AtoVM_Increment: {
                    inc(cur);
                    break;
                }

                case AtoVM_Add: {
                    auto* ato_add = std::get_if<Add>(&it);
                    add(cur, ato_add->count);

                    break;
                }
                
                case AtoVM_Decrement: {
                    dec(cur);
                    break;
                }

                case AtoVM_Sub: {
                    auto* ato_sub = std::get_if<Sub>(&it);
                    sub(cur, ato_sub->count);

                    break;
                }

                case AtoVM_Mul: {
                    auto* ato_mul = std::get_if<Mul>(&it);

                    mov(al, cur);
                    mov(bl, ato_mul->count);
                    imul(al, bl);
                    mov(cur, al);
                    break;
                }
    
                case AtoVM_ForwardCell: {
                    inc(stack);
                    break;
                }

                case AtoVM_ForwardMany: {
                    auto* ato_fwd = std::get_if<ForwardMany>(&it);
                    add(stack, ato_fwd->count);

                    break;
                }
                
                case AtoVM_BackwardCell: {
                    dec(stack);
                    break;
                }

                case AtoVM_BackwardMany: {
                    auto* ato_bwd = std::get_if<BackwardMany>(&it);
                    sub(stack, ato_bwd->count);

                    break;
                }
    
                case AtoVM_LoopLabel: {
                    Label B = L();
				    labelB.push(B);
				    movzx(eax, cur);
				    test(eax, eax);

				    Label F;
				    jz(F, T_NEAR);
				    labelF.push(F);
                    break;
                }
    
                case AtoVM_JumpNotZero: {
                    Label B = labelB.top(); labelB.pop();
				    jmp(B);

				    Label F = labelF.top(); labelF.pop();
				    L(F);
                    break;
                }
    
                case AtoVM_Print: {
                    #if defined(XBYAK64_WIN)
				        movzx(ecx, cur);
				        sub(rsp, 32);
				        call(pPutchar);
				        add(rsp, 32);
                    #else                   
				        movzx(edi, cur);                    
				        call(pPutchar);
                    #endif
                    break;
                }
    
                case AtoVM_Scan: {
                    #if defined(XBYAK64_GCC)
				        call(pGetchar);
                    #elif defined(XBYAK64_WIN)
                    	sub(rsp, 32);
                    	call(pGetchar);
                    	add(rsp, 32);
                    #endif

                    mov(cur, al);
                    break;
                }
    
                case AtoVM_Exit: {
                    has_exit = true;
                    mov(al, cur);

                    #if defined(XBYAK64_WIN)
		                pop(rbp);
		                pop(rdi);
		                pop(rsi);
                    #else
                    	pop(r12);
                    	pop(rbp);
                    	pop(rbx);
                    #endif

                    ret();
                    
                    break;
                }
            }
        }

        if (!has_exit) {
            #if defined(XBYAK64_WIN)
		       pop(rbp);
		       pop(rdi);
		       pop(rsi);
            #else
            	pop(r12);
            	pop(rbp);
            	pop(rbx);
            #endif
            
            ret();
        }
    }     
}