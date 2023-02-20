#include "RunTime.H"
#include <functional>
#include <iostream>
#include <memory>

using namespace RunTime;

int main(const int argc, const char* argv[])
{
  int numThreads = 1;
  if (argc > 1)
  {
    numThreads = std::atoi(argv[1]);
  }

  int a = 0;
  int b = 0;

  // construct a lambda
  auto firstFunc = [&](int a_val)
  {
    a = 1;
    return 0;
  };

  std::string test = "hello how are"
                     "you";

  // construct a functor
  // this acts like the lamdba [&b](int a_val){ b = a_val; };
  class SecondFunc
  {
   public:

    SecondFunc(int& a_b)
        : m_b(a_b){};

    int operator()(int a_val)
    {
      m_b = a_val;
      return 0;
    }

   private:

    int& m_b;
  };

  SecondFunc secondFunc(b);

  Scheduler& scheduler = Scheduler::getScheduler();
  scheduler.start(numThreads);
  // construct a task with a lambda
  std::shared_ptr<BaseTask> firstTaskPtr =
    scheduler.addTask<void(int)>("firstFunc", firstFunc, 0, Dependency(&a, DataAccess::write));
  // construct a task with a functor
  std::shared_ptr<BaseTask> secondTaskPtr =
    scheduler.addTask<int(int)>("secondFunc", secondFunc, 2, Dependency(&a, DataAccess::write));
  // construct a task with a rvalue lambda
  std::shared_ptr<BaseTask> thirdTaskPtr = scheduler.addTask<void(void)>(
    "thirdFunc", [&]() { ++b; }, Dependency(&a, DataAccess::write));
  firstTaskPtr->print();
  secondTaskPtr->print();
  thirdTaskPtr->print();
  scheduler.finish();
  std::cout << a << " " << b << std::endl;
  return 0;
}
