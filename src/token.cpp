#include <cctype>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <tuple>
#include <vector>

#include "9cc.hpp"

static std::tuple<const char *, int> symbols[] = {
    {"==", TK_EQ}, {"!=", TK_NE}, {"<=", TK_LE}, {">=", TK_GE}};

// トークナイズした結果のトークン列はこのベクタに保存する
std::vector<Token> tokens;

// pが指している文字列をトークンに分割してtokensに保存する
void tokenize(const char *p) {
    tokens.clear();
loop:
    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        for (auto &&sym : symbols) {
            auto op = std::get<0>(sym);
            if (!strncmp(p, op, strlen(op))) {
                tokens.push_back(Token{std::get<1>(sym), 0, p});
                p += strlen(op);
                goto loop;
            }
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
            *p == ')' || *p == '<' || *p == '>') {
            tokens.push_back(Token{*p, 0, p});
            p++;
            continue;
        }

        if (isdigit(*p)) {
            auto last_p = p;
            char *tmp = nullptr;
            auto val = strtol(p, &tmp, 10);
            p = tmp;
            tokens.push_back(Token{TK_NUM, static_cast<int>(val), last_p});
            continue;
        }

        error("トークナイズできません: %s", p);
        exit(1);
    }

    tokens.push_back(Token{TK_EOF, 0, p});
}
