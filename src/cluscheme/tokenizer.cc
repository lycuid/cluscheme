#include "tokenizer.hh"
#include <cstring>
#include <iostream>
#include <map>
#include <optional>
#include <unordered_map>

#define REPR(x) #x

namespace Tokenizer
{

StringParser::StringParser() : m_buffer(std::string_view()), m_cursor(0) {}

StringParser::StringParser(std::string buffer) : m_buffer(buffer), m_cursor(0)
{
}

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

bool StringParser::is_ascii_char(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

const char *StringParser::peek() const
{
    return size() > 0 ? &m_buffer[m_cursor] : nullptr;
}

char StringParser::next()
{
    std::cout << "[ Input\t\t]: '" << m_buffer[m_cursor] << "'" << std::endl;
    return m_buffer[m_cursor++];
}

size_t StringParser::advance() { return advance(1); }
size_t StringParser::advance(size_t steps) { return m_cursor += steps, steps; }
size_t StringParser::rollback() { return advance(-1); }

bool StringParser::consume(char ch) { return size() > 0 && next() == ch; }

bool StringParser::consume(const std::string &str)
{
    return str.size() <= size() &&
           std::memcmp(remaining(), str.c_str(), str.size()) == 0 &&
           advance(str.size());
}

Token::Token(Token::Kind _kind, Token::Value _value)
    : kind(_kind), value(_value)
{
}

void Token::dump() const
{
    std::cout << "Token { ";
    switch (kind) {
    case Token::Kind::Unknown: {
        std::cout << REPR(Unknown);
    } break;
    case Token::Kind::WhiteSpace: {
        std::cout << REPR(WhiteSpace) << "(";
        switch (value.letter) {
        case '\t': std::cout << "Tab"; break;
        case '\n': std::cout << "Newline"; break;
        case ' ': std::cout << "Space"; break;
        };
        std::cout << ")";
    } break;
    case Token::Kind::LeftParen: std::cout << REPR(LeftParen); break;
    case Token::Kind::RightParen: std::cout << REPR(RightParen); break;
    case Token::Kind::VectorParen: std::cout << REPR(VectorParen); break;
    case Token::Kind::Identifier: {
        std::cout << REPR(Identifier) << "(";
        switch (value.ident) {
        case Token::Value::Identifier::Plus: {
            std::cout << REPR(Plus);
        } break;
        case Token::Value::Identifier::Minus: {
            std::cout << REPR(Minus);
        } break;
        }
        std::cout << ")";
    } break;
    case Token::Kind::Boolean: {
        std::cout << REPR(Boolean);
        std::cout << "(" << (value.boolean ? "true" : "false") << ")";
    } break;
    case Token::Kind::Letter: {
        std::cout << REPR(Letter);
        switch (value.letter) {
        case ' ': {
            std::cout << "('"
                      << "#\\space"
                      << "')";
        } break;
        case '\n': {
            std::cout << "('"
                      << "#\\newline"
                      << "')";
        } break;
        default: std::cout << "('" << value.letter << "')";
        }
    } break;
    case Token::Kind::Done: std::cout << "Done"; break;
    };
    std::cout << " }" << std::endl;
}

StateMachine::StateMachine() : m_state(StateMachine::State::Ground) {}

StateMachine::State StateMachine::state() const { return m_state; }
void StateMachine::transition(StateMachine::State new_state)
{
    if (new_state != state()) {
#if 1
        std::cout << "[ Transition\t]: ";
        switch (m_state) {
        // case Dispatch: {
        //     std::cout << REPR(Dispatch);
        // } break;
        case Ground: {
            std::cout << REPR(Ground);
        } break;
        case Hash: {
            std::cout << REPR(Hash);
        } break;
        case Letter: {
            std::cout << REPR(Letter);
        } break;
        case Number: {
            std::cout << REPR(Number);
        } break;
        }
#endif

        m_state = new_state;

#if 1
        std::cout << " -> ";
        switch (m_state) {
        // case Dispatch: {
        //     std::cout << REPR(Dispatch);
        // } break;
        case Ground: {
            std::cout << REPR(Ground);
        } break;
        case Hash: {
            std::cout << REPR(Hash);
        } break;
        case Letter: {
            std::cout << REPR(Letter);
        } break;
        case Number: {
            std::cout << REPR(Number);
        } break;
        }
        std::cout << std::endl;
#endif
    }
}

Lexer::Lexer(const std::string &code) { m_parser.feed(code); }

StateMachine *Lexer::fsm() { return &m_fsm; }
StringParser *Lexer::parser() { return &m_parser; }

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
        fsm()->transition(StateMachine::State::Ground);
    };

    auto forward = [&](StateMachine::State new_state) {
        parser()->rollback();
        fsm()->transition(new_state);
    };

    for (; !f_emit && parser()->peek();) {
        char input = parser()->next();

        switch (fsm()->state()) {
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

            case '#': fsm()->transition(StateMachine::State::Hash); break;
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

            default: emit({});
            };
        } break;

        case StateMachine::State::Hash: {
            switch (input) {
            case '(': emit(Token(Token::Kind::VectorParen)); break;
            case '\\': {
                if (parser()->consume("space")) {
                    emit(Token(Token::Kind::Letter, {' '}));
                } else if (parser()->consume("newline")) {
                    emit(Token(Token::Kind::Letter, {'\n'}));
                } else if (parser()->peek()) {
                    if (char next = parser()->next();
                        parser()->is_ascii_char(next))
                        emit(Token(Token::Kind::Letter, {next}));
                }
            } break;
            case 't': [[fallthrough]];
            case 'f': emit(Token(Token::Kind::Boolean, {input == 't'})); break;
            default: emit({});
            };
        } break;

        case StateMachine::State::Letter: {
            fsm()->transition(StateMachine::State::Ground);
        } break;

        case StateMachine::State::Number: {
            fsm()->transition(StateMachine::State::Ground);
        } break;

        default: emit({});
        };
    }

    if (!parser()->size())
        token = Token(Token::Kind::Done);

    std::cout << "[ Remaining\t]: '" << parser()->remaining() << "'"
              << std::endl;
    return token;
}

} // namespace Tokenizer
