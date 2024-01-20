#ifndef __CLUSCHEME__LEXER__KEYWORDS_HH__
#define __CLUSCHEME__LEXER__KEYWORDS_HH__

#include <cluscheme/lexer/token.hh>
#include <functional>
#include <iostream>
#include <string>

namespace Lexer
{

struct Trie {
    struct Node {
        Node *m_children[128];
        const std::string *m_value;

        Node();
        ~Node() = default;
    } m_root;

    Trie()  = default;
    ~Trie() = default;

    void insert(const std::string &);
    const std::string *get(const std::string &) const;
};

class Keywords : private Trie
{
  public:
    Keywords()  = default;
    ~Keywords() = default;

    void add(const std::string &);
    const std::string *find(const std::string &) const;
};

} // namespace Lexer

#endif
