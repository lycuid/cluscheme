#ifndef __CLUSCHEME__TOKENIZER_HH__
#define __CLUSCHEME__TOKENIZER_HH__

#include <functional>
#include <string>

namespace Tokenizer
{

class StringParser
{
    std::string m_buffer;
    size_t m_cursor;

  public:
    StringParser();
    StringParser(std::string);
    ~StringParser() = default;

    const char *remaining() const;
    size_t size() const;

    const char *peek() const;
    char next();

    size_t advance();
    size_t advance(size_t);

    bool consume(char);
    bool consume(const std::string &);
};

struct Token {
    enum Ident { Plus, Minus };

    enum Kind : uint {
        Unknown,
        WhiteSpace,

        LeftParen,
        RightParen,
        VectorParen,

        // Keyword,    // (Keyword),
        Identifier, // (Identifier),
        Boolean,    // (bool),
        // Number,     // (Number),
        // Letter,     // (char),

        Space,
        Newline,
    } kind;

    union Value {
        Ident identifier;
        bool boolean;
    } value;

    void dump() const;
};

struct StateMachine {
    enum State {
        Ground,
        Hash,
        Dispatch,
    } m_state;

  public:
    State state() const;
    void transition(State);
};

class Lexer
{
    StateMachine m_fsm;

  public:
    void tokenize(const std::string &);

  private:
    StateMachine *fsm();
};

} // namespace Tokenizer

#endif
