#include "tokenizer.hh"
#include <cstring>
#include <iostream>

#define REPR(x) #x

namespace Tokenizer
{

StringParser::StringParser() : m_buffer(std::string_view()), m_cursor(0) {}

StringParser::StringParser(std::string buffer)
    : m_buffer(std::move(buffer)), m_cursor(0)
{
}

const char *StringParser::remaining() const
{
    return m_buffer.c_str() + m_cursor;
}

size_t StringParser::size() const { return m_buffer.size() - m_cursor; }

const char *StringParser::peek() const { return &m_buffer[m_cursor]; }

char StringParser::next() { return m_buffer[m_cursor++]; }

size_t StringParser::advance() { return advance(1); }
size_t StringParser::advance(size_t steps) { return m_cursor += steps, steps; }

bool StringParser::consume(char ch) { return size() > 0 && next() == ch; }

bool StringParser::consume(const std::string &str)
{
    return str.size() <= size() &&
           std::memcmp(remaining(), str.c_str(), str.size()) == 0 &&
           advance(str.size());
}

void Token::dump() const
{
    std::cout << "Token { ";
    switch (kind) {
    case Unknown: {
        std::cout << REPR(Unknown);
    } break;
    case WhiteSpace: {
        std::cout << REPR(WhiteSpace);
    } break;
    case LeftParen: {
        std::cout << REPR(LeftParen);
    } break;
    case RightParen: {
        std::cout << REPR(RightParen);
    } break;
    case VectorParen: {
        std::cout << REPR(VectorParen);
    } break;
    case Identifier: {
        std::cout << REPR(Identifier);
        switch (value.identifier) {
        case Plus: {
            std::cout << "(" REPR(Plus) ")";
        } break;
        case Minus: {
            std::cout << "(" REPR(Minus) ")";
        } break;
        }
    } break;
    case Boolean: {
        std::cout << REPR(Boolean);
    } break;
    case Space: {
        std::cout << REPR(Space);
    } break;
    case Newline: {
        std::cout << REPR(Newline);
    } break;
    };
    std::cout << " }" << std::endl;
}

StateMachine::State StateMachine::state() const { return m_state; }
void StateMachine::transition(State new_state) { m_state = new_state; }

StateMachine *Lexer::fsm() { return &m_fsm; }

void Lexer::tokenize(const std::string &code) { (void)code; }

} // namespace Tokenizer
