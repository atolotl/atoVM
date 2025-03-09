#ifndef ATOVM_LEXER__H
#define ATOVM_LEXER__H

#include "prelude.h"

#include <memory>

namespace atovm {
    class Lexer {
    public:
        Lexer(const std::string& file_name);

    public:
        auto CheckBounds(usize new_pos) { return new_pos <= m_sesbian.length(); }

    public:
        static auto Init(const std::string& file_name) -> void;
        static auto GetInstance() -> std::shared_ptr<Lexer>;

    public:
        auto Get()  -> char { return CheckBounds(m_data_pos) ? m_sesbian[m_data_pos] : '\0'; }
        auto Peek() -> char { return CheckBounds(m_data_pos + 1) ? m_sesbian[m_data_pos + 1] : '\0'; }
        
        auto Behind() -> char { return CheckBounds(m_data_pos - 1) ? m_sesbian[m_data_pos - 1] : '\0'; }
        
        auto GetAdv() -> char { return CheckBounds(m_data_pos) ? m_sesbian[m_data_pos++] : '\0'; }
        auto AdvGet() -> char { return CheckBounds(m_data_pos + 1) ? m_sesbian[++m_data_pos] : '\0'; }
        
        auto Advance() -> void { CheckBounds(m_data_pos + 1) ? m_data_pos++ : m_data_pos; }

        const auto GetLex() const { return m_sesbian ; }
        const auto GetPos() const { return m_data_pos; }

    private:
        usize m_data_pos;
        std::string m_sesbian;

        static std::shared_ptr<Lexer> m_instance;
    };
}

#endif // !ATOVM_LEXER__H