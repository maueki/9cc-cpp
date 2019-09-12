#include <cstdio>

#include "9cc.hpp"


std::string new_label() {
    static long label_index = 0;
    char buf[100];
    sprintf(buf, ".Label%ld", label_index++);
    return std::string(buf);
}

void NodeGeneral::gen_lval() {
    error("代入の左辺値が変数ではありません");
}

void NodeNum::gen_lval() {
    error("代入の左辺値が変数ではありません");
}

void NodeIdent::gen_lval() {
     printf("  mov rax, rbp\n");
     printf("  sub rax, %ld\n", offset);
     printf("  push rax\n");
 }

void NodeNum::gen() {
    printf("  push %d\n", val);
    return;
}

void NodeIdent::gen() {
    gen_lval();
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
}

void NodeGeneral::gen() {
    switch (ty) {
    case ND_ASSIGN:
        lhs->gen_lval();
        rhs->gen();

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    case ND_RETURN:
        lhs->gen();
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    }

    lhs->gen();
    rhs->gen();

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (ty) {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  mul rdi\n");
        break;
    case ND_DIV:
        printf("  mov rdx, 0\n");
        printf("  div rdi\n");
        break;
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_GT:
        printf("  cmp rdi, rax\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_GE:
        printf("  cmp rdi, rax\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    }

    printf("  push rax\n");
}

void NodeIf::gen() {
    cond->gen();
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    auto else_label = new_label();
    printf("  je %s\n", else_label.c_str());
    then->gen();
    if (els) {
        auto end_label = new_label();
        printf("  jmp %s\n", end_label.c_str());
        printf("%s:\n", else_label.c_str());
        els->gen();
        printf("%s:\n", end_label.c_str());
    } else {
        printf("%s:\n", else_label.c_str());
    }
}

void NodeIf::gen_lval() {
    error("代入の左辺値が変数ではありません");
}
