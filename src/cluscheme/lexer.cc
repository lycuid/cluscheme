#include "lexer.hh"
#include <cstring>

namespace Lexer
{

void Lexer::feed(std::string str)
{
    m_buffer = str;
    m_cursor = 0;
}

const char *Lexer::remaining() const { return m_buffer.c_str() + m_cursor; }

size_t Lexer::size() const { return m_buffer.size() - m_cursor + 1; }

const char *Lexer::peek(int at) const
{
    int index = m_cursor + at;
    return index >= 0 && index < static_cast<int>(m_buffer.size())
               ? &m_buffer[m_cursor + at]
               : nullptr;
}

char Lexer::get()
{
#if defined(DEBUG_LEXER)
    printf("[ Input\t\t]: '%c'\n", m_buffer[m_cursor]);
#endif
    return m_buffer[m_cursor++];
}

int Lexer::parse_digit(int base)
{
    int num = 0;
    while (peek() && is_digit(*peek(), base))
        num = (num * base) + (get() - '0');
    return num;
}

Token::Value::Number Lexer::tokenize_number()
{
    Token::Value::Number number;
    /* parsing 'whole' */ {
        int mul = 1, input = 0;
        switch (input = get()) {
        case '+': break;
        case '-': mul = -1; break;
        default: break;
        }

        if (input >= '0' && input <= '9')
            number.whole = number.whole * 10 + (input - '0');
        for (; peek() && is_digit(*peek());)
            number.whole = number.whole * 10 + get() - '0';
        number.whole *= mul;
    }

    if (consume('.')) { /* parsing 'decimal'. */
        const char *start = peek();
        while (peek() && is_digit(*peek()))
            number.decimal = number.decimal * 10 + get() - '0';
        while (start++ < peek())
            number.decimal /= 10;
    }
    return number;
}

int Lexer::advance(int steps) { return m_cursor += steps, steps; }

inline int Lexer::rollback() { return advance(-1); }

bool Lexer::consume(char ch) { return size() > 0 && get() == ch; }

bool Lexer::consume(const std::string &str)
{
    return str.size() <= size() &&
           std::memcmp(remaining(), str.c_str(), str.size()) == 0 &&
           advance(str.size());
}

Lexer::Lexer(const std::string &code) : m_buffer(code), m_cursor(0)
{
    m_keywords.add("define");
    m_keywords.add("lambda");
    m_keywords.add("let");
    m_keywords.add("letrec");
}

Token Lexer::next()
{
    Token token;
    bool f_emit = false;

    auto emit = [&](Token new_token) {
        token  = new_token;
        f_emit = true;
    };

    while (!f_emit && size() > 0) {
        switch (char input = get()) {
        case 0: emit(Token(Token::Kind::Eof)); break;

        case '\t': [[fallthrough]];
        case '\n': [[fallthrough]];
        case '\r': [[fallthrough]];
        case ' ': {
            emit(Token(
                Token::Kind::WhiteSpace,
                {.whitespace = static_cast<Token::Value::WhiteSpace>(input)}));
        } break;

        case '#': {
            if (size() > 0) {
                switch (char input = get(); input) {
                case '\\': {
                    if (consume("space") || consume("Space"))
                        emit(Token(Token::Kind::Letter, {.letter = ' '}));
                    else if (consume("newline") || consume("Newline"))
                        emit(Token(Token::Kind::Letter, {.letter = '\n'}));
                    else if (peek() && is_ascii_char(*peek()))
                        emit(Token(Token::Kind::Letter, {.letter = get()}));
                } break;

                case '(': emit(Token(Token::Kind::VectorParen)); break;

                case 't': [[fallthrough]];
                case 'f': {
                    emit(
                        Token(Token::Kind::Boolean, {.boolean = input == 't'}));
                } break;

                default: break;
                };
            }
        } break;

        case '"': {
            const char *start = peek();
            char input        = 0;
            while (size() > 0) {
                if ((input = get()) == '\\')
                    advance();
                else if (input == '"')
                    break;
            }
            if (input == '"')
                emit(Token(Token::Kind::String,
                           {.string = new std::string(start, peek(-1))}));
        } break;

        case '(': emit(Token(Token::Kind::LeftParen)); break;
        case ')': emit(Token(Token::Kind::RightParen)); break;

        case '+':
        case '-': {
            if (peek() && is_digit(*peek())) {
                rollback();
                emit(Token(Token::Kind::Number, {.number = tokenize_number()}));
            } else {
                emit(Token(
                    Token::Kind::Identifier,
                    {.ident = static_cast<Token::Value::Identifier>(input)}));
            }
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
            rollback();
            emit(Token(Token::Kind::Number, {.number = tokenize_number()}));
        } break;

        default: {
            rollback();
            if (auto keyword = m_keywords.find(remaining()); keyword) {
                advance(keyword->size());
                emit(Token(Token::Kind::Keyword, {.keyword = keyword}));
            } else {
                advance();
                emit({});
            }
        } break;
        };
    }

    return token;
}

} // namespace Lexer
