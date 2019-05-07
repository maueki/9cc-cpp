#include "9cc.hpp"

static int pos;

Node *new_node(int ty, Node *lhs, Node *rhs) {
    return new Node{ty, lhs, rhs, 0, 0};
}

Node *new_node_num(int val) {
    return new Node{ND_NUM, nullptr, nullptr, val, 0};
}

Node *new_node_ident(char c) {
    return new Node{ND_IDENT, nullptr, nullptr, 0, c};
}

int consume(int ty) {
    if (tokens[pos].ty != ty) return 0;
    pos++;
    return 1;
}

Node *stmt();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *term();
Node *unary();

/// syntax
///
/// program: stmt program;
/// program: ε
///
/// stmt: assign ";"
///
/// assign: equality
/// assign: equality "=" assign
///
/// equality: relational
/// euqality: euqality "==" relational
/// euqality: euqality "!=" relational
///
/// relational: add
/// relational: relational "<" add
/// relational: relational "<=" add
/// relational: relational ">" add
/// relational: relational ">=" add
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
/// term: "(" assign ")"Node *assign() {

std::vector<Node*> program() {
    std::vector<Node*> code;

    while (tokens[pos].ty != TK_EOF)
        code.push_back(stmt());

    return code;
}

Node *stmt() {
    Node *node = assign();
    if (!consume(';'))
        error("';'ではないトークンです: %s", tokens[pos].input);
    return node;
}

Node* assign() {
    Node *node = equality();
    if (consume('='))
        node = new_node('=', node, assign());
    return node;
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume(TK_EQ))
            node = new_node(ND_EQ, node, relational());
        else if (consume(TK_NE))
            node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume('<'))
            node = new_node('<', node, add());
        else if (consume(TK_LE))
            node = new_node(ND_LE, node, add());
        else if (consume('>'))
            node = new_node('>', node, add());
        else if (consume(TK_GE))
            node = new_node(ND_GE, node, add());
        else
            return node;
    }
}

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
        Node *node = equality();
        if (!consume(')'))
            error("開きカッコに対応する閉じカッコがありません: %s",
                  tokens[pos].input);
        return node;
    }

    if (tokens[pos].ty == TK_IDENT) return new_node_ident(*tokens[pos++].input);

    if (tokens[pos].ty == TK_NUM) return new_node_num(tokens[pos++].val);

    error("想定外のトークンです: %s", tokens[pos].input);

    return nullptr;  // unreachable
}

std::vector<Node*> parse() {
    pos = 0;
    return program();
}

#ifdef UNIT_TEST
void parser_init() {
    pos = 0;
}
#endif
