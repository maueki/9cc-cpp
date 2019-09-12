#include <gtest/gtest.h>

#include "9cc.hpp"

Node* stmt_test(const std::vector<Token>& tokens);

bool operator==(const Node& lhs, const Node& rhs) {
    try {
        auto l = dynamic_cast<const NodeGeneral&>(lhs);
        auto r = dynamic_cast<const NodeGeneral&>(rhs);

        return l.ty == r.ty &&
               ((l.lhs == nullptr && r.lhs == nullptr) ||
                (l.lhs != nullptr && r.lhs != nullptr && (*l.lhs == *r.lhs))) &&
               ((l.rhs == nullptr && r.rhs == nullptr) ||
                (l.rhs != nullptr && r.rhs != nullptr && (*l.rhs == *r.rhs)));
    } catch (...) {}

    try {
        auto l = dynamic_cast<const NodeIdent&>(lhs);
        auto r = dynamic_cast<const NodeIdent&>(rhs);
        return l.offset == r.offset;
    } catch (...) {}

    try {
        auto l = dynamic_cast<const NodeNum&>(lhs);
        auto r = dynamic_cast<const NodeNum&>(rhs);
        return l.val == r.val;
    } catch (...) {}

    try {
        auto l = dynamic_cast<const NodeIf&>(lhs);
        auto r = dynamic_cast<const NodeIf&>(rhs);

        return *l.cond == *r.cond && *l.then == *r.then &&
            (l.els == r.els ||
             (l.els && r.els && *l.els == *r.els));
    } catch (...) {
    }

    return false;
}

class ParseTest : public testing::Test {
};

TEST_F(ParseTest, add_test)
{
    {
        auto tokens = tokenize("3;");
        Node* actual = stmt_test(tokens);

        Node* expect = new NodeNum{3};
        EXPECT_EQ(*actual, *expect);
    }

    {
        auto tokens = tokenize("3+5;");
        Node* actual = stmt_test(tokens);

        Node* expect = new NodeGeneral{ND_ADD,
                                new NodeNum{3},
                                new NodeNum{5}};
        EXPECT_EQ(*actual, *expect);
    }
}

TEST_F(ParseTest, equality_test)
{
    {
        auto tokens = tokenize("1==2;");
        Node* actual = stmt_test(tokens);
        Node *expect = new NodeGeneral{ND_EQ, new NodeNum{1}, new NodeNum{2}};
        EXPECT_EQ(*actual, *expect);
    }

    {
        auto tokens = tokenize("1<2==2;");
        Node* actual = stmt_test(tokens);
        Node* expect = new NodeGeneral{ND_EQ,
                                       new NodeGeneral{ND_LT,
                                                       new NodeNum{1},
                                                       new NodeNum{2}},
                                       new NodeNum{2}};
        EXPECT_EQ(*actual, *expect);
    }

    {
        auto tokens = tokenize("(1==2)==1;");
        Node* actual = stmt_test(tokens);
        Node* expect = new NodeGeneral{ND_EQ,
                                new NodeGeneral{ND_EQ,
                                         new NodeNum{1},
                                         new NodeNum{2}},
                                new NodeNum{1}};
        EXPECT_EQ(*actual, *expect);
    }

    {
        auto tokens = tokenize("foo+bar;");
        Node* actual = stmt_test(tokens);
        Node* expect =
            new NodeGeneral(ND_ADD, new NodeIdent{8}, new NodeIdent{16});
        EXPECT_EQ(*actual, *expect);
    }

    {
        auto tokens = tokenize("if (1==a) 1; else return 2;");
        Node* actual = stmt_test(tokens);
        Node* expect =
            new NodeIf{new NodeGeneral{ND_EQ, new NodeNum{1}, new NodeIdent{8}},
                       new NodeNum{1},
                       new NodeGeneral{ND_RETURN, new NodeNum{2}, nullptr}};
        EXPECT_EQ(*actual, *expect);
    }

    {
        auto tokens = tokenize("if (a==1+1) return a;");
        Node* actual = stmt_test(tokens);
        Node* expect =
            new NodeIf{new NodeGeneral{ND_EQ, new NodeIdent{8},
                                       new NodeGeneral{ND_ADD, new NodeNum{1}, new NodeNum{1}}},
                       new NodeGeneral{ND_RETURN, new NodeIdent{8}, nullptr}, nullptr};
        EXPECT_EQ(*actual, *expect);
    }
}
