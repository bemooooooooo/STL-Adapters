#include <gtest/gtest.h>
#include <lib/adapters.hpp>
#include <vector>
#include <map>

TEST(adapter, transform_test){
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> r1;
    for (int i: v | Transform([](int i){ return i*i; })){
        r1.push_back(i);
    }
    std::vector<int> r2 = {1, 4, 9, 16, 25, 36};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, filter_test){
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> r1;
    for (auto i: v | Filter([](int i){ return i%2; })){
        r1.push_back(i);
    }
    std::vector<int> r2 = {1, 3, 5};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, take_test){
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> r1;
    for (auto i: v | Take(3)){
        r1.push_back(i);
    }
    std::vector<int> r2 = {1, 2, 3};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, drop_test){
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> r1;
    for (auto i: v | Drop(3)){
        r1.push_back(i);
    }
    std::vector<int> r2 = {4, 5, 6};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, reverse_test){
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> r1;
    for (auto i: v | Reverse()){
        r1.push_back(i);
    }
    std::vector<int> r2 = {6, 5, 4, 3, 2, 1};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, keys_test){
    std::map<int, int> m = {{1,2}, {2,3}, {3,4}};
    std::vector<int> r1;
    for (auto i: m | Keys()){
        r1.push_back(i);
    }
    std::vector<int> r2 = {1, 2, 3};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, values_test){
    std::map<int, int> m = {{1,2}, {2,3}, {3,4}};
    std::vector<int> r1;
    for (auto i: m | Values()){
        r1.push_back(i);
    }
    std::vector<int> r2 = {2, 3, 4};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, transform_transform_test){
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> r1;
    for (auto i: v | Transform([](int i){ return i*i; }) | Transform([](int i){ return i*2; })){
        r1.push_back(i);
    }
    std::vector<int> r2 = {2, 8, 18, 32, 50, 72};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, transform_filter_test){
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> r1;
    for (auto i: v | Transform([](int i){ return i*i; }) | Filter([](int i){ return i%2; })){
        r1.push_back(i);
    }
    std::vector<int> r2 = {1, 9, 25};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, transform_take_test){
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> r1;
    for (auto i: v | Transform([](int i){ return i*i; }) | Take(3)){
        r1.push_back(i);
    }
    std::vector<int> r2 = {1, 4, 9};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, transform_drop_test){
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> r1;
    for (auto i: v | Transform([](int i){ return i*i; }) | Drop(3)){
        r1.push_back(i);
    }
    std::vector<int> r2 = {16, 25, 36};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, transform_reverse_test){
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> r1;
    for (auto i: v | Transform([](int i){ return i*i; }) | Reverse()){
        r1.push_back(i);
    }
    std::vector<int> r2 = {36, 25, 16, 9, 4, 1};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, transform_keys_test){
    std::map<int, int> m = {{1,2}, {2,3}, {3,4}};
    std::vector<int> r1;
    for (auto i: m | Keys() | Transform([](int i){ return i*i; })){
        r1.push_back(i);
    }
    std::vector<int> r2 = {1, 4, 9};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, transform_values_test){
    std::map<int, int> m = {{1,2}, {2,3}, {3,4}};
    std::vector<int> r1;
    for (auto i: m | Values() | Transform([](int i){ return i*i; })){
        r1.push_back(i);
    }
    std::vector<int> r2 = {4, 9, 16};
    ASSERT_EQ(r1,r2);
}

TEST(adapter, drop_take_test){
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> r1;
    for (auto i: v | Drop(3) | Take(2)){
        r1.push_back(i);
    }
    std::vector<int> r2 = {4, 5};
    ASSERT_EQ(r1,r2);
}
