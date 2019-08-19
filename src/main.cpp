#include <cstdio>

#include "9cc.hpp"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // トークナイズしてパースする
    std::vector<Token> tokens = tokenize(argv[1]);
    std::vector<Node*> nodes = program(tokens);

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // プロローグ
    // 変数26古個分の領域を確保する
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    for(auto&& node: nodes) {
        gen(node);
        printf("  pop rax\n");
    }
    // 抽象構文木を下りながらコード生成


    // エピローグ
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
}
