#include <cluscheme/ast.hh>
#include <cluscheme/lexer.hh>
#include <cluscheme/lexer/token.hh>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main(void)
{
    using AST::AST;
    using Lexer::Lexer;

    char buffer[256] = {0};
    ssize_t n;
    if (int fp = open("sample.scm", O_RDONLY)) {
        if ((n = read(fp, buffer, sizeof(buffer) - 1)) > 0) {
            printf("%s\n", buffer);

            buffer[n] = 0;
            Lexer lexer{buffer};
            AST ast{lexer};
        }
        close(fp);
    }

    return 0;
}
