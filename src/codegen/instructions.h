#ifndef ATOVM_INSTRUCTIONS__H
#define ATOVM_INSTRUCTIONS__H

#include <variant>
#include "prelude.h"

#include <magic_enum/magic_enum.hpp>

#define ATOVM_INST_LIST(X) \
    X(Increment, {})       \
    X(Add, {i32 count;})       \
    X(Decrement, {})       \
    X(Sub, {i32 count;})       \
    X(Mul, {i32 count;})       \
    X(ForwardCell, {})     \
    X(ForwardMany, {i32 count;})       \
    X(BackwardCell, {})    \
    X(BackwardMany, {i32 count;})       \
    X(Print, {})           \
    X(Scan, {})            \
    X(Exit, {})            \
    X(LoopLabel, {})       \
    X(JumpNotZero, {})     \
    X(Unknown, {})

#define ATOVM_INST_COUNT 15

#define ATO_INST_STRUCT(n, v) struct n v;
#define ATO_INST_ENUM_ELEMENT(n, v) AtoVM_##n,
#define ATO_INST_FUR_VARIANT(n, v) n,
#define ATO_INST_IS_CLAUSE(n, v) inline const bool Is##n(const InstructionTypes& inst) { return inst.index() == AtoVM_##n; }

namespace atovm
{
    ATOVM_INST_LIST(ATO_INST_STRUCT)

    enum eInstructionType {
        ATOVM_INST_LIST(ATO_INST_ENUM_ELEMENT)
    };

    using InstructionTypes = std::variant<
        ATOVM_INST_LIST(ATO_INST_FUR_VARIANT) std::nullptr_t
    >;

    ATOVM_INST_LIST(ATO_INST_IS_CLAUSE)

    union __Convert {
        u8 x[4];
        i32 y;
    };

    inline auto EncodeInstruction(const InstructionTypes& ist, std::vector<u8>& buffer) {
        buffer.push_back(static_cast<u8>(ist.index()));

        auto PushValue = [](const __Convert& converter, std::vector<u8>& buffer) {
            for (usize i = 0; i < sizeof(__Convert::x); i++) {
                buffer.push_back(converter.x[i]);
            }
        };
        
        __Convert converter;
        if (auto* ato_add = std::get_if<atovm::Add>(&ist)) {
            converter.y = ato_add->count;
            PushValue(converter, buffer);
        } else if (auto* ato_sub = std::get_if<atovm::Sub>(&ist)) {
            converter.y = ato_sub->count;
            PushValue(converter, buffer);
        } else if (auto* ato_fwd = std::get_if<atovm::ForwardMany>(&ist)) {
            converter.y = ato_fwd->count;
            PushValue(converter, buffer);
        } else if (auto* ato_bwd = std::get_if<atovm::BackwardMany>(&ist)) {
            converter.y = ato_bwd->count;
            PushValue(converter, buffer);
        } else if (auto* ato_mul = std::get_if<atovm::Mul>(&ist)) {
            converter.y = ato_mul->count;
            PushValue(converter, buffer);
        }
    }

    inline auto DecodeInstruction(const std::vector<u8>& buffer, usize& pos) -> InstructionTypes {
        if (pos >= buffer.size()) {
            return std::nullptr_t{};
        }

        u8 type_index = buffer[pos++];

        auto GetValue = [](__Convert& converter, const std::vector<u8>& buffer, usize& pos) {
            for (usize i = 0; i < sizeof(__Convert::x); i++) {
                converter.x[i] = buffer[pos++];
            }
        };
        
        __Convert converter;
        if (type_index == AtoVM_Add) {
            GetValue(converter, buffer, pos);
            return atovm::Add{ .count = converter.y };
        } else if (type_index == AtoVM_Sub) {
            GetValue(converter, buffer, pos);
            return atovm::Sub{ .count = converter.y };
        } else if (type_index == AtoVM_ForwardMany) {
            GetValue(converter, buffer, pos);
            return atovm::ForwardMany{ .count = converter.y };
        } else if (type_index == AtoVM_BackwardMany) {
            GetValue(converter, buffer, pos);
            return atovm::BackwardMany{ .count = converter.y };
        } else if (type_index == AtoVM_Mul) {
            GetValue(converter, buffer, pos);
            return atovm::Mul{ .count = converter.y };
        }
        
        switch (type_index) {
            case AtoVM_Increment: return atovm::Increment{};
            case AtoVM_Decrement: return atovm::Decrement{};
            case AtoVM_ForwardCell: return atovm::ForwardCell{};
            case AtoVM_BackwardCell: return atovm::BackwardCell{};
            case AtoVM_Print: return atovm::Print{};
            case AtoVM_Scan: return atovm::Scan{};
            case AtoVM_Exit: return atovm::Exit{};
            case AtoVM_LoopLabel: return atovm::LoopLabel{};
            case AtoVM_JumpNotZero: return atovm::JumpNotZero{};
            default: return atovm::Unknown{};
        }
    }    
}

template <>
struct fmt::formatter<atovm::InstructionTypes> : formatter<string_view> {
    auto format(atovm::InstructionTypes instruction, fmt::format_context &ctx) const -> fmt::format_context::iterator {
        auto typ = static_cast<atovm::eInstructionType>(instruction.index() > ATOVM_INST_COUNT ? ATOVM_INST_COUNT : instruction.index());

        if (typ != atovm::AtoVM_Add && typ != atovm::AtoVM_Sub && typ != atovm::AtoVM_ForwardMany && typ != atovm::AtoVM_BackwardMany && typ != atovm::AtoVM_Mul) {
            return formatter<string_view>::format(fmt::format("AtoVM {{Instruction: {}}}", magic_enum::enum_name<atovm::eInstructionType>(typ)), ctx);
        } else {
            if (auto* ato_add = std::get_if<atovm::Add>(&instruction)) {
                return formatter<string_view>::format(fmt::format("AtoVM {{Instruction: {}, Parameters: {}}}", magic_enum::enum_name<atovm::eInstructionType>(typ), ato_add->count), ctx);
            } else if (auto* ato_sub = std::get_if<atovm::Sub>(&instruction)) {
                return formatter<string_view>::format(fmt::format("AtoVM {{Instruction: {}, Parameters: {}}}", magic_enum::enum_name<atovm::eInstructionType>(typ), ato_sub->count), ctx);
            } else if (auto* ato_fwd = std::get_if<atovm::ForwardMany>(&instruction)) {
                return formatter<string_view>::format(fmt::format("AtoVM {{Instruction: {}, Parameters: {}}}", magic_enum::enum_name<atovm::eInstructionType>(typ), ato_fwd->count), ctx);
            } else if (auto* ato_bwd = std::get_if<atovm::BackwardMany>(&instruction)) {
                return formatter<string_view>::format(fmt::format("AtoVM {{Instruction: {}, Parameters: {}}}", magic_enum::enum_name<atovm::eInstructionType>(typ), ato_bwd->count), ctx);
            } else if (auto* ato_mul = std::get_if<atovm::Mul>(&instruction)) {
                return formatter<string_view>::format(fmt::format("AtoVM {{Instruction: {}, Parameters: {}}}", magic_enum::enum_name<atovm::eInstructionType>(typ), ato_mul->count), ctx);
            }
        }
    }
};

#endif // !ATOVM_INSTRUCTIONS__H