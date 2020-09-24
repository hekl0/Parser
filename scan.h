/* Definitions the scanner shares with the parser
    Michael L. Scott, 2008-2020.
*/

typedef enum {t_read, t_write, t_id, t_literal, t_gets, t_if, t_while, t_end,
                t_equal, t_nequal, t_smaller, t_larger, t_smaller_or_equal, t_larger_or_equal,
                t_add, t_sub, t_mul, t_div, t_lparen, t_rparen, t_eof} token;

#define MAX_TOKEN_LEN 128
extern std::string token_image;

extern token scan();
