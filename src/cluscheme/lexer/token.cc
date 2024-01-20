#include "token.hh"
#include <cstring>

namespace Lexer
{

void Token::Value::Number::repr() const
{
    printf("whole:%d", whole);
    if (decimal > 0.0)
        printf("|decimal:%f", decimal);
}

void Token::repr() const
{
    printf("Token { ");
    switch (m_kind) {
    case Token::Kind::Unknown: {
        printf("Unknown");
    } break;
    case Token::Kind::WhiteSpace: {
        printf("WhiteSpace");
        switch (m_value.letter) {
        case '\t': printf("(Tab)"); break;
        case '\n': printf("(Newline)"); break;
        case ' ': printf("(Space)"); break;
        };
    } break;
    case Token::Kind::LeftParen: printf("LeftParen"); break;
    case Token::Kind::RightParen: printf("RightParen"); break;
    case Token::Kind::VectorParen: printf("VectorParen"); break;
    case Token::Kind::Boolean: {
        printf("Boolean(%s)", m_value.boolean ? "true" : "false");
    } break;
    case Token::Kind::Letter: {
        printf("Letter");
        switch (m_value.letter) {
        case ' ': printf("('#\\space')"); break;
        case '\n': printf("('#\\newline')"); break;
        default: printf("('%c')", m_value.letter);
        }
    } break;
    case Token::Kind::Number: {
        printf("Number(");
        m_value.number.repr();
        printf(")");
    } break;
    case Token::Kind::Identifier: {
        printf("Identifier");
        switch (m_value.ident) {
        case Token::Value::Identifier::Plus: printf("(Plus)"); break;
        case Token::Value::Identifier::Minus: printf("(Minus)"); break;
        }
    } break;
    case Token::Kind::Keyword: {
        printf("Keyword(%s)",
               m_value.keyword ? m_value.keyword->c_str() : "Invalid");
    } break;
    case Token::Kind::String: {
        printf("String('%s')", (*m_value.string).c_str());
    } break;
    case Token::Kind::Eof: printf("Eof"); break;
    };
    printf(" }\n");
}

} // namespace Lexer
