#include "9cc.hpp"

#include <cassert>

static int pos;

Node *new_node(int ty, Node *lhs, Node *rhs) {
    return new NodeGeneral{ty, lhs, rhs};
}

Node *new_node_num(int val) {
    return new NodeNum{val};
}

Node *new_node_ident(const std::string &s) {
    return new NodeIdent{s};
}

Node *new_node_return(Node *lhs) {
    return new NodeGeneral{ND_RETURN, lhs, nullptr};
}

Node *new_node_if(Node* cond, Node* then, Node* els) {
    assert(cond);
    assert(then);

    return new NodeIf{cond, then, els};
}

Node *new_node_for(Node* init, Node* cond, Node* proc, Node* block) {
    return new NodeFor{init, cond, proc, block};
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
/// stmt: "return" assign ";"
/// stmt: assign ";"
/// stmt: ifclause
/// stmt: forclause
///
/// ifclause: "if" "(" assign ")" stmt ["else" stmt]
///
/// forclause: "for" "(" assign ";" assign ";" assign ")" stmt
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

std::vector<Node *> program() {
    std::vector<Node *> code;

    while (tokens[pos].ty != TK_EOF) code.push_back(stmt());

    return code;
}

Node *stmt() {
    Node *node;

    if (consume(TK_RETURN)) {
        node = new_node_return(assign());
    } else if (consume(TK_IF)) {
        if (!consume('(')) error("'('ではないトークンです: %s", tokens[pos].input);
        auto cond = assign();
        if (!consume(')')) error("')'ではないトークンです: %s", tokens[pos].input);
        auto then = stmt();
        if (consume(TK_ELSE)) {
            auto els = stmt();
            return new_node_if(cond, then, els);
        } else {
            return new_node_if(cond, then, nullptr);
        }
    } else if (consume(TK_FOR)) {
        Node *init, *cond, *proc, *block;
        if (!consume('(')) error("'('ではないトークンです: %s", tokens[pos].input);

        if (consume(';')) {
            init = nullptr;
        } else {
            init = assign();
            if (!consume(';')) error("';'ではないトークンです: %s", tokens[pos].input);
        }

        if (consume(';')) {
            cond = nullptr;
        } else {
            cond = assign();
            if (!consume(';')) error("';'ではないトークンです: %s", tokens[pos].input);
        }

        if (consume(')')) {
            proc = nullptr;
        } else {
            proc = assign();
            if (!consume(')')) error("')'ではないトークンです: %s", tokens[pos].input);
        }

        if (consume(';')) {
            block = nullptr;
        } else {
            block = stmt();
        }

        return new_node_for(init, cond, proc, block);
    } else {
        node = assign();
    }

    if (!consume(';')) error("';'ではないトークンです: %s", tokens[pos].input);
    return node;
}

Node *assign() {
    Node *node = equality();
    if (consume('=')) node = new_node('=', node, assign());
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
    if (consume('+')) return term();
    if (consume('-')) return new_node('-', new_node_num(0), term());
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

    if (tokens[pos].ty == TK_IDENT) return new_node_ident(tokens[pos++].name);

    if (tokens[pos].ty == TK_NUM) return new_node_num(tokens[pos++].val);

    error("想定外のトークンです: %s", tokens[pos].input);

    return nullptr;  // unreachable
}

std::vector<Node *> parse() {
    pos = 0;
    return program();
}

#ifdef UNIT_TEST
void parser_init() {
    pos = 0;
}
#endif
