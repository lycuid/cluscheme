#ifndef __CLUSCHEME__TOKENIZER_HH__
#define __CLUSCHEME__TOKENIZER_HH__

#include <functional>
#include <iostream>
#include <string>

namespace Tokenizer
{

struct StringParser {
    std::string m_buffer;
    size_t m_cursor;

    StringParser() : m_buffer(std::string()), m_cursor(0)
    {
        std::cout << "StringParser Empty Constructor." << std::endl;
    }
    StringParser(std::string buffer) : m_buffer(buffer), m_cursor(0)
    {
        std::cout << "StringParser String Constructor." << std::endl;
    }
    ~StringParser() = default;

    void feed(std::string);

    const char *remaining() const;
    size_t size() const;

    const char *peek() const;
    char next();

    size_t advance(size_t = 1);
    size_t rollback();

    bool consume(char);
    bool consume(const std::string &);
    size_t consume(std::function<bool(char)>);
};

struct Token {
    enum class Kind {
        Unknown,
        WhiteSpace,

        LeftParen,
        RightParen,
        VectorParen,

        Boolean,
        // Number,
        Letter,
        Identifier,
        Keyword,

        Done,
    } m_kind;

    union Value {
        int None;
        enum WhiteSpace : char {
            Space          = ' ',
            Tab            = '\t',
            LineFeed       = '\n',
            CarriageReturn = '\r',
        } white_space;
        bool boolean;
        char letter;
        enum Identifier : char { Plus = '+', Minus = '-' } ident;
        enum Keyword : int {
            Define,
            Lambda,
            Let,
            LetRec,
            Invalid,
        } keyword;
    } m_value;

    Token(Token::Kind kind = Token::Kind::Unknown, Token::Value value = {})
        : m_kind(kind), m_value(value)
    {
        std::cout << "Token Constructor." << std::endl;
    }
    ~Token() = default;

    void dump() const;
};

struct StateMachine {
    enum State {
        Ground,
        Hash,
        Letter,
        Number,
        Keyword,
    } m_state;

    StateMachine() : m_state(StateMachine::State::Ground)
    {
        std::cout << "StateMachine Constructor." << std::endl;
    }
    ~StateMachine() = default;

    State state() const;
    void transition(StateMachine::State);
};

struct Trie {
    struct Node {
        Node *children[128];
        Token::Value::Keyword keyword;

        Node();
        ~Node();
    } root;

    Trie() : root(Trie::Node{})
    {
        std::cout << "Trie Constructor." << std::endl;
    }
    ~Trie() = default;

    void insert(const std::string &, Token::Value::Keyword);
    std::pair<Token::Value::Keyword, size_t> check(const std::string &);
};

class Lexer : private StringParser, private StateMachine
{
    Trie m_keywords;

    void forward(StateMachine::State);

  public:
    Lexer(const std::string &);
    Token tokenize();
};

} // namespace Tokenizer

#endif
