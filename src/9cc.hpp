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
    ND_NUM,        //! 整数のノードの型
};

struct Node {
    NodeKind ty;            // 演算子かND_NUM
    struct Node *lhs;  // 左辺
    struct Node *rhs;  // 右辺
    int val;           // tyがND_NUMの場合のみ使う
    int offset;        // tyがND_LVALの場合のみ使う
};

std::vector<Token> tokenize(const char *p);
std::vector<Node*> program(const std::vector<Token>&);
void gen(Node *node);

void error(const char *fmt, ...);
