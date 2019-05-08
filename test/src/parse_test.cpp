#include <gtest/gtest.h>

#include "9cc.hpp"

void parser_init();
Node* stmt();
Node* assign();
Node* equality();
Node* add();

bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.ty == rhs.ty && lhs.val == rhs.val && lhs.name == rhs.name &&
        ((lhs.lhs == nullptr && rhs.lhs == nullptr) ||
         (lhs.lhs != nullptr && rhs.lhs != nullptr && (*lhs.lhs == *rhs.lhs))) &&
        ((lhs.rhs == nullptr && rhs.rhs == nullptr) ||
         (lhs.rhs != nullptr && rhs.rhs != nullptr && (*lhs.rhs == *rhs.rhs)));
}

Node* node_ident(char c) {
    return new Node{ND_IDENT, nullptr, nullptr, 0, c};
}

Node* node_num(int i) {
    return new Node{ND_NUM, nullptr, nullptr, i, 0};
}

Node* node(int ty, Node* lhs, Node* rhs) {
    return new Node{ty, lhs, rhs};
}

class ParseTest : public testing::Test {

};

TEST_F(ParseTest, add_test)
{
    {
        tokenize("3");
        parser_init();
        Node* actual = add();

        Node* expect = new Node{ND_NUM, nullptr, nullptr, 3};
        EXPECT_EQ(*actual, *expect);
    }

    {
        tokenize("3+5");
        parser_init();
        Node* actual = add();

        Node* expect = new Node{'+',
                                new Node{ND_NUM, nullptr, nullptr, 3},
                                new Node{ND_NUM, nullptr, nullptr, 5}, 0};
        EXPECT_EQ(*actual, *expect);
    }
}

TEST_F(ParseTest, equality_test)
{
    {
        tokenize("1==2");
        parser_init();
        Node* actual = equality();
        Node *expect = new Node{ND_EQ, new Node{ND_NUM, nullptr, nullptr, 1},
                                new Node{ND_NUM, nullptr, nullptr, 2}, 0};
        EXPECT_EQ(*actual, *expect);
    }

    {
        tokenize("1<2==2");
        parser_init();
        Node* actual = equality();
        Node* expect = new Node{ND_EQ,
                                new Node{'<',
                                         new Node{ND_NUM, nullptr, nullptr, 1},
                                         new Node{ND_NUM, nullptr, nullptr, 2},
                                         0},
                                new Node{ND_NUM, nullptr, nullptr, 2}, 0};
        EXPECT_EQ(*actual, *expect);
    }

    {
        tokenize("(1==2)==1");
        parser_init();
        Node* actual = equality();
        Node* expect = new Node{ND_EQ,
                                new Node{ND_EQ,
                                         new Node{ND_NUM, nullptr, nullptr, 1},
                                         new Node{ND_NUM, nullptr, nullptr, 2}, 0},
                                new Node{ND_NUM, nullptr, nullptr, 1}, 0};
        EXPECT_EQ(*actual, *expect);
    }

}

TEST_F(ParseTest, assign_test)
{
    {
        tokenize("a=1");
        parser_init();
        Node* actual = assign();
        Node *expect = node('=', node_ident('a'), node_num(1));
        EXPECT_EQ(*actual, *expect);
    }
}

TEST_F(ParseTest, stmt_test)
{
    {
        tokenize("return a+b;");
        parser_init();
        Node* actual = stmt();
        Node* expect = node(ND_RETURN, node('+', node_ident('a'), node_ident('b')), nullptr);
        EXPECT_EQ(*actual, *expect);
    }
}
