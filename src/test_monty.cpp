#include <gtest/gtest.h>

#include "ast.h"

namespace Monty {
namespace AST {

TEST(Binary,TestingWorks) {
    EXPECT_EQ(1,1);
}

TEST(Binary,SEQ) {
    Message m("{}");
    
    Binary b1(Binary::Type::SEQ, new Value("foo"), new Value("bar") );
    EXPECT_FALSE(b1.eval(m));

    Binary b2(Binary::Type::SEQ, new Value("foo"), new Value("foo") );
    EXPECT_TRUE(b2.eval(m));
    
    Binary b3(Binary::Type::SEQ, new Value(""), new Value("") );
    EXPECT_TRUE(b3.eval(m));
    
    Binary b4(Binary::Type::SEQ, new Value(""), new Value("foo") );
    EXPECT_FALSE(b4.eval(m));
    
    Binary b5(Binary::Type::SEQ, new Value("4"), new Value("4") );
    EXPECT_TRUE(b5.eval(m));
    
    Binary b6(Binary::Type::SEQ, new Value("4"), new Value("04") );
    EXPECT_FALSE(b6.eval(m));
}

TEST(Binary,EQ) {
    Message m("{}");

    Binary b1(Binary::Type::EQ, new Value("foo"), new Value("bar") );
    EXPECT_FALSE(b1.eval(m));
}

}
}
