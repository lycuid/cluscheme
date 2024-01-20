#ifndef __CLUSCHEME__LEXER_HH__
#define __CLUSCHEME__LEXER_HH__

#include <cluscheme/lexer/keywords.hh>
#include <cluscheme/lexer/token.hh>

namespace Lexer
{

class Lexer
{
    std::string m_buffer;
    size_t m_cursor;

    Keywords m_keywords;

  public:
    Lexer(const std::string &);
    ~Lexer() = default;

    size_t cursor() const { return m_cursor; }

    const std::string &code() const { return m_buffer; }
    Token next();

    static constexpr bool is_ascii_char(char ch)
    {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
    }

    static constexpr bool is_digit(char ch, int base = 10)
    {
        if (base == 16)
            return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') ||
                   (ch >= 'A' && ch <= 'F');
        return ch >= '0' && ch < (base + '0');
    }

  private:
    void feed(std::string);

    const char *remaining() const;
    size_t size() const;

    const char *peek(int = 0) const;
    char get();

    int advance(int = 1);
    inline int rollback();

    bool consume(char);
    bool consume(const std::string &);

    int parse_digit(int = 10);

    Token::Value::Number tokenize_number();
};

} // namespace Lexer

#endif
