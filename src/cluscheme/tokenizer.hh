#ifndef __CLUSCHEME__TOKENIZER_HH__
#define __CLUSCHEME__TOKENIZER_HH__

#include <functional>
#include <string>

namespace Tokenizer
{

struct StringParser {
    std::string m_buffer;
    size_t m_cursor;

    StringParser();
    StringParser(std::string);
    ~StringParser() = default;

    void feed(std::string);

    const char *remaining() const;
    size_t size() const;
    bool is_ascii_char(char ch);

    const char *peek() const;
    char next();

    size_t advance();
    size_t advance(size_t);
    size_t rollback();

    bool consume(char);
    bool consume(const std::string &);
};

struct Token {
    enum class Kind {
        Unknown,
        WhiteSpace,

        LeftParen,
        RightParen,
        VectorParen,

        // Keyword,
        Identifier,
        Boolean,
        // Number,
        Letter,

        Done,
    } kind;

    union Value {
        int None;
        enum Identifier : char { Plus = '+', Minus = '-' } ident;
        enum WhiteSpace : char {
            Space          = ' ',
            Tab            = '\t',
            LineFeed       = '\n',
            CarriageReturn = '\r',
        } white_space;
        char letter;
        bool boolean;
    } value;

    void dump() const;

    Token(Token::Kind = Token::Kind::Unknown, Token::Value = {});
    ~Token() = default;
};

struct StateMachine {
    enum State {
        Ground,
        Hash,
        Letter,
        Number,
    } m_state;

  public:
    StateMachine();
    ~StateMachine() = default;
    State state() const;
    void transition(StateMachine::State);
};

class Lexer
{
    StateMachine m_fsm;
    StringParser m_parser;

  public:
    Lexer(const std::string &);
    Token tokenize();

  private:
    StateMachine *fsm();
    StringParser *parser();

    Token dispatch(Token);
    void forward(StateMachine::State);
};

} // namespace Tokenizer

#endif
