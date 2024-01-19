#include "cluscheme/tokenizer.hh"
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main(void)
{
    using Lexer = Tokenizer::Lexer;
    using Token = Tokenizer::Token;
    // using Trie = Tokenizer::Trie;
    // using StringParser = Tokenizer::StringParser;

    // Trie trie;
    // trie.insert("define");
    // trie.insert("define-var");
    // trie.insert("lambda");
    //
    // std::cout << "[ Checking\t]: 'define-var' -> "
    //           << (trie.check("define-var") ? "true" : "false") << std::endl;
    // std::cout << "[ Checking\t]: 'lamdbda' -> "
    //           << (trie.check("lamdbda") ? "true" : "false") << std::endl;
    // std::cout << "[ Checking\t]: 'lambda' -> "
    //           << (trie.check("lambda") ? "true" : "false") << std::endl;

    char buffer[256] = {0};
    ssize_t n;
    if (int fp = open("sample.scm", O_RDONLY)) {
        if ((n = read(fp, buffer, sizeof(buffer) - 1)) > 0) {
            std::cout << buffer << std::endl;
            Lexer lexer{buffer};
            std::vector<Token> tokens;

            for (Token token; token = lexer.tokenize(), 1;) {
                token.dump();
                tokens.push_back(token);
                if (token.m_kind == Token::Kind::Done)
                    break;
            }
            std::cout << std::endl;

            std::cout << "Code: " << buffer << std::endl;
            for (Token token : tokens) {
                token.dump();
            }
        }
        close(fp);
    }

    // StringParser parser{"Abhishek"};
    // std::cout << parser.remaining() << " " << parser.size() << std::endl;
    // std::cout << parser.consume("Abh") << std::endl;
    // std::cout << parser.remaining() << " " << parser.size() << std::endl;
    // std::cout << parser.consume("i") << std::endl;
    // std::cout << parser.remaining() << " " << parser.size() << std::endl;

    return 0;
}
