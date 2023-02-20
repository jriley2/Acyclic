#include "Task.H"
#include <gtest/gtest.h>

using namespace RunTimeInternal;

TEST(FuncTraits, int_void)
{
  FuncTraits<std::function<int()>> ft;
  EXPECT_EQ(ft.nArgs, 0);
}

TEST(FuncTraits, void_void)
{
  FuncTraits<std::function<void()>> ft;
  EXPECT_EQ(ft.nArgs, 0);
}

TEST(FuncTraits, multipleArgs)
{
  FuncTraits<std::function<int(int, float, double)>> ft;
  EXPECT_EQ(ft.nArgs, 3);

  using FT = FuncTraits<std::function<int(int, float, double)>>;
  EXPECT_EQ(FT::nArgs, 3);
  FT::args_tuple_type tup = std::tuple<int, float&&, const double&>(1, 1.f, 1.);
  (void)tup;
}

TEST(FuncTraits, returnType)
{
  using FT = FuncTraits<std::function<int(int, float&&, const double&)>>;
  FT::return_type r = 2;
  (void)r; // prevent unused variable warning
}
