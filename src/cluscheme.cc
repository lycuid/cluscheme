#include "cluscheme/tokenizer.hh"
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main(void)
{
    // Tokenizer::Lexer lexer{};
    // char buffer[256] = {0};
    // ssize_t n;
    // if (int fp = open("sample.scm", O_RDONLY)) {
    //     while ((n = read(fp, buffer, sizeof(buffer) - 1)) > 0) {
    //         lexer.tokenize(buffer);
    //     }
    //     close(fp);
    // }

    Tokenizer::StringParser parser{"Abhishek"};
    std::cout << parser.remaining() << " " << parser.size() << std::endl;
    std::cout << parser.consume("Abh") << std::endl;
    std::cout << parser.remaining() << " " << parser.size() << std::endl;
    std::cout << parser.consume("i") << std::endl;
    std::cout << parser.remaining() << " " << parser.size() << std::endl;
    return 0;
}
