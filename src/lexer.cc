#include "lexer.h"

#include <fstream>

namespace atovm {
    std::shared_ptr<Lexer> Lexer::m_instance{nullptr};
    Lexer::Lexer(const std::string& file_name) {
        m_data_pos = 0;

        std::ifstream t(file_name);
        m_sesbian = std::string((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

        t.close();
    }

    auto Lexer::Init(const std::string& file_name) -> void {
        m_instance = std::make_shared<Lexer>(file_name);
    }

    auto Lexer::GetInstance() -> std::shared_ptr<Lexer> {
        return m_instance;
    }
}