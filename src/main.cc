#include "prelude.h"

#include "lexer.h"

#include "interpreter/x86_state.h"
#include "jit/swag.h"

#include "translators/translator_bf.h"
#include "translators/translator_c.h"

#include "codegen/optimizer.h"

#include <fstream>
#include <fmt/chrono.h>

#include <iostream>

auto readBinary(const std::string& file_name) -> std::vector<u8> {
    std::ifstream fs{ file_name, std::ios::binary };
    fs.unsetf(std::ios::skipws);

    fs.seekg(0, std::ios::end);
    usize file_size = fs.tellg();
    fs.seekg(0, std::ios::beg);

    std::vector<u8> data{};
    data.resize(file_size);

    fs.read((char*)data.data(), file_size);
    fs.close();

    return data;
}

auto main() -> i32 {
    bool run_bytecode = false;

    atovm::Lexer::Init("input.bf");
    auto optimizer = atovm::Optimizer::GetInstance();
    
    auto translator_bf = atovm::BrainfuckTranslator::GetInstance();
    auto translator_c = atovm::CTranslator::GetInstance();

    std::vector<atovm::InstructionTypes> inst = translator_bf->From(), optimized;

    auto now = std::chrono::high_resolution_clock::now();
    optimized = optimizer->Optimize(inst);
    auto end = std::chrono::high_resolution_clock::now() - now;

    fmt::println("Optimization took {}", std::chrono::duration_cast<std::chrono::microseconds>(end));

    std::vector<u8> byte_code;
    for (const auto &ist : optimized) {
        atovm::EncodeInstruction(ist, byte_code);
    }

    fmt::println("Optimized bytecode: ");
    for (const auto& bc : byte_code) {
        fmt::print("{:02x} ", bc);
    }
    fmt::println("");

    char choice;
    do {
        fmt::println("Select an option: ");
        fmt::println("0 - Run bytecode instead");
        fmt::println("1 - JIT (Swag) [brainfuck]");
        fmt::println("2 - Interpreter (x86Interpreter) [brainfuck]");
        fmt::println("3 - Output bytecode to file");
        fmt::println("4 - Bytecode to brainfuck");
        fmt::println("5 - Bytecode to C");
        fmt::print("> ");
        std::cin >> choice;

    } while (choice < '0' || choice > '5');

    if (choice == '0') {
        run_bytecode = true;
        fmt::println("[!] running bytecode now! (reading from out.bin)");

        fmt::print("> ");
        std::cin >> choice;
    }

    if (choice == '1') {
        fmt::println("Using Swag Engine: ");
        {
            if (run_bytecode) {
                std::vector<u8> data = readBinary("out.bin");

                std::vector<atovm::InstructionTypes> insts;
                usize pos = 0;

                while (true) {
                    auto it = atovm::DecodeInstruction(data, pos);
                
                    if (std::holds_alternative<std::nullptr_t>(it)) {
                        break;
                    }
                
                    insts.push_back(it);
                }
                
                atovm::SwagJIT swag{insts};
    
                auto fn = swag.getCode<i32 (*)(const void *, const void *, i32 *)>();
    
                static i32 stack[128 * 1024];
    
                i32 exit_code = fn(reinterpret_cast<const void *>(putchar), reinterpret_cast<const void *>(getchar), stack);
                fmt::println("\nJIT Exit code: {}", exit_code);
            } else {
                atovm::SwagJIT swag{optimized};
    
                auto fn = swag.getCode<i32 (*)(const void *, const void *, i32 *)>();
    
                static i32 stack[128 * 1024];
    
                i32 exit_code = fn(reinterpret_cast<const void *>(putchar), reinterpret_cast<const void *>(getchar), stack);
                fmt::println("\nJIT Exit code: {}", exit_code);
            }
        }
    } else if (choice == '2') {
        fmt::println("");
        fmt::println("Using x86Interpreter Engine: ");
        {
            if (run_bytecode) {
                std::vector<u8> data = readBinary("out.bin");

                std::vector<atovm::InstructionTypes> insts;
                usize pos = 0;

                while (true) {
                    auto it = atovm::DecodeInstruction(data, pos);
                
                    if (std::holds_alternative<std::nullptr_t>(it)) {
                        break;
                    }
                
                    insts.push_back(it);
                }

                atovm::x86Interpreter inter{insts};
    
                i32 exit_code = inter.Execute();
                fmt::println("\nInterpreter Exit code: {}", exit_code);
            } else {
                atovm::x86Interpreter inter{optimized};
    
                i32 exit_code = inter.Execute();
                fmt::println("\nInterpreter Exit code: {}", exit_code);
            }
        }
    } else if (choice == '3') {
        std::ofstream fs{ "out.bin", std::ios::binary };

        fs.write((const char*)byte_code.data(), byte_code.size());
        fs.close();
    } else if (choice == '4') {
        std::vector<u8> data = readBinary("out.bin");

        std::vector<atovm::InstructionTypes> insts;
        usize pos = 0;

        while (true) {
            auto it = atovm::DecodeInstruction(data, pos);

            if (std::holds_alternative<std::nullptr_t>(it)) {
                break;
            }

            insts.push_back(it);
        }

        std::string txt = translator_bf->To(insts);

        std::ofstream of{"out.bf"};
        of.write(txt.c_str(), txt.size());
        of.close();
    } else if (choice == '5') {
        std::vector<u8> data = readBinary("out.bin");

        std::vector<atovm::InstructionTypes> insts;
        usize pos = 0;

        while (true) {
            auto it = atovm::DecodeInstruction(data, pos);

            if (std::holds_alternative<std::nullptr_t>(it)) {
                break;
            }

            insts.push_back(it);
        }

        std::string txt = translator_c->To(insts);

        std::ofstream of{"out.c"};
        of.write(txt.c_str(), txt.size());
        of.close();
    }

    return 0;
}