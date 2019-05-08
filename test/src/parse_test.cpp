#include <gtest/gtest.h>
#include <ostream>

#include "9cc.hpp"

void parser_init();
Node* stmt();
Node* assign();
Node* equality();
Node* add();

bool operator==(const Node& lhs, const Node& rhs) {
    try {
        auto l = dynamic_cast<const NodeGeneral&>(lhs);
        auto r = dynamic_cast<const NodeGeneral&>(rhs);

        return l.ty == r.ty &&
               ((l.lhs == nullptr && r.lhs == nullptr) ||
                (l.lhs != nullptr && r.lhs != nullptr && (*l.lhs == *r.lhs))) &&
               ((l.rhs == nullptr && r.rhs == nullptr) ||
                (l.rhs != nullptr && r.rhs != nullptr && (*l.rhs == *r.rhs)));
    } catch (...) {
    }

    try {
        auto l = dynamic_cast<const NodeIdent&>(lhs);
        auto r = dynamic_cast<const NodeIdent&>(rhs);

        return l.name == r.name;
    } catch (...) {
    }

    try {
        auto l = dynamic_cast<const NodeNum&>(lhs);
        auto r = dynamic_cast<const NodeNum&>(rhs);

        return l.val == r.val;
    } catch (...) {
    }

    return false;
}

Node* new_node(int ty, Node* lhs, Node* rhs);
Node* new_node_num(int val);
Node* new_node_ident(const std::string& s);
Node* new_node_return(Node* lhs);

class ParseTest : public testing::Test {};

TEST_F(ParseTest, add_test) {
    {
        tokenize("3");
        parser_init();
        Node* actual = add();
        Node* expect = new_node_num(3);
        EXPECT_EQ(*actual, *expect);
    }

    {
        tokenize("3+5");
        parser_init();
        Node* actual = add();

        Node* expect = new_node('+', new_node_num(3), new_node_num(5));
        EXPECT_EQ(*actual, *expect);
    }
}

TEST_F(ParseTest, equality_test) {
    {
        tokenize("1==2");
        parser_init();
        Node* actual = equality();
        Node* expect = new_node(ND_EQ, new_node_num(1), new_node_num(2));
        EXPECT_EQ(*actual, *expect);
    }

    {
        tokenize("1<2==2");
        parser_init();
        Node* actual = equality();
        Node* expect =
            new_node(ND_EQ, new_node('<', new_node_num(1), new_node_num(2)),
                     new_node_num(2));
        EXPECT_EQ(*actual, *expect);
    }

    {
        tokenize("(1==2)==1");
        parser_init();
        Node* actual = equality();
        Node* expect =
            new_node(ND_EQ, new_node(ND_EQ, new_node_num(1), new_node_num(2)),
                     new_node_num(1));
        EXPECT_EQ(*actual, *expect);
    }
}

TEST_F(ParseTest, assign_test) {
    {
        tokenize("a=1");
        parser_init();
        Node* actual = assign();
        Node* expect = new_node('=', new_node_ident("a"), new_node_num(1));
        EXPECT_EQ(*actual, *expect);
    }
}

TEST_F(ParseTest, stmt_test) {
    {
        tokenize("return a+b;");
        parser_init();
        Node* actual = stmt();
        Node* expect = new_node_return(
            new_node('+', new_node_ident("a"), new_node_ident("b")));
        EXPECT_EQ(*actual, *expect);
    }

    {
        tokenize("foo=1;");
        parser_init();
        Node* actual = stmt();
        Node* expect = new_node('=', new_node_ident("foo"), new_node_num(1));
        EXPECT_EQ(*actual, *expect);
    }

    {
        tokenize("foo+bar;");
        parser_init();
        Node* actual = stmt();
        Node* expect =
            new_node('+', new_node_ident("foo"), new_node_ident("bar"));
        EXPECT_EQ(*actual, *expect);
    }
}
