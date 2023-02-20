#include "Dependency.H"
#include <gtest/gtest.h>

using namespace RunTimeInternal;

TEST(Dependency, ReadAfterRead)
{
  int* ptr = (int*)1;
  Dependency dep1(ptr, DataAccess::read);
  Dependency dep2(ptr, DataAccess::read);
  auto [isPrereq, isLast] = dep1.isPrereqOf(dep1);
  EXPECT_FALSE(isLast);
  EXPECT_FALSE(isPrereq);
  std::tie(isPrereq, isLast) = dep1.isPrereqOf(dep2);
  EXPECT_FALSE(isLast);
  EXPECT_FALSE(isPrereq);
}

TEST(Dependency, ReadAfterWrite)
{
  void* ptr = (void*)1;
  Dependency dep1(ptr, DataAccess::write);
  Dependency dep2(ptr, DataAccess::read);
  auto [isPrereq, isLast] = dep1.isPrereqOf(dep2);
  EXPECT_TRUE(isLast);
  EXPECT_TRUE(isPrereq);
}

TEST(Dependency, WriteAfterWrite)
{
  int* ptr = (int*)1;
  Dependency dep1(ptr, DataAccess::write);
  Dependency dep2(ptr, DataAccess::write);
  auto [isPrereq, isLast] = dep1.isPrereqOf(dep2);
  EXPECT_TRUE(isLast);
  EXPECT_TRUE(isPrereq);
}

TEST(Dependency, WriteAfterRead)
{
  int* ptr = (int*)1;
  Dependency dep1(ptr, DataAccess::read);
  Dependency dep2(ptr, DataAccess::write);
  auto [isPrereq, isLast] = dep1.isPrereqOf(dep2);
  EXPECT_FALSE(isLast);
  EXPECT_TRUE(isPrereq);
}

TEST(Dependency, NoDepRAW)
{
  int* ptr = (int*)1;
  Dependency dep1(ptr, DataAccess::write);
  ptr = (int*)2;
  Dependency dep2(ptr, DataAccess::read);
  auto [isPrereq, isLast] = dep1.isPrereqOf(dep2);
  EXPECT_FALSE(isLast);
  EXPECT_FALSE(isPrereq);
}

TEST(Dependency, NoDepRAR)
{
  int* ptr = (int*)1;
  Dependency dep1(ptr, DataAccess::read);
  ptr = (int*)2;
  Dependency dep2(ptr, DataAccess::read);
  auto [isPrereq, isLast] = dep1.isPrereqOf(dep2);
  EXPECT_FALSE(isLast);
  EXPECT_FALSE(isPrereq);
}

TEST(Dependency, NoDepWAR)
{
  int* ptr = (int*)1;
  Dependency dep1(ptr, DataAccess::read);
  ptr = (int*)2;
  Dependency dep2(ptr, DataAccess::write);
  auto [isPrereq, isLast] = dep1.isPrereqOf(dep2);
  EXPECT_FALSE(isLast);
  EXPECT_FALSE(isPrereq);
}

TEST(Dependency, NoDepWAW)
{
  int* ptr = (int*)1;
  Dependency dep1(ptr, DataAccess::write);
  ptr = (int*)2;
  Dependency dep2(ptr, DataAccess::write);
  auto [isPrereq, isLast] = dep1.isPrereqOf(dep2);
  EXPECT_FALSE(isLast);
  EXPECT_FALSE(isPrereq);
}
