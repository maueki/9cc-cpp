#include <gtest/gtest.h>

#include "9cc.hpp"

void parser_init();
Node* add();

bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.ty == rhs.ty && lhs.val == rhs.val &&
        ((lhs.lhs == nullptr && rhs.lhs == nullptr) ||
         (lhs.lhs != nullptr && rhs.lhs != nullptr && (*lhs.lhs == *rhs.lhs))) &&
        ((lhs.rhs == nullptr && rhs.rhs == nullptr) ||
         (lhs.rhs != nullptr && rhs.rhs != nullptr && (*lhs.rhs == *rhs.rhs)));
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
