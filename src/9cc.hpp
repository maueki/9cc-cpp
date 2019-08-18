#include <vector>
#include <string>

// トークンの型を表す値
enum {
    TK_RESERVED,
    TK_NUM, //! 整数トークン
    TK_EOF,       //! 入力の終わりを表すトークン
};

// トークンの型
struct Token {
    int ty;       //! トークンの型
    int val;      //! tyがTK_NUMの場合、その数値
    std::string reserved; //! tyがTK_RESERVEDの場合
    const char *input;  //! トークン文字列（エラーメッセージ用）
};

enum {
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
    ND_NUM,        //! 整数のノードの型
};

struct Node {
    int ty;            // 演算子かND_NUM
    struct Node *lhs;  // 左辺
    struct Node *rhs;  // 右辺
    int val;           // tyがND_NUMの場合のみ使う
};

extern std::vector<Token> tokens;

void tokenize(const char *p);
Node *parse();
void gen(Node *node);

void error(const char *fmt, ...);
