#include "9cc.hpp"

#include <cassert>

static int pos;

Node *new_node(NodeKind ty, Node *lhs, Node *rhs) {
    return new Node{ty, lhs, rhs, 0};
}

Node *new_node_num(int val) {
    return new Node{ND_NUM, nullptr, nullptr, val};
}

bool consume(const char* op) {
    assert(op);
    if (tokens[pos].ty != TK_RESERVED || tokens[pos].reserved != op) return false;
    pos++;
    return true;
}

Token* consume_ident() {
    if (tokens[pos].ty == TK_IDENT) return &tokens[pos];

    return nullptr;
}

bool expect(const char* op) {
    if (tokens[pos].ty != TK_RESERVED || tokens[pos].reserved != op)
        error("'%s'ではありません", op);
    pos++;
}

bool at_eof() {
    return tokens[pos].ty == TK_EOF;
}

Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *term();
Node *unary();

/// syntax
///
/// program: stmt*
///
/// stmt: expr ";"
///
/// expr: assign
///
/// assign: equality ("=" assign)?
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
/// term: "(" equality ")"

std::vector<Node*> program() {
    std::vector<Node*> nodes;

    while(!at_eof()) {
        nodes.push_back(stmt());
    }

    return nodes;
}

Node *stmt() {
    Node *node = expr();
    expect(";");
    return node;
}

Node* expr() {
    return assign();
}

Node* assign() {
    Node* node = equality();
    if (consume("="))
        node = new_node(ND_ASSIGN, node ,assign());
    return node;
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if (consume("!="))
            node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume("<"))
            node = new_node(ND_LT, node, add());
        else if (consume("<="))
            node = new_node(ND_LE, node, add());
        else if (consume(">"))
            node = new_node(ND_GT, node, add());
        else if (consume(">="))
            node = new_node(ND_GE, node, add());
        else
            return node;
    }
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary() {
    if (consume("+"))
        return term();
    if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), term());
    return term();
}

Node *term() {
    // 次のトークンが'('なら、"(" add ")"のはず
    if (consume("(")) {
        Node *node = equality();
        if (!consume(")"))
            error("開きカッコに対応する閉じカッコがありません: %s",
                  tokens[pos].input);
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
        Node *node = new Node{};
        node->ty = ND_LVAL;
        node->offset = (tok->input[0] - 'a' + 1) * 8;
        return node;
    }

    // そうでなければ数値のはず
    if (tokens[pos].ty == TK_NUM) return new_node_num(tokens[pos++].val);

    error("数値でも開きカッコでもないトークンです: %s", tokens[pos].input);

    return nullptr;  // unreachable
}

Node *parse() {
    pos = 0;
    return equality();
}

#ifdef UNIT_TEST
void parser_init() {
    pos = 0;
}
#endif
