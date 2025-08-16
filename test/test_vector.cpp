#include <gtest/gtest.h>
#include "../include/vector.h"

using namespace leistd;

TEST(VectorTest, DefaultConstructor) {
    vector<int> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
}

TEST(VectorTest, PushBackAndSize) {
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    EXPECT_FALSE(v.empty());
    EXPECT_EQ(v.size(), 3);
    EXPECT_GE(v.capacity(), 3); // capacity >= size

    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
}

TEST(VectorTest, PopBack) {
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.pop_back();

    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);

    v.pop_back();
    v.pop_back();
    EXPECT_TRUE(v.empty());
}

TEST(VectorTest, CopyConstructor) {
    vector<int> v1;
    v1.push_back(10);
    v1.push_back(20);

    vector<int> v2 = v1; // 拷贝构造
    EXPECT_EQ(v1.size(), v2.size());
    EXPECT_EQ(v1, v2);

    // 修改 v2 不影响 v1
    v2.push_back(30);
    EXPECT_NE(v1, v2);
}

TEST(VectorTest, MoveConstructor) {
    vector<int> v1;
    v1.push_back(100);
    v1.push_back(200);

    vector<int> v2 = std::move(v1); // 移动构造
    EXPECT_EQ(v2.size(), 2);
    EXPECT_EQ(v2[0], 100);
    EXPECT_EQ(v2[1], 200);

    // v1 已经被置空
    EXPECT_TRUE(v1.empty());
}

TEST(VectorTest, CopyAssignment) {
    vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);

    vector<int> v2;
    v2 = v1; // 拷贝赋值
    EXPECT_EQ(v2.size(), 2);
    EXPECT_EQ(v1, v2);
}

TEST(VectorTest, MoveAssignment) {
    vector<int> v1;
    v1.push_back(5);
    v1.push_back(10);

    vector<int> v2;
    v2 = std::move(v1); // 移动赋值
    EXPECT_EQ(v2.size(), 2);
    EXPECT_EQ(v2[0], 5);
    EXPECT_EQ(v2[1], 10);
    EXPECT_TRUE(v1.empty());
}

TEST(VectorTest, OperatorAtThrows) {
    vector<int> v;
    v.push_back(42);
    EXPECT_NO_THROW(v.at(0));
    EXPECT_THROW(v.at(1), std::out_of_range);
}

TEST(VectorTest, ComparisonOperators) {
    vector<int> v1;
    vector<int> v2;
    v1.push_back(1);
    v1.push_back(2);
    v2.push_back(1);
    v2.push_back(2);

    EXPECT_TRUE(v1 == v2);
    v2.push_back(3);
    EXPECT_TRUE(v1 != v2);
}


TEST(VectorTest, Insert) {
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.insert(v.cbegin() + 1, 4);
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[1], 4);
    v.insert(v.begin() + 2, 999);
    EXPECT_EQ(v[2], 999);
}

TEST(VectorTest, InsertRange) {
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    vector<int> v2;
    v2.push_back(4);
    v2.push_back(5);
    v2.push_back(6);
    v.insert(v.begin() + 1, v2.begin(), v2.end());
    EXPECT_EQ(v.size(), 6);
    EXPECT_EQ(v[1], 4);
    EXPECT_EQ(v[2], 5);
    EXPECT_EQ(v[3], 6);

    vector v3{1, 2, 3, 4};
    vector v4 = {5, 6, 7, 8};
    EXPECT_EQ(v3.size(), 4);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
