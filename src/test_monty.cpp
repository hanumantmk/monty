#include <gtest/gtest.h>

#include "ast.h"

namespace Monty {
namespace AST {

std::shared_ptr<Value> mv(const char * str)
{
    return std::shared_ptr<Value>(new Value(str));
}


TEST(Binary,TestingWorks) {
    EXPECT_EQ(1,1);
}

TEST(Binary,SEQ) {
    Message m("{}");
    
    Binary b1(Binary::Type::SEQ, mv("foo"), mv("bar") );
    EXPECT_FALSE(b1.eval(m));

    Binary b2(Binary::Type::SEQ, mv("foo"), mv("foo") );
    EXPECT_TRUE(b2.eval(m));
    
    Binary b3(Binary::Type::SEQ, mv(""), mv("") );
    EXPECT_TRUE(b3.eval(m));
    
    Binary b4(Binary::Type::SEQ, mv(""), mv("foo") );
    EXPECT_FALSE(b4.eval(m));
    
    Binary b5(Binary::Type::SEQ, mv("4"), mv("4") );
    EXPECT_TRUE(b5.eval(m));
    
    Binary b6(Binary::Type::SEQ, mv("4"), mv("04") );
    EXPECT_FALSE(b6.eval(m));
}

TEST(Binary,EQ) {
    Message m("{}");

    Binary b1(Binary::Type::EQ, mv("foo"), mv("4bar") );
    EXPECT_FALSE(b1.eval(m));

    Binary b2(Binary::Type::EQ, mv("4"), mv("4") );
    EXPECT_TRUE(b2.eval(m));
}

}
}
