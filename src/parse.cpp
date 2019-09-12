#include "9cc.hpp"

#include <cassert>
#include <map>
#include <optional>

static int pos;

class Context {
    const std::vector<Token> tokens_;
    size_t pos_ = 0;

public:
    Context(const std::vector<Token> tokens): tokens_(tokens) {}

    bool consume(const char* op) {
        assert(op);
        if (tokens_[pos_].ty != TK_RESERVED || tokens_[pos_].reserved != op) return false;
        pos_++;
        return true;
    }

    const Token* consume_ident() {
        if (tokens_[pos_].ty == TK_IDENT) return &tokens_[pos_++];

        return nullptr;
    }

    std::optional<int> consume_num() {
        if (tokens_[pos_].ty == TK_NUM) return tokens_[pos_++].val;

        return std::nullopt;
    }

    bool expect(const char* op) {
        if (tokens_[pos_].ty != TK_RESERVED || tokens_[pos_].reserved != op)
            error("'%s'ではありません: %s", op, now_input());
        pos_++;
    }

    bool at_eof() const {
        return tokens_[pos_].ty == TK_EOF;
    }

    const char* now_input() const {
        return tokens_[pos_].input;
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
    return new NodeGeneral{ty, lhs, rhs};
}

Node *new_node_num(int val) {
    return new NodeNum{val};
}

Node *new_node_return(Node *lhs) {
    return new NodeGeneral{ND_RETURN, lhs, nullptr};
}

Node *new_node_ident(size_t offset) {
    return new NodeIdent{offset};
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
/// stmt: "return" expr ";"
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

    if (ctx.consume("return")) {
        Node *node = expr(ctx);
        ctx.expect(";");
        return new_node_return(node);
    }

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
            error("開きカッコに対応する閉じカッコがありません: %s", ctx.now_input());
        return node;
    }

    const Token *tok = ctx.consume_ident();
    if (tok) {
        return new_node_ident(ctx.ident_to_offset(tok->ident));
    }

    // そうでなければ数値のはず
    if (auto opt_num = ctx.consume_num()) return new_node_num(opt_num.value());

    error("数値でも変数でも開きカッコでもないトークンです: %s", ctx.now_input());

    return nullptr;  // unreachable
}

#ifdef UNIT_TEST
Node* stmt_test(const std::vector<Token>& tokens) {
    Context context{tokens};

    return stmt(context);
}
#endif
