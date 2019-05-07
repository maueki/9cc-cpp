#include "9cc.hpp"

#include <cstdarg>
#include <cstdlib>
#include <cstdio>

// エラーを報告するための関数
// printfと同じ引数を取る
void error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
