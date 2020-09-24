/* Simple ad-hoc scanner for the calculator language.
    Michael L. Scott, 2008-2020.
*/

#include <iostream>
#include <locale>

#include "scan.h"

std::string token_image;

token scan() {
    static int c = ' ';
        /* next available char; extra (int) width accommodates EOF */
    token_image = "";

    /* skip white space */
    while (isspace(c)) {
        c = getchar();
    }
    if (c == EOF)
        return t_eof;
    if (isalpha(c)) {
        do {
            token_image += c;
            if (token_image.length() >= MAX_TOKEN_LEN) {
                std::cout << "max token length exceeded\n";
                exit(1);
            }
            c = getchar();
        } while (isalpha(c) || isdigit(c) || c == '_');
        if (token_image == "read") return t_read;
        else if (token_image == "write") return t_write;
        else if (token_image == "if") return t_if;
        else if (token_image == "while") return t_while;
        else if (token_image == "end") return t_end;
        else return t_id;
    }
    else if (isdigit(c)) {
        do {
            token_image += c;
            c = getchar();
        } while (isdigit(c));
        return t_literal;
    } else switch (c) {
        case ':':
            if ((c = getchar()) != '=') {
                std::cerr << "error\n";
                exit(1);
            } else {
                c = getchar();
                token_image = ":=";
                return t_gets;
            }
            break;
        case '(': c = getchar(); token_image = "("; return t_lparen;
        case ')': c = getchar(); token_image = ")"; return t_rparen;
        case '+': c = getchar(); token_image = "+"; return t_add;
        case '-': c = getchar(); token_image = "-"; return t_sub;
        case '*': c = getchar(); token_image = "*"; return t_mul;
        case '/': c = getchar(); token_image = "/"; return t_div;
        case '=': c = getchar(); token_image = "="; return t_equal;
        case '<':
            c = getchar();
            if (c == '>') return c = getchar(), token_image = "<>", t_nequal;
            else if (c == '=') return c = getchar(), token_image = "<=", t_smaller_or_equal;
            else return token_image = "<", t_smaller;
        case '>':
            c = getchar();
            if (c == '=') return c = getchar(), token_image = ">=", t_larger_or_equal;
            else return token_image = ">", t_larger;
        default:
            std::cout << "error\n";
            exit(1);
    }
}
