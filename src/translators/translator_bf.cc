#include "translator_bf.h"

#include <random>

namespace atovm {
    std::shared_ptr<BrainfuckTranslator> BrainfuckTranslator::m_instance{nullptr};

    auto BrainfuckTranslator::GetInstance() -> std::shared_ptr<BrainfuckTranslator> {
        if (!m_instance)
            m_instance = std::make_shared<BrainfuckTranslator>();

        return m_instance;
    }

    auto BrainfuckTranslator::From() -> std::vector<InstructionTypes> {
        auto lexer = atovm::Lexer::GetInstance();
        std::vector<InstructionTypes> out;

        while (lexer->CheckBounds(lexer->GetPos())) {
            char c = lexer->GetAdv();
    
            switch (c) {
            case '+':
                out.push_back(atovm::Increment{});
                break;
            case '-':
                out.push_back(atovm::Decrement{});
                break;
            case '>':
                out.push_back(atovm::ForwardCell{});
                break;
            case '<':
                out.push_back(atovm::BackwardCell{});
                break;
            case '.':
                out.push_back(atovm::Print{});
                break;
            case ',':
                out.push_back(atovm::Scan{});
                break;
            case ':':
                if (lexer->Peek() != '/')
                    out.push_back(atovm::Exit{});

                break;
            case '[':
                out.push_back(atovm::LoopLabel{});
                break;
            case ']':
                out.push_back(atovm::JumpNotZero{});
                break;
            }
        }

        return out;
    }

    auto BrainfuckTranslator::To(const std::vector<InstructionTypes>& buf) -> std::string {
        std::string out = "[> Translated with AtoVM <]\n";

        for (const auto& it : buf) {
            switch (it.index()) {
                case AtoVM_Increment: {
                    out += "+";
                    break;
                }

                case AtoVM_Add: {
                    auto* ato_add = std::get_if<Add>(&it);                    
                    out.append(ato_add->count, '+');

                    break;
                }

                case AtoVM_Decrement: {
                    out += "-";
                    break;
                }

                case AtoVM_Sub: {
                    auto* ato_sub = std::get_if<Sub>(&it);
                    out.append(ato_sub->count, '-');

                    break;
                }

                case AtoVM_Mul: {
                    auto* ato_mul = std::get_if<Mul>(&it);
                    if (ato_mul) {
                        i32 factor = ato_mul->count;
                    
                        std::vector<i32> possible_factors;
                        for (i32 i = 1; i <= factor; i++) {
                            if (factor % i == 0) {
                                possible_factors.push_back(i);
                            }
                        }
                    
                        std::random_device rd;
                        std::mt19937_64 gen(rd());
                        std::uniform_int_distribution<> distrib(0, static_cast<i32>(possible_factors.size() - 1));

                        // ts kinda funny
                    
                        i32 f1 = possible_factors[distrib(gen)];
                        i32 f2 = factor / f1;
                    
                        out += "[->";
                        out.append(f1, '+');

                        out += "<]";
                        out.append(f2, '>');
                    }

                    break;
                }

                case AtoVM_ForwardCell: {
                    out += ">";
                    break;
                }

                case AtoVM_BackwardCell: {
                    out += "<";
                    break;
                }

                case AtoVM_ForwardMany: {
                    auto* ato_fwd = std::get_if<ForwardMany>(&it);
                    out.append(ato_fwd->count, '>');

                    break;
                }

                case AtoVM_BackwardMany: {
                    auto* ato_bwd = std::get_if<BackwardMany>(&it);
                    out.append(ato_bwd->count, '<');

                    break;
                }

                case AtoVM_LoopLabel: {
                    out += "[";
                    break;
                }

                case AtoVM_JumpNotZero: {
                    out += "]";
                    break;
                }

                case AtoVM_Print: {
                    out += ".";
                    break;
                }

                case AtoVM_Scan: {
                    out += ",";
                    break;
                }

                case AtoVM_Exit: {
                    out += ":";
                    break;
                }
            }
        }

        return out;
    }
}