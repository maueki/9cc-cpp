#include "9cc.hpp"

#include <cassert>
#include <map>

static int pos;

struct Context {
    const std::vector<Token> tokens;
    size_t pos = 0;

    Context(const std::vector<Token> tokens): tokens(tokens) {}

    bool consume(const char* op) {
        assert(op);
        if (tokens[pos].ty != TK_RESERVED || tokens[pos].reserved != op) return false;
        pos++;
        return true;
    }

    const Token* consume_ident() {
        if (tokens[pos].ty == TK_IDENT) return &tokens[pos++];

        return nullptr;
    }

    bool expect(const char* op) {
        if (tokens_[pos_].ty != TK_RESERVED || tokens_[pos_].reserved != op)
            error("'%s'ではありません: %s", op, now_input());
        pos_++;
    }

    bool at_eof() const {
        return tokens_[pos_].ty == TK_EOF;
    }

private:
    std::map<std::string, size_t> idents_;
    size_t offset_ = 8;

public:
    size_t ident_to_offset(const std::string& ident) {
        if (idents_.count(ident) > 0) {
            return idents_[ident];
        }

        idents_[ident] = offset_;
        offset_ += 8;
        return idents_[ident];
    }
};

Node *new_node(NodeKind ty, Node *lhs, Node *rhs) {
    return new Node{ty, lhs, rhs, 0};
}

Node *new_node_num(int val) {
    return new Node{ND_NUM, nullptr, nullptr, val};
}

Node *stmt(Context& ctx);
Node *expr(Context& ctx);
Node *assign(Context& ctx);
Node *equality(Context& ctx);
Node *relational(Context& ctx);
Node *add(Context& ctx);
Node *mul(Context& ctx);
Node *term(Context& ctx);
Node *unary(Context& ctx);

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

std::vector<Node*> program(const std::vector<Token>& tokens) {
    std::vector<Node*> nodes;

    Context ctx{tokens};

    while(!ctx.at_eof()) {
        nodes.push_back(stmt(ctx));
    }

    return nodes;
}

Node *stmt(Context& ctx) {
    Node *node = expr(ctx);
    ctx.expect(";");
    return node;
}

Node* expr(Context& ctx) {
    return assign(ctx);
}

Node* assign(Context& ctx) {
    Node* node = equality(ctx);
    if (ctx.consume("="))
        node = new_node(ND_ASSIGN, node ,assign(ctx));
    return node;
}

Node *equality(Context& ctx) {
    Node *node = relational(ctx);

    for (;;) {
        if (ctx.consume("=="))
            node = new_node(ND_EQ, node, relational(ctx));
        else if (ctx.consume("!="))
            node = new_node(ND_NE, node, relational(ctx));
        else
            return node;
    }
}

Node *relational(Context& ctx) {
    Node *node = add(ctx);

    for (;;) {
        if (ctx.consume("<"))
            node = new_node(ND_LT, node, add(ctx));
        else if (ctx.consume("<="))
            node = new_node(ND_LE, node, add(ctx));
        else if (ctx.consume(">"))
            node = new_node(ND_GT, node, add(ctx));
        else if (ctx.consume(">="))
            node = new_node(ND_GE, node, add(ctx));
        else
            return node;
    }
}

Node *add(Context& ctx) {
    Node *node = mul(ctx);

    for (;;) {
        if (ctx.consume("+"))
            node = new_node(ND_ADD, node, mul(ctx));
        else if (ctx.consume("-"))
            node = new_node(ND_SUB, node, mul(ctx));
        else
            return node;
    }
}

Node *mul(Context& ctx) {
    Node *node = unary(ctx);

    for (;;) {
        if (ctx.consume("*"))
            node = new_node(ND_MUL, node, unary(ctx));
        else if (ctx.consume("/"))
            node = new_node(ND_DIV, node, unary(ctx));
        else
            return node;
    }
}

Node *unary(Context& ctx) {
    if (ctx.consume("+"))
        return term(ctx);
    if (ctx.consume("-"))
        return new_node(ND_SUB, new_node_num(0), term(ctx));
    return term(ctx);
}

Node *term(Context& ctx) {
    // 次のトークンが'('なら、"(" add ")"のはず
    if (ctx.consume("(")) {
        Node *node = equality(ctx);
        if (!ctx.consume(")"))
            error("開きカッコに対応する閉じカッコがありません: %s",
                  ctx.tokens[ctx.pos].input);
        return node;
    }

    const Token *tok = ctx.consume_ident();
    if (tok) {
        Node *node = new Node{};
        node->ty = ND_LVAL;
        node->offset = ctx.ident_to_offset(tok->ident);
        return node;
    }

    // そうでなければ数値のはず
    if (ctx.tokens[ctx.pos].ty == TK_NUM) return new_node_num(ctx.tokens[ctx.pos++].val);

    error("数値でも変数でも開きカッコでもないトークンです: %s", ctx.tokens[ctx.pos].input);

    return nullptr;  // unreachable
}

#ifdef UNIT_TEST
void parser_init() {
    pos = 0;
}
#endif
