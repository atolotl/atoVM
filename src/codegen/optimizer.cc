#include "optimizer.h"

namespace atovm {
    std::shared_ptr<Optimizer> Optimizer::m_instance{nullptr};

    auto Optimizer::GetInstance() -> std::shared_ptr<Optimizer> {
        if (!m_instance)
            m_instance = std::make_shared<Optimizer>();

        return m_instance;
    }

    auto Optimizer::Pass1(const std::vector<InstructionTypes>& inst) -> std::vector<InstructionTypes> {
        std::vector<InstructionTypes> out;
        out.reserve(inst.size());

        for (usize i = 0; i < inst.size(); ) {
            auto& current_inst = inst[i];
            switch (current_inst.index()) {
                case AtoVM_Increment:
                case AtoVM_Decrement:
                case AtoVM_ForwardCell:
                case AtoVM_BackwardCell: {
                    i32 count = 0;
                    auto current_index = current_inst.index();

                    while (i < inst.size() && inst[i].index() == current_index) {
                        count++;
                        i++;
                    }

                    if (current_index == AtoVM_Increment) {
                        if (count == 1) out.push_back(Increment{});
                        else out.push_back(Add{ .count = count });
                    } else if (current_index == AtoVM_Decrement) {
                        if (count == 1) out.push_back(Decrement{});
                        else out.push_back(Sub{ .count = count });
                    } else if (current_index == AtoVM_ForwardCell) {
                        if (count == 1) out.push_back(ForwardCell{});
                        else out.push_back(ForwardMany{ .count = count });
                    } else if (current_index == AtoVM_BackwardCell) {
                        if (count == 1) out.push_back(BackwardCell{});
                        else out.push_back(BackwardMany{ .count = count });
                    }

                    break;
                }

                default: {
                    out.push_back(current_inst); 
                    i++;
                    break;
                }
            }
        }

        return Pass2(out);
    }

    auto Optimizer::Pass2(const std::vector<InstructionTypes>& inst) -> std::vector<InstructionTypes> {
        std::vector<InstructionTypes> out;
        out.reserve(inst.size());

        for (usize i = 0; i < inst.size(); ) {
            auto& inst1 = inst[i];
            if (i + 5 < inst.size()) {
                auto& inst2 = inst[i + 1];
                auto& inst3 = inst[i + 2];
                auto& inst4 = inst[i + 3];
                auto& inst5 = inst[i + 4];
                auto& inst6 = inst[i + 5];

                // look for [->+<] and [-<+>] please
                if (IsLoopLabel(inst1) && 
                    (IsDecrement(inst2) || IsSub(inst2)) &&
                    (IsForwardCell(inst3) || IsForwardMany(inst3) || IsBackwardCell(inst3) || IsBackwardMany(inst3)) &&
                    (IsIncrement(inst4) || IsAdd(inst4)) &&
                    (IsForwardCell(inst5) || IsForwardMany(inst5) || IsBackwardCell(inst5) || IsBackwardMany(inst5)) &&
                    IsJumpNotZero(inst6)) {

                    i32 factor = 1;
                    if (auto* add_op = std::get_if<Add>(&inst4)) {
                        factor = add_op->count;
                    }

                    auto getDirection = [](const InstructionTypes& instruction) -> i32 {
                        if (IsForwardCell(instruction)) return 1;
                        if (IsBackwardCell(instruction)) return -1;

                        if (auto* fwd = std::get_if<ForwardMany>(&instruction)) return fwd->count;
                        if (auto* bwd = std::get_if<BackwardMany>(&instruction)) return -bwd->count;

                        return 0;
                    };

                    i32 direction1 = getDirection(inst3);
                    i32 direction2 = getDirection(inst5);

                    if (direction1 == -direction2) {
                        out.push_back(Mul{ .count = factor });
                        i += 6;
                        continue;
                    }
                }
            }

            out.push_back(inst1);
            i++;
        }

        return out;
    }
}