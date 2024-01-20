#include "keywords.hh"
#include <cluscheme/lexer.hh>
#include <cstring>

namespace Lexer
{

static constexpr bool is_valid_keyword_char(char ch)
{
    switch (ch) {
    case '~': [[fallthrough]];
    case '!': [[fallthrough]];
    case '@': [[fallthrough]];
    case '#': [[fallthrough]];
    case '$': [[fallthrough]];
    case '%': [[fallthrough]];
    case '^': [[fallthrough]];
    case '&': [[fallthrough]];
    case '*': [[fallthrough]];
    case '_': [[fallthrough]];
    case '+': [[fallthrough]];
    case '-': [[fallthrough]];
    case '=': return true;
    }
    return Lexer::is_ascii_char(ch) || Lexer::is_digit(ch);
}

Trie::Node::Node() : m_value(nullptr)
{
    memset(m_children, 0, sizeof(m_children));
}

/*
 * @FIXME: Segfault.
 * */
// Trie::Node::~Node()
// {
//     for (auto child : m_children) {
//         if (child) {
//             if (child->m_value)
//                 delete child->m_value;
//             delete child;
//         }
//     }
// }

void Trie::insert(const std::string &str)
{
    Trie::Node *node = &m_root;
#if defined(DEBUG_LEXER)
    printf("[ Inserting\t]: '%s'\n", str.c_str());
#endif
    for (int ch : str) {
        if (!node->m_children[ch]) {
            node->m_children[ch] = new Trie::Node();
        }
        node = node->m_children[ch];
    }
    if (node != &m_root && str.size() > 0)
        node->m_value = new std::string(str);
}

const std::string *Trie::get(const std::string &str) const
{
#if defined(DEBUG_LEXER)
    printf("[ Checking\t]: '%s'\n", str.c_str());
#endif
    const Trie::Node *node = &m_root;
    for (int ch : str) {
        if (!node->m_children[ch]) {
            if (is_valid_keyword_char(ch))
                return nullptr;
            break;
        }
        node = node->m_children[ch];
    }
    return node->m_value;
}

void Keywords::add(const std::string &str) { Trie::insert(str); }

const std::string *Keywords::find(const std::string &str) const
{
    return Trie::get(str);
}

} // namespace Lexer
