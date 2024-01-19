#include "tokenizer.hh"
#include <cstring>
#include <iostream>
#include <map>
#include <optional>
#include <unordered_map>

inline bool is_ascii_char(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

inline bool is_valid_keyword_char(char ch)
{
    switch (ch) {
    case '!': [[fallthrough]];
    case '@': [[fallthrough]];
    case '#': [[fallthrough]];
    case '$': [[fallthrough]];
    case '%': [[fallthrough]];
    case '^': [[fallthrough]];
    case '&': [[fallthrough]];
    case '*': [[fallthrough]];
    case '-': [[fallthrough]];
    case '_': [[fallthrough]];
    case '+': [[fallthrough]];
    case '=': return true;
    default: break;
    }
    return is_ascii_char(ch) || (ch >= '0' && ch <= '9') || false;
}

namespace Tokenizer
{

void StringParser::feed(std::string str)
{
    m_buffer = str;
    m_cursor = 0;
}

const char *StringParser::remaining() const
{
    return m_buffer.c_str() + m_cursor;
}

size_t StringParser::size() const { return m_buffer.size() - m_cursor; }

const char *StringParser::peek() const
{
    return size() > 0 ? &m_buffer[m_cursor] : nullptr;
}

char StringParser::next()
{
    std::cout << "[ Input\t\t]: '" << m_buffer[m_cursor] << "'" << std::endl;
    return m_buffer[m_cursor++];
}

size_t StringParser::advance(size_t steps) { return m_cursor += steps, steps; }
size_t StringParser::rollback() { return advance(-1); }

bool StringParser::consume(char ch) { return size() > 0 && next() == ch; }

bool StringParser::consume(const std::string &str)
{
    return str.size() <= size() &&
           std::memcmp(remaining(), str.c_str(), str.size()) == 0 &&
           advance(str.size());
}

size_t StringParser::consume(std::function<bool(char)> fn)
{
    size_t size = 0;
    for (; peek() && fn(*peek()); ++size)
        advance();
    return size;
}

void Token::dump() const // {{{
{
    std::cout << "Token { ";
    switch (m_kind) {
    case Token::Kind::Unknown: {
        std::cout << "Unknown";
    } break;
    case Token::Kind::WhiteSpace: {
        std::cout << "WhiteSpace"
                  << "(";
        switch (m_value.letter) {
        case '\t': std::cout << "Tab"; break;
        case '\n': std::cout << "Newline"; break;
        case ' ': std::cout << "Space"; break;
        };
        std::cout << ")";
    } break;
    case Token::Kind::LeftParen: std::cout << "LeftParen"; break;
    case Token::Kind::RightParen: std::cout << "RightParen"; break;
    case Token::Kind::VectorParen: std::cout << "VectorParen"; break;
    case Token::Kind::Boolean: {
        std::cout << "Boolean";
        std::cout << "(" << (m_value.boolean ? "true" : "false") << ")";
    } break;
    case Token::Kind::Letter: {
        std::cout << "Letter";
        switch (m_value.letter) {
        case ' ': std::cout << "('#\\space')"; break;
        case '\n': std::cout << "('#\\newline')"; break;
        default: std::cout << "('" << m_value.letter << "')";
        }
    } break;
    case Token::Kind::Identifier: {
        std::cout << "Identifier"
                  << "(";
        switch (m_value.ident) {
        case Token::Value::Identifier::Plus: std::cout << "Plus"; break;
        case Token::Value::Identifier::Minus: std::cout << "Minus"; break;
        }
        std::cout << ")";
    } break;
    case Token::Kind::Keyword: {
        std::cout << "Keyword"
                  << "(";
        switch (m_value.keyword) {
        case Token::Value::Keyword::Define: std::cout << "Define"; break;
        case Token::Value::Keyword::Lambda: std::cout << "Lambda"; break;
        case Token::Value::Keyword::Let: std::cout << "Let"; break;
        case Token::Value::Keyword::LetRec: std::cout << "LetRec"; break;
        case Token::Value::Keyword::Invalid: std::cout << "Invalid"; break;
        }
        std::cout << ")";
    } break;
    case Token::Kind::Done: std::cout << "Done"; break;
    };
    std::cout << " }" << std::endl;
} // }}}

StateMachine::State StateMachine::state() const { return m_state; }
void StateMachine::transition(StateMachine::State new_state)
{
    if (new_state != state()) {
#if 1
        // {{{
        std::cout << "[ Transition\t]: ";
        switch (m_state) {
        case Ground: std::cout << "Ground"; break;
        case Hash: std::cout << "Hash"; break;
        case Letter: std::cout << "Letter"; break;
        case Number: std::cout << "Number"; break;
        case Keyword: std::cout << "Keyword"; break;
        }
        // }}}
#endif

        m_state = new_state;

#if 1
        // {{{
        std::cout << " -> ";
        switch (m_state) {
        case Ground: std::cout << "Ground"; break;
        case Hash: std::cout << "Hash"; break;
        case Letter: std::cout << "Letter"; break;
        case Number: std::cout << "Number"; break;
        case Keyword: std::cout << "Keyword"; break;
        }
        std::cout << std::endl;
        // }}}
#endif
    }
}

Trie::Node::Node() : keyword(Token::Value::Keyword::Invalid)
{
    std::cout << "Trie::Node Constructor." << std::endl;
    memset(children, 0, sizeof(children));
}

Trie::Node::~Node()
{
    for (auto child : children)
        if (child)
            delete child;
}

void Trie::insert(const std::string &str, Token::Value::Keyword keyword)
{
    Trie::Node *node = &root;
#if 1
    std::cout << "[ Inserting\t]: '" << str << "'" << std::endl;
#endif
    for (int ch : str) {
        if (!node->children[ch])
            node->children[ch] = new Trie::Node();
        node = node->children[ch];
    }
    node->keyword = keyword;
}

std::pair<Token::Value::Keyword, size_t> Trie::check(const std::string &str)
{
    std::cout << "[ Checking\t]: '" << str << "'" << std::endl;
    Trie::Node *node = &root;
    size_t size      = 0;
    for (int ch : str) {
        if (!node->children[ch]) {
            if (is_valid_keyword_char(ch))
                return {Token::Value::Keyword::Invalid, size};
            break;
        }
        ++size;
        node = node->children[ch];
    }
    return {node->keyword, size};
}

Lexer::Lexer(const std::string &code) : StringParser(code)
{
    std::cout << "Lexer Constructor." << std::endl;
    m_keywords.insert("define", Token::Value::Keyword::Define);
    m_keywords.insert("lambda", Token::Value::Keyword::Lambda);
    m_keywords.insert("let", Token::Value::Keyword::Let);
    m_keywords.insert("letrec", Token::Value::Keyword::LetRec);
}

Token Lexer::tokenize()
{
    Token token;
    bool f_emit = false;

    auto emit = [&](Token new_token) {
        token  = new_token;
        f_emit = true;
#if 1
        std::cout << "[ Emitting\t]: ";
        new_token.dump();
#endif
        transition(StateMachine::State::Ground);
    };

    auto forward = [&](StateMachine::State new_state) {
        rollback();
        transition(new_state);
    };

    for (; !f_emit && peek();) {
        char input = next();

        switch (state()) {
        case StateMachine::State::Ground: {
            switch (input) {
            case '\t': [[fallthrough]];
            case '\n': [[fallthrough]];
            case '\r': [[fallthrough]];
            case ' ': {
                emit(Token(Token::Kind::WhiteSpace,
                           {static_cast<Token::Value::WhiteSpace>(input)}));
                break;
            }

            case '#': transition(StateMachine::State::Hash); break;
            case '(': emit(Token(Token::Kind::LeftParen)); break;
            case ')': emit(Token(Token::Kind::RightParen)); break;

            case '+':
            case '-': {
                emit(Token(Token::Kind::Identifier,
                           {static_cast<Token::Value::Identifier>(input)}));
            } break;

            case '0': [[fallthrough]];
            case '1': [[fallthrough]];
            case '2': [[fallthrough]];
            case '3': [[fallthrough]];
            case '4': [[fallthrough]];
            case '5': [[fallthrough]];
            case '6': [[fallthrough]];
            case '7': [[fallthrough]];
            case '8': [[fallthrough]];
            case '9': {
                forward(StateMachine::State::Number);
            } break;

            default: forward(StateMachine::State::Keyword);
            };
        } break;

        case StateMachine::State::Hash: {
            switch (input) {
            case '(': emit(Token(Token::Kind::VectorParen)); break;
            case '\\': {
                if (consume("space")) {
                    emit(Token(Token::Kind::Letter, {' '}));
                } else if (consume("newline")) {
                    emit(Token(Token::Kind::Letter, {'\n'}));
                } else if (peek()) {
                    if (char ch = next(); is_ascii_char(ch))
                        emit(Token(Token::Kind::Letter, {ch}));
                }
            } break;
            case 't': [[fallthrough]];
            case 'f': emit(Token(Token::Kind::Boolean, {input == 't'})); break;

            default: forward(StateMachine::State::Keyword);
            };
        } break;

        case StateMachine::State::Letter: {
            transition(StateMachine::State::Ground);
        } break;

        case StateMachine::State::Number: {
            transition(StateMachine::State::Ground);
        } break;

        case StateMachine::State::Keyword: {
            rollback();
            if (auto pair = m_keywords.check(remaining());
                pair.first != Token::Value::Keyword::Invalid) {
                advance(pair.second);
                emit(Token(Token::Kind::Keyword, {pair.first}));
            } else {
                advance();
                emit({});
            }
        } break;

        default: break;
        };
    }

    if (!size())
        token = Token(Token::Kind::Done);

    std::cout << "[ Remaining\t]: '" << remaining() << "'" << std::endl;
    return token;
}

} // namespace Tokenizer

// vim:fdm=marker:fmr={{{,}}}
