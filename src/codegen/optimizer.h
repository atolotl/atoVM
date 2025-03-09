#ifndef ATOVM_OPTIMIZER__H
#define ATOVM_OPTIMIZER__H

#include <vector>

#include "instructions.h"
#include <memory>

namespace atovm {
    class Optimizer {
    public:
        Optimizer() = default;
    
    public:
        static auto GetInstance() -> std::shared_ptr<Optimizer>;

    public:
        inline auto Optimize(const std::vector<InstructionTypes>& inst) -> std::vector<InstructionTypes> { return Pass1(inst); }
        
        auto Pass1(const std::vector<InstructionTypes>& inst) -> std::vector<InstructionTypes>;
        auto Pass2(const std::vector<InstructionTypes>& inst) -> std::vector<InstructionTypes>;
    
    private:
        static std::shared_ptr<Optimizer> m_instance;
    };
}

#endif // !ATOVM_OPTIMIZER__H