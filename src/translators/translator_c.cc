#include "translator_c.h"

#include <random>

namespace atovm {
    std::shared_ptr<CTranslator> CTranslator::m_instance{nullptr};

    auto CTranslator::GetInstance() -> std::shared_ptr<CTranslator> {
        if (!m_instance)
            m_instance = std::make_shared<CTranslator>();

        return m_instance;
    }

    auto CTranslator::To(const std::vector<InstructionTypes>& buf) -> std::string {
        usize default_tab = 4;
        usize extra_tab = 0;
        std::string out = "// [> Translated with AtoVM <]\n\n#include <stdio.h>\n#include <stdint.h>\n\nint main(int argc, char** argv) {\n    uint8_t cells[60 * (1 << 10)];\n    uint64_t pos = 0;\n    ";

        bool has_exit = false;

        usize other_idx = 0;
        for (const auto& it : buf) {
            if (other_idx + 1 < buf.size() && buf[other_idx + 1].index() == AtoVM_JumpNotZero) {
                extra_tab -= 4;
            }

            switch (it.index()) {
                case AtoVM_Increment: {
                    out += "cells[pos]++;\n";
                    out.append(default_tab + extra_tab, ' ');

                    break;
                }

                case AtoVM_Add: {
                    auto* ato_add = std::get_if<Add>(&it);
                    out += fmt::format("cells[pos] += {};\n\n", ato_add->count);

                    out.append(default_tab + extra_tab, ' ');

                    break;
                }

                case AtoVM_Decrement: {
                    out += "cells[pos]--;\n";
                    out.append(default_tab + extra_tab, ' ');

                    break;
                }

                case AtoVM_Sub: {
                    auto* ato_sub = std::get_if<Sub>(&it);
                    out += fmt::format("cells[pos] -= {};\n\n", ato_sub->count);

                    out.append(default_tab + extra_tab, ' ');

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

                        out += fmt::format("cells[pos] = {} * {};\n", f1, f2);
                        out.append(default_tab + extra_tab, ' ');
                    }

                    break;
                }

                case AtoVM_ForwardCell: {
                    out += "pos++;\n";
                    out.append(default_tab + extra_tab, ' ');

                    break;
                }

                case AtoVM_BackwardCell: {
                    out += "pos--;\n";
                    out.append(default_tab + extra_tab, ' ');
                    
                    break;
                }

                case AtoVM_ForwardMany: {
                    auto* ato_fwd = std::get_if<ForwardMany>(&it);
                    out += fmt::format("pos += {};\n", ato_fwd->count);

                    out.append(default_tab + extra_tab, ' ');

                    break;
                }

                case AtoVM_BackwardMany: {
                    auto* ato_bwd = std::get_if<BackwardMany>(&it);
                    out += fmt::format("pos -= {};\n", ato_bwd->count);

                    out.append(default_tab + extra_tab, ' ');

                    break;
                }

                case AtoVM_LoopLabel: {
                    out += "while (cells[pos] != 0) {\n";
                    extra_tab += 4;

                    out.append(default_tab + extra_tab, ' ');
                    break;
                }

                case AtoVM_JumpNotZero: {
                    out += "}\n\n";
                    out.append(default_tab + extra_tab, ' ');

                    break;
                }

                case AtoVM_Print: {
                    out += "putchar(cells[pos]);\n\n";
                    out.append(default_tab + extra_tab, ' ');

                    break;
                }

                case AtoVM_Scan: {
                    out += "cells[pos] = getchar();\n\n";
                    out.append(default_tab + extra_tab, ' ');

                    break;
                }

                case AtoVM_Exit: {
                    has_exit = true;
                    out += "return cells[pos];\n}";
                    break;
                }
            }

            other_idx++;
        }

        if (!has_exit) {
            out += "return cells[pos];\n}";
        }

        return out;
    }
}