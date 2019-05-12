#include <vector>
#include <string>

// トークンの型を表す値
enum {
    TK_NUM = 256,  //! 整数トークン
    TK_IDENT,      //! 識別子
    TK_EQ,         //! ==
    TK_NE,         //! !=
    TK_LE,         //! <=
    TK_GE,         //! >=
    TK_RETURN,     //! return
    TK_EOF,        //! 入力の終わりを表すトークン
    TK_IF,         //! if
    TK_ELSE,       //! else
    TK_FOR,        //! for
    TK_WHILE,      //! while
};

// トークンの型
struct Token {
    int ty;            //! トークンの型
    int val;           //! tyがTK_NUMの場合、その数値
    std::string name;  //! tyがTK_IDENTの場合、その名前
    const char *input;  //! トークン文字列（エラーメッセージ用）
};

enum {
    ND_NUM = 256,  //! 整数のノードの型
    ND_IDENT,      //! 識別子のノードの型
    ND_EQ,         //! ==
    ND_NE,         //! !=
    ND_LE,         //! <=
    ND_GE,         //! >=
    ND_RETURN,     //! return
};

struct Node {
    virtual void gen(struct GenContext &) = 0;
    virtual void gen_lval(struct GenContext &) = 0;
};

struct NodeGeneral : public Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;

    NodeGeneral(int ty, Node *lhs, Node *rhs) : ty(ty), lhs(lhs), rhs(rhs) {}

    void gen(struct GenContext &) override;
    void gen_lval(struct GenContext &) override;
};

struct NodeNum : public Node {
    int val;

    explicit NodeNum(int val) : val(val) {}

    void gen(struct GenContext &) override;
    void gen_lval(struct GenContext &) override;
};

struct NodeIdent : public Node {
    std::string name;

    explicit NodeIdent(const std::string &name) : name(name) {}

    void gen(struct GenContext &) override;
    void gen_lval(struct GenContext &) override;
};

struct NodeIf : public Node {
    struct Node* cond;
    struct Node* then;
    struct Node* els;

    NodeIf(Node* cond, Node* then, Node* els): cond(cond), then(then), els(els) {}

    void gen(struct GenContext &) override;
    void gen_lval(struct GenContext &) override;
};

struct NodeFor : public Node {
    struct Node* init;
    struct Node* cond;
    struct Node* proc;
    struct Node* block;

    NodeFor(Node* init, Node* cond, Node* proc, Node* block)
        : init(init), cond(cond), proc(proc), block(block) {}

    void gen(struct GenContext &) override;
    void gen_lval(struct GenContext &) override;
};

struct NodeWhile: public Node {
    struct Node* cond;
    struct Node* block;

    NodeWhile(Node* cond, Node* block): cond(cond), block(block){}

    void gen(struct GenContext &) override;
    void gen_lval(struct GenContext &) override;
};

struct NodeBlock: public Node {
    std::vector<Node*> block;

    NodeBlock(std::vector<Node*>&& block): block(block) {}

    void gen(struct GenContext &) override;
    void gen_lval(struct GenContext &) override;
};

extern std::vector<Token> tokens;

void tokenize(const char *p);
std::vector<Node*> parse();
void code_gen(std::vector<Node*>& code);

void error(const char *fmt, ...);
