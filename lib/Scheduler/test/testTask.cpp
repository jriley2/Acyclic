#include "Task.H"
#include <gtest/gtest.h>

using namespace RunTimeInternal;

TEST(Task, ConstructWithLambda)
{
  size_t id = 0;
  int a = 0;

  // construct a lambda
  auto firstFunc = [&](int a_val)
  {
    a = 1;
    return 0;
  };

  // construct a task with a lambda
  Task<std::function<int(int)>> task("firstFunc", firstFunc, 0, id);
  task.print();
  EXPECT_TRUE(task.id() == id);
  EXPECT_TRUE(a == 0);
  task();
  EXPECT_TRUE(a == 1);
}

// construct a functor
TEST(Task, ConstructWithFunctor)
{
  size_t id = 1;
  int b = 1;

  class SecondFunc
  {
   public:

    SecondFunc(int& a_b) : m_b(a_b){};

    int operator()(int a_val)
    {
      m_b = a_val;
      return 0;
    }

   private:

    int& m_b;
  };

  SecondFunc functor(b);

  // construct a task with a lambda
  Task<std::function<int(int)>> task("functor", functor, 0, id);
  task.print();
  EXPECT_TRUE(b == 1);
  task();
  EXPECT_TRUE(b == 0);
}

// construct a rvalue lambda
TEST(Task, ConstructWithRValueLambda)
{
  size_t id = 1;
  int b = 0;

  // construct a task with a lambda
  Task<std::function<int(int)>> task(
    "rvalue lambda", [&](int val) { return ++b; }, 0, id);
  task.print();
  task();

  EXPECT_TRUE(task.id() == id);
  EXPECT_TRUE(b == 1);
}

TEST(Task, void_int)
{
  size_t id = 1;
  int b = 0;

  Task<std::function<void(int)>> task(
    "rvalue lambda", [&](int val) { ++b; }, 0, id);
  task.print();
  task();

  EXPECT_TRUE(task.id() == id);
  EXPECT_TRUE(b == 1);
}

TEST(Task, int_void)
{
  size_t id = 1;
  int b = 0;

  Task<std::function<int(void)>> task(
    "rvalue lambda", [&]() { return ++b; }, id);
  task.print();
  task();

  EXPECT_TRUE(task.id() == id);
  EXPECT_TRUE(b == 1);
}

TEST(Task, void_void)
{
  size_t id = 1;
  int b = 0;

  Task<std::function<void(void)>> task(
    "rvalue lambda", [&]() { ++b; }, id);
  task.print();
  task();

  EXPECT_TRUE(task.id() == id);
  EXPECT_TRUE(b == 1);
}

TEST(Task, int_intR_float)
{
  size_t id = 1;
  int a = 2;
  float b = 3.14;
  float gval = 0.5;

  Task<std::function<int(int&, float)>> task(
    "rvalue lambda",
    [&](int& val, float valf)
    {
      val++;
      return gval += valf;
    },
    std::tuple<int&, float>(a, b), id);
  task.print();
  task();

  EXPECT_TRUE(task.id() == id);
  EXPECT_TRUE(a == 3);
  EXPECT_NEAR(0.5 + b, gval, 1.e-6);
}
