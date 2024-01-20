#ifndef __CLUSCHEME__AST_HH__
#define __CLUSCHEME__AST_HH__

#include <cluscheme/lexer.hh>

#define panic(fmt, ...)                                                        \
    {                                                                          \
        printf(fmt __VA_OPT__(, ) __VA_ARGS__);                                \
        exit(1);                                                               \
    }

namespace AST
{

class AST
{
    struct Node {
    };

    Lexer::Lexer m_lexer;

  public:
    AST(Lexer::Lexer lexer) : m_lexer(lexer)
    {
        std::vector<Lexer::Token> tokens;

        for (Lexer::Token token; token = lexer.next(), 1;) {
#if defined(DEBUG_LEXER)
            token.repr();
#endif
            token.repr();
            tokens.push_back(token);
            if (token.kind() == Lexer::Token::Kind::Eof)
                break;
        }
        printf("\n");

        printf("Code: '%s'\n", lexer.code().c_str());
        for (Lexer::Token token : tokens) {
            token.repr();
            if (token.kind() == Lexer::Token::Kind::Unknown)
                panic("[ UnknownToken\t]:(%ld)\n", m_lexer.cursor());
        }
    }
    ~AST() = default;
};

}; // namespace AST

#endif
