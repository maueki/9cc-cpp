#include <cstdio>
#include <unordered_map>
#include <string>

#include "9cc.hpp"

struct GenContext {
    std::unordered_map<std::string, int> vars;
    int current_offset = 0;
    int label_index = 0;

    int var_put(const std::string& var) {
        auto iter = vars.find(var);
        if (iter != vars.end()) {
            return iter->second;
        }

        current_offset += 8;
        vars.insert({var, current_offset});
        return current_offset;
    }

    std::string new_label() {
        char buf[100];
        sprintf(buf, ".Label%d", label_index++);
        return std::string(buf);
    }
};

void NodeGeneral::gen_lval(GenContext&) {
    error("代入の左辺値が変数ではありません");
}

void NodeNum::gen_lval(GenContext&) {
    error("代入の左辺値が変数ではありません");
}

void NodeIdent::gen_lval(GenContext& context) {
    int offset = context.var_put(name);
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", offset);
    printf("  push rax\n");
}

void NodeNum::gen(GenContext& context) {
    printf("  push %d\n", val);
    return;
}

void NodeIdent::gen(GenContext& context) {
    gen_lval(context);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
}

void NodeGeneral::gen(GenContext& context) {
    if (ty == '=') {
        lhs->gen_lval(context);
        rhs->gen(context);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    }

    if (ty == ND_RETURN) {
        lhs->gen(context);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    }

    lhs->gen(context);
    rhs->gen(context);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (ty) {
    case '+':
        printf("  add rax, rdi\n");
        break;
    case '-':
        printf("  sub rax, rdi\n");
        break;
    case '*':
        printf("  mul rdi\n");
        break;
    case '/':
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
    case '<':
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    case '>':
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

void NodeIf::gen(GenContext& context) {
    cond->gen(context);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    auto else_label = context.new_label();
    printf("  je %s\n", else_label.c_str());
    then->gen(context);
    if (els) {
        auto end_label = context.new_label();
        printf("  jmp %s\n", end_label.c_str());
        printf("%s:\n", else_label.c_str());
        els->gen(context);
        printf("%s:\n", end_label.c_str());
    } else {
        printf("%s:\n", else_label.c_str());
    }
}

void NodeIf::gen_lval(GenContext& context) {
    error("代入の左辺値が変数ではありません");
}

void NodeFor::gen(GenContext& context) {
    if (init) init->gen(context);
    printf("  pop rax\n");
    auto begin_label = context.new_label();
    auto end_label = context.new_label();
    printf("%s:\n", begin_label.c_str());
    if (cond) cond->gen(context);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je %s\n", end_label.c_str());
    if (block) block->gen(context);
    printf("  pop rax\n");
    if (proc) proc->gen(context);
    printf("  pop rax\n");
    printf("  jmp %s\n", begin_label.c_str());
    printf("%s:\n", end_label.c_str());
}

void NodeFor::gen_lval(GenContext& context) {
    error("代入の左辺値が変数ではありません");
}

void NodeWhile::gen(GenContext& context) {
    auto begin_label = context.new_label();
    auto end_label = context.new_label();
    printf("%s:\n", begin_label.c_str());
    cond->gen(context);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je %s\n", end_label.c_str());
    block->gen(context);
    printf("  pop rax\n");
    printf("  jmp %s\n", begin_label.c_str());
    printf("%s:\n", end_label.c_str());
}

void NodeWhile::gen_lval(GenContext& context) {
    error("代入の左辺値が変数ではありません");
}

void NodeBlock::gen(GenContext& context) {
    for(auto& n: block) {
        n->gen(context);
        printf("  pop rax\n");
    }

    printf("  push rax\n");
}

void NodeBlock::gen_lval(GenContext& context) {
    error("代入の左辺値が変数ではありません");
}

void code_gen(std::vector<Node*>& code) {
    auto context = GenContext{};

    for (auto n : code) {
        n->gen(context);
        printf("  pop rax\n");
    }
}
