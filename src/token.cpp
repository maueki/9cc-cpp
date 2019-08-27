#include <cctype>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <tuple>
#include <vector>

#include "9cc.hpp"

static const char* symbols[] = {"==", "!=", "<=",">="};

static const char* reserved[] = {"return"};

Token new_num_token(int val, const char* input) {
    return Token{TK_NUM, val, "", "", input};
}

Token new_reserved_token(const std::string& reserved, const char* input) {
    return Token{TK_RESERVED, 0, reserved, "", input};
}

Token new_ident_token(const std::string& ident, const char* input) {
    return Token{TK_IDENT, 0, "", ident, input};
}


// pが指している文字列をトークンに分割してtokensに保存する
std::vector<Token> tokenize(const char *p) {
    std::vector<Token> tokens;
loop:
    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        for (auto &&sym : symbols) {
            if (!strncmp(p, sym, strlen(sym))) {
                tokens.push_back(new_reserved_token(sym, p));
                p += strlen(sym);
                goto loop;
            }
        }

        for (auto &&sym : reserved) {
            if (!strncmp(p, sym, strlen(sym)) &&
                !std::isalnum(*(p+strlen(sym))) &&
                *(p+strlen(sym)) != '_') {
                tokens.push_back(new_reserved_token(sym, p));
                p += strlen(sym);
                goto loop;
            }
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
            *p == ')' || *p == '<' || *p == '>' || *p == '=' || *p == ';') {
            tokens.push_back(new_reserved_token(std::string(p, 1), p));
            p++;
            continue;
        }

        if (isdigit(*p)) {
            auto last_p = p;
            char *tmp = nullptr;
            auto val = strtol(p, &tmp, 10);
            p = tmp;
            tokens.push_back(new_num_token(static_cast<int>(val), last_p));
            continue;
        }

        if ( std::isalpha(*p) || *p == '_' ) {
            auto epos = std::find_if_not(p, p+1024, [](const char c) {
                                                        return std::isalnum(c) || c == '_';});
            tokens.push_back(new_ident_token(std::string(p, epos-p), p));
            p = epos;
            continue;
        }

        error("トークナイズできません: %s", p);
        exit(1);
    }

    tokens.push_back(Token{TK_EOF, 0, p});

    return tokens;
}
