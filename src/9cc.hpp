#include <vector>
#include <string>

// トークンの型を表す値
enum TokenKind{
    TK_RESERVED,
    TK_NUM, //! 整数トークン
    TK_IDENT,
    TK_EOF,       //! 入力の終わりを表すトークン
};

// トークンの型
struct Token {
    TokenKind ty;       //! トークンの型
    int val;      //! tyがTK_NUMの場合、その数値
    std::string reserved; //! tyがTK_RESERVEDの場合
    std::string ident;  //! tyがTK_IDENTの場合
    const char *input;  //! トークン文字列（エラーメッセージ用）
};

enum NodeKind {
    ND_ADD,        //! +
    ND_SUB,        //! -
    ND_MUL,        //! *
    ND_DIV,        //! /
    ND_EQ,         //! ==
    ND_NE,         //! !=
    ND_LT,         //! <
    ND_LE,         //! <=
    ND_GT,         //! >
    ND_GE,         //! >=
    ND_ASSIGN,     //! =
    ND_LVAL,       //! ローカル変数
    ND_RETURN,     //! return
    ND_NUM,        //! 整数のノードの型
};

 struct Node {
    virtual void gen() = 0;
    virtual void gen_lval() = 0;
};

struct NodeGeneral : public Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;

    NodeGeneral(int ty, Node *lhs, Node *rhs) : ty(ty), lhs(lhs), rhs(rhs) {}

    void gen() override;
    void gen_lval() override;
};

struct NodeNum : public Node {
    int val;

    explicit NodeNum(int val) : val(val) {}

    void gen() override;
    void gen_lval() override;
};

struct NodeIdent : public Node {
    size_t offset;

    explicit NodeIdent(size_t offset) : offset(offset) {}

    void gen() override;
    void gen_lval() override;
 };

std::vector<Token> tokenize(const char *p);
std::vector<Node*> program(const std::vector<Token>&);
void gen(Node *node);

void error(const char *fmt, ...);
