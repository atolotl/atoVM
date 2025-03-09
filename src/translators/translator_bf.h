#ifndef ATOVM_TRANSLATOR_BF__H
#define ATOVM_TRANSLATOR_BF__H

#include "codegen/instructions.h"
#include "lexer.h"

namespace atovm {
    class BrainfuckTranslator {
    public:
        BrainfuckTranslator() = default;
    
    public:
        static auto GetInstance() -> std::shared_ptr<BrainfuckTranslator>;

    public:
        auto To(const std::vector<InstructionTypes>& buf) -> std::string;
        auto From() -> std::vector<InstructionTypes>;

    private:
        static std::shared_ptr<BrainfuckTranslator> m_instance;
    };
}

#endif // !ATOVM_TRANSLATOR_BF__H