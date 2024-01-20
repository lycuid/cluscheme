#ifndef __CLUSCHEME__LEXER__TOKEN_HH__
#define __CLUSCHEME__LEXER__TOKEN_HH__

#include <iostream>

namespace Lexer
{

struct Token {
    enum class Kind {
        Unknown,
        WhiteSpace,

        LeftParen,
        RightParen,
        VectorParen,

        Boolean,
        Number,
        Letter,
        Identifier,
        Keyword,
        String,

        Eof,
    } m_kind;

    union Value {
        int None : 1;
        enum WhiteSpace : char {
            Space          = ' ',
            Tab            = '\t',
            LineFeed       = '\n',
            CarriageReturn = '\r',
        } whitespace;
        bool boolean;
        char letter;
        enum Identifier : char { Plus = '+', Minus = '-' } ident;
        const std::string *keyword;
        const std::string *string;
        struct Number {
            int whole;
            float decimal;

            Number(int _whole = 0, float _decimal = 0)
                : whole(_whole), decimal(_decimal)
            {
            }
            void repr() const;
        } number;
    } m_value;

    Token(Token::Kind kind = Token::Kind::Unknown, Token::Value value = {})
        : m_kind(kind), m_value(value)
    {
    }
    ~Token() = default;

    void repr() const;
    constexpr Kind kind() const { return m_kind; }
    constexpr const Value &value() const { return m_value; }
};

} // namespace Lexer

#endif
