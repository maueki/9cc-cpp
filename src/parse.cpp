#include "9cc.hpp"

static int pos;

Node *new_node(int ty, Node *lhs, Node *rhs) {
    return new Node{ty, lhs, rhs, 0};
}

Node *new_node_num(int val) {
    return new Node{ND_NUM, nullptr, nullptr, val};
}

int consume(int ty) {
    if (tokens[pos].ty != ty) return 0;
    pos++;
    return 1;
}

static Node *mul();
static Node *term();
static Node *unary();

/// syntax
///
/// add: mul
/// add: add "+" mul
/// add: add "-" mul
///
/// mul: unary
/// mul: mul "*" unary
/// mul: mul "/" unary
///
/// unary: term
/// unary: "+" term
/// unary: "-" term
///
/// term: num
/// term: "(" add ")"

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume('+'))
            node = new_node('+', node, mul());
        else if (consume('-'))
            node = new_node('-', node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume('*'))
            node = new_node('*', node, unary());
        else if (consume('/'))
            node = new_node('/', node, unary());
        else
            return node;
    }
}

Node *unary() {
    if (consume('+'))
        return term();
    if (consume('-'))
        return new_node('-', new_node_num(0), term());
    return term();
}

Node *term() {
    // 次のトークンが'('なら、"(" add ")"のはず
    if (consume('(')) {
        Node *node = add();
        if (!consume(')'))
            error("開きカッコに対応する閉じカッコがありません: %s",
                  tokens[pos].input);
        return node;
    }

    // そうでなければ数値のはず
    if (tokens[pos].ty == TK_NUM) return new_node_num(tokens[pos++].val);

    error("数値でも開きカッコでもないトークンです: %s", tokens[pos].input);

    return nullptr;  // unreachable
}

Node *parse() {
    pos = 0;
    return add();
}

#ifdef UNIT_TEST
void parser_init() {
    pos = 0;
}
#endif
