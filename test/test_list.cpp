#include "list_lt.h"

#include <gtest/gtest.h>

using namespace leistd;

TEST(TestListForBaseDataType, PushAndPop) {
  list<int> list;
  list.push_back(1);
  list.push_back(2);
  list.push_back(3);
  // head->1 2 3
  EXPECT_EQ(list.size(), 3);
  list.pop_back();
  // head->1 2
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.front(), 1);
  EXPECT_EQ(list.back(), 2);

  // head->1 2
  list.pop_front();
  // head->2
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list.front(), 2);
  EXPECT_EQ(list.back(), 2);

  list.push_front(4);
  list.push_front(5);
  list.push_front(6);
  // head->6 5 4 2
  list.push_back(7);
  // head->6 5 4 2 7
  EXPECT_EQ(list.size(), 5);
  EXPECT_EQ(list.front(), 6);
  EXPECT_EQ(list.back(), 7);

  list.clear();
  EXPECT_EQ(list.size(), 0);
}

TEST(TestListForBaseDataType, Constructor) {
  list<int> l1 = {1, 2, 3, 4};
  list<int> l2 = l1;
  EXPECT_EQ(l2.size(), 4);
  EXPECT_EQ(l1.size(), 4);

  list<int> l3 = std::move(l1);
  EXPECT_EQ(l3.size(), 4);
  EXPECT_EQ(l1.size(), 0);

  l3.push_back(10);
  l2.push_back(20);
  EXPECT_EQ(l2.size(), 5);
  EXPECT_EQ(l3.size(), 5);

  swap(l2, l3);
  EXPECT_EQ(l3.back(), 20);
  EXPECT_EQ(l2.back(), 10);

  list<int> l5;
  l5 = l2;
  EXPECT_EQ(l5.size(), 5);
  EXPECT_EQ(l5.front(), 1);
  EXPECT_EQ(l5.back(), 10);
}

TEST(TestListForBaseDataType, Insert) {
  list<int> list;
  list.insert(list.begin(), 1);
  list.insert(list.begin(), 2);
  list.insert(list.begin(), 3);
  // head->3 2 1
  EXPECT_EQ(list.size(), 3);
  EXPECT_EQ(list.front(), 3);
  EXPECT_EQ(list.back(), 1);

  auto it = list.begin();
  std::advance(it, 2);
  list.insert(it, 4);
  list.pop_back();
  // head->3 2 4
  EXPECT_EQ(list.size(), 3);
  EXPECT_EQ(list.front(), 3);
  EXPECT_EQ(list.back(), 4);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}