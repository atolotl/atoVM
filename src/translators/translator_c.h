#ifndef ATOVM_TRANSLATOR_C__H
#define ATOVM_TRANSLATOR_C__H

#include "codegen/instructions.h"
#include "lexer.h"

namespace atovm {
    class CTranslator {
    public:
        CTranslator() = default;
    
    public:
        static auto GetInstance() -> std::shared_ptr<CTranslator>;

    public:
        auto To(const std::vector<InstructionTypes>& buf) -> std::string;

    private:
        static std::shared_ptr<CTranslator> m_instance;
    };
}

#endif // !ATOVM_TRANSLATOR_C__H