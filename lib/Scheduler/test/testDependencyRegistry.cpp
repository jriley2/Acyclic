#include "DependencyRegistry.H"
#include "Task.H"
#include <gtest/gtest.h>

using namespace RunTimeInternal;

TEST(DependencyRegistry, rr)
{
  DepRegistry dr;
  size_t id = 0;
  std::shared_ptr<BaseTask> task = std::make_shared<Task<std::function<void()>>>(
    "task", [&]() {}, id++);
  int* ptr = (int*)1;
  Dependency depR(ptr, DataAccess::read);
  auto cntPrereqs = dr.insert(task, depR);
  EXPECT_EQ(cntPrereqs, 0);
  cntPrereqs = dr.insert(task, depR);
  EXPECT_EQ(cntPrereqs, 0);
  Dependency depW(ptr, DataAccess::write);
  cntPrereqs = dr.insert(task, depW);
  EXPECT_EQ(cntPrereqs, 2);
  cntPrereqs = dr.insert(task, depW);
  EXPECT_EQ(cntPrereqs, 1);
  cntPrereqs = dr.insert(task, depR);
  EXPECT_EQ(cntPrereqs, 1);

  auto successors = task->successors();
  for (auto s : successors)
  {
    s->print();
  }
}