#include <cctype>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <tuple>

#include "9cc.hpp"

static std::tuple<const char *, int> symbols[] = {
    {"==", TK_EQ}, {"!=", TK_NE}, {"<=", TK_LE}, {">=", TK_GE}};

// トークナイズした結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする
Token tokens[100];

// pが指している文字列をトークンに分割してtokensに保存する
void tokenize(const char *p) {
    int i = 0;
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
                tokens[i].ty = std::get<1>(sym);
                tokens[i].input = p;
                i++;
                p += strlen(op);
                goto loop;
            }
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
            *p == ')') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            char *tmp = nullptr;
            tokens[i].val = strtol(p, &tmp, 10);
            p = tmp;
            i++;
            continue;
        }

        error("トークナイズできません: %s", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}
