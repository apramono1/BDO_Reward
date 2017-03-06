#include "gtest/gtest.h"
#include "xxx.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(FunTest, Zero) {
  ASSERT_EQ(Fun(0), 0);
}

TEST(FunTest, Eleven) {
  ASSERT_EQ(Fun(11), 12);
}

class ClsTester : public ::testing::Test {
 public:
  void SetUp() {
    std::cout << "starting test on fixture " << this << std::endl;
  }
  void TearDown() {
    std::cout << "test done on fixture " << this << std::endl;
  }
};

TEST_F(ClsTester, Equal) {
  Cls cls(42);
  ASSERT_TRUE(cls.Equals(42));
}

TEST_F(ClsTester, NotEqual) {
  Cls cls(42);
  ASSERT_FALSE(cls.Equals(43));
}
