/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.16.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
    Michael L. Scott, 2008-2020.
*/

#include <iostream>
#include <list>

#include "scan.h"

#ifdef debug // compile with -DDEBUG
    #define dbg(msg) std::cout << msg
#else
    #define dbg(...) 0
#endif

#define TAB(x) std::string(x, ' ')

struct Node {
    std::string name;
    std::list<Node*> children;
    bool sameline;

    Node(std::string name) {
        this->name = name;
        this->children.clear();
        sameline = false;
    }

    Node(std::string name, bool sameline) {
        this->name = name;
        this->children.clear();
        this->sameline = sameline;
    }
};

bool fail;

const std::string names[] = {"read", "write", "id", "literal", "gets", "if", "while", "end",
                        "equal", "nequal", "smaller", "larger", "smaller_or_equal", "larger_or_equal",
                        "add", "sub", "mul", "div", "lparen", "rparen", "eof"};

static token input_token;

void error() {
    throw "Unexpected " + names[input_token] + ": " + token_image;
}

Node* match(token expected) {
    if (input_token == expected) {
        dbg("matched " + names[input_token]);
        if (input_token == t_id || input_token == t_literal)
            dbg(": " + token_image);
        dbg("\n");
        token last_input_token = input_token;
        std::string last_token_image = token_image;
        input_token = scan();
        switch (last_input_token) {
            case t_id:
            case t_literal:
                return new Node(names[last_input_token] + " '" + last_token_image + "'");
            default:
                return new Node(last_token_image);
        }
    } else
        throw "Expected " + names[expected] + ", got " + names[input_token] + ": " + token_image;
    return nullptr;
}

Node* program();
Node* stmt_list();
Node* stmt();
Node* cond();
Node* expr();
Node* term();
Node* term_tail();
Node* factor();
Node* factor_tail();
Node* add_op();
Node* mul_op();
Node* r_op();

Node* program() {
    Node *node = new Node("program");
    try {
        switch (input_token) {
            case t_id:
            case t_read:
            case t_write:
            case t_if:
            case t_while:
            case t_eof:
                dbg("predict program --> stmt_list eof\n");
                node->children.push_back(stmt_list());
                match(t_eof);
                break;
            default: error();
        }
    } catch (std::string err) {
        fail = true;
        std::cout << "Error: " << err << std::endl;
        for (; ; input_token = scan())
            switch (input_token) {
                // if input_token in FIRST, retry
                case t_id:
                case t_read:
                case t_write:
                case t_if:
                case t_while:
                case t_eof:
                    std::cout << "Retry program on " + names[input_token] + ": " + token_image << std::endl;
                    return program(); 
                
                // if input_token in FOLLOW, return
            }
    }
    return node;
}

Node* stmt_list() {
    Node *node = new Node("stmt_list");
    Node* temp;
    try {
        switch (input_token) {
            case t_id:
            case t_read:
            case t_write:
            case t_if:
            case t_while:
                dbg("predict stmt_list --> stmt stmt_list\n");
                node->children.push_back(stmt());
                temp = stmt_list();
                if (temp != nullptr)
                    node->children.insert(node->children.end(), temp->children.begin(), temp->children.end());
                break;
            case t_end:
            case t_eof:
                dbg("predict stmt_list --> epsilon\n");
                break;          /* epsilon production */
            default: error();
        }
    } catch(std::string err) {
        fail = true;
        std::cout << "Error: " << err << std::endl;
        for (; ; input_token = scan())
            switch (input_token) {
                // if input_token in FIRST, retry
                case t_id:
                case t_read:
                case t_write:
                case t_if:
                case t_while:
                    std::cout << "Retry stmt_list on " + names[input_token] + ": " + token_image << std::endl;
                    return stmt_list(); 
                
                // if input_token in FOLLOW, return
                case t_end:
                case t_eof:
                    std::cout << "Skip stmt_list" << std::endl;
                    return nullptr;
            }
    }
    return node;
}

Node* stmt() {
    Node* node;
    Node* n1;
    Node* n2;
    try {
        switch (input_token) {
            case t_id:
                dbg("predict stmt --> id gets expr\n");
                n1 = match(t_id);
                node = match(t_gets);
                n2 = expr();
                node->children.push_back(n1);
                node->children.push_back(n2);
                return node;
            case t_read:
                dbg("predict stmt --> read id\n");
                node = match(t_read);
                node->children.push_back(match(t_id));
                return node;
            case t_write:
                dbg("predict stmt --> write expr\n");
                node = match(t_write);
                node->children.push_back(expr());
                return node;
            case t_if:
                dbg("predict stmt --> if cond stmt_list end\n");
                node = match(t_if);
                node->children.push_back(cond());
                node->children.push_back(stmt_list());
                match(t_end);
                return node;
            case t_while:
                dbg("predict stmt --> while cond stmt_list end\n");
                node = match(t_while);
                node->children.push_back(cond());
                node->children.push_back(stmt_list());
                match(t_end);
                return node;
            default: error();
        }
    } catch (std::string err) {
        fail = true;
        std::cout << "Error: " << err << std::endl;
        for (; ; input_token = scan())
            switch (input_token) {
                // if input_token in FIRST, retry
                case t_id:
                case t_read:
                case t_write:
                case t_if:
                case t_while:
                    std::cout << "Retry stmt on " + names[input_token] + ": " + token_image << std::endl;
                    return stmt();
                
                // if input_token in FOLLOW, return
                case t_end:
                case t_eof:
                    std::cout << "Skip stmt" << std::endl;
                    return nullptr;
            }
    }
    return node;
}

Node* cond() {
    Node* node;
    Node* n1;
    Node* n2;
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                dbg("predict cond --> expr r_op expr\n");
                n1 = expr();
                node = r_op();
                n2 = expr();
                node->children.push_back(n1);
                node->children.push_back(n2);
                return node;
            default: error();
        }
    } catch (std::string err) {
        fail = true;
        std::cout << "Error: " << err << std::endl;
        for (; ; input_token = scan())
            switch (input_token) {
                // if input_token in FIRST, retry
                case t_id:
                case t_literal:
                case t_lparen:
                    std::cout << "Retry cond on " + names[input_token] + ": " + token_image << std::endl;
                    return cond(); 
                
                // if input_token in FOLLOW, return
                case t_read:
                case t_write:
                case t_if:
                case t_while:
                case t_end:
                case t_eof:
                    std::cout << "Skip cond" << std::endl;
                    return nullptr;
            }
    }
    return node;
}

Node* expr() {
    Node* node;
    Node* n1;
    Node* n2;
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                dbg("predict expr --> term term_tail\n");
                n1 = term();
                n2 = term_tail();
                if (n2 == nullptr)
                    return n1;
                else {
                    n2->children.push_front(n1);
                    return n2;
                }
            default: error();
        }
    } catch(std::string err) {
        fail = true;
        std::cout << "Error: " << err << std::endl;
        for (; ; input_token = scan())
            switch (input_token) {
                // if input_token in FIRST, retry
                case t_id:
                case t_literal:
                case t_lparen:
                    std::cout << "Retry expr on " + names[input_token] + ": " + token_image << std::endl;
                    return expr();
                
                // if input_token in FOLLOW, return
                case t_read:
                case t_write:
                case t_if:
                case t_while:
                case t_end:
                case t_eof:
                    std::cout << "Skip expr" << std::endl;
                    return nullptr;
            }
    }
}

Node* term() {
    Node* node;
    Node* n1;
    Node* n2;
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            dbg("predict term --> factor factor_tail\n");
            n1 = factor();
            n2 = factor_tail();
            if (n2 == nullptr)
                return n1;
            else {
                n2->children.push_front(n1);
                return n2;
            }
        default: error();
    }
}

Node* term_tail() {
    Node* node;
    Node* n1;
    Node* n2;
    switch (input_token) {
        case t_add:
        case t_sub:
            dbg("predict term_tail --> add_op term term_tail\n");
            node = add_op();
            n1 = term();
            n2 = term_tail();
            if (n2 == nullptr)
                node->children.push_back(n1);
            else {
                n2->children.push_front(n1);
                node->children.push_back(n2);
            }
            return node;
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_equal:
        case t_nequal:
        case t_smaller:
        case t_smaller_or_equal:
        case t_larger:
        case t_larger_or_equal:
        case t_if:
        case t_while:
        case t_end:
        case t_eof:
            dbg("predict term_tail --> epsilon\n");
            return nullptr;          /* epsilon production */
        default: error();
    }
}

Node* factor() {
    Node* node;
    switch (input_token) {
        case t_literal:
            dbg("predict factor --> literal\n");
            return match(t_literal);
        case t_id :
            dbg("predict factor --> id\n");
            return match(t_id);
        case t_lparen:
            dbg("predict factor --> lparen expr rparen\n");
            match(t_lparen);
            node = expr();
            match(t_rparen);
            return node;
        default: error();
    }
}

Node* factor_tail() {
    Node* node;
    Node* n1;
    Node* n2;
    switch (input_token) {
        case t_mul:
        case t_div:
            dbg("predict factor_tail --> mul_op factor factor_tail\n");
            node = mul_op();
            n1 = factor();
            n2 = factor_tail();
            if (n2 == nullptr)
                node->children.push_back(n1);
            else {
                n2->children.push_front(n1);
                node->children.push_back(n2);
            }
            return node;
        case t_add:
        case t_sub:
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_equal:
        case t_nequal:
        case t_smaller:
        case t_smaller_or_equal:
        case t_larger:
        case t_larger_or_equal:
        case t_if:
        case t_while:
        case t_end:
        case t_eof:
            dbg("predict factor_tail --> epsilon\n");
            return nullptr;          /* epsilon production */
        default: error();
    }
}

Node* add_op() {
    switch (input_token) {
        case t_add:
            dbg("predict add_op --> add\n");
            return match(t_add);
        case t_sub:
            dbg("predict add_op --> sub\n");
            return match(t_sub);
        default: error();
    }
}

Node* mul_op() {
    switch (input_token) {
        case t_mul:
            dbg("predict mul_op --> mul\n");
            return match(t_mul);
        case t_div:
            dbg("predict mul_op --> div\n");
            return match(t_div);
        default: error();
    }
}

Node* r_op() {
    switch (input_token) {
        case t_equal:
            dbg("predict r_op --> equal\n");
            return match(t_equal);
        case t_nequal:
            dbg("predict r_op --> nequal\n");
            return match(t_nequal);
        case t_smaller:
            dbg("predict r_op --> smaller\n");
            return match(t_smaller);
        case t_smaller_or_equal:
            dbg("predict r_op --> smaller_or_equal\n");
            return match(t_smaller_or_equal);
        case t_larger:
            dbg("predict r_op --> larger\n");
            return match(t_larger);
        case t_larger_or_equal:
            dbg("predict r_op --> larger_or_equal\n");
            return match(t_larger_or_equal);
        default: error();
    }
}

// void printTabs(int tabs) {
//     for (int i = 0; i < tabs; i++)
//         std::cout << "\t";
// }

void printTree(Node* node, int tabs) {
    std::string lparen, rparen, name;
    if (node->name == "stmt_list") {
        name = "";
        lparen = "[";
        rparen = "]";
    } else {
        name = node->name;
        lparen = "(";
        rparen = ")";
    }

    if (node->children.size() > 0) {
        std::cout << TAB(tabs) << lparen << name << std::endl;
        for (Node* child: node->children)
            printTree(child, tabs+2);
        std::cout << TAB(tabs) << rparen << "\n";
    } else 
        std::cout << TAB(tabs) << lparen << name << rparen << "\n";
}

int main() {
    input_token = scan();
    fail = false;
    Node* root = program();
    if (!fail) {
        printTree(root, 0);
    } else {
        std::cout << "Compilation failed" << std::endl;
    }
    return 0;
}
