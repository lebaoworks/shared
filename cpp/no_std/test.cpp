#include <gtest/gtest.h>
#include <stdio.h>

#include "_std.h"
using namespace _std;

TEST (list, push_back_ref) { 
    list<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    int i=0;
    for (auto it=l.begin(); it!=l.end(); it++, i++)
        ASSERT_EQ(*it, i);
}

TEST (list, push_back_rvalue) { 
    list<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i+1);
    int i=1;
    for (auto it=l.begin(); it!=l.end(); it++, i++)
        ASSERT_EQ(*it, i);
}

TEST (list, push_front_ref) { 
    list<int> l;
    for (int i=0; i<100; i++)
        l.push_front(i);
    int i=99;
    for (auto it=l.begin(); it!=l.end(); it++, i--)
        ASSERT_EQ(*it, i);
}

TEST (list, push_front_rvalue) { 
    list<int> l;
    for (int i=0; i<100; i++)
        l.push_front(i+1);
    int i=100;
    for (auto it=l.begin(); it!=l.end(); it++, i--)
        ASSERT_EQ(*it, i);
}

TEST (list, pop_back) {
    list<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    for (int i=99; i>0; i--)
    {
        ASSERT_EQ(i, l.back());
        l.pop_back();
    }
}

TEST (list, pop_front) {
    list<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    for (int i=0; i<100; i++)
    {
        ASSERT_EQ(i, l.front());
        l.pop_front();
    }
}

TEST (list, erase_ref) {
    list<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    int c=0;
    for (auto it=l.begin(); it!=l.end(); c++)
    {
        ASSERT_EQ(c, *it);
        it = l.erase(it);
    }
}

TEST (list, erase_rvalue) {
    list<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    for (int i=1; i<100; i++)
        ASSERT_EQ(i, *l.erase(l.begin()));
}

TEST (list, clear) {
    list<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    l.clear();
    ASSERT_EQ(l.empty(), true);
}

TEST (list, empty) {
    list<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    ASSERT_EQ(l.empty(), false);

    for (int i=0; i<100; i++)
        l.erase(l.begin());
    ASSERT_EQ(l.empty(), true);
}

TEST (list, begin) {
    list<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    for (int i=0; i<100; i++)
    {
        ASSERT_EQ(*l.begin(), i);
        l.pop_front();
    }
}

TEST (list, end) {
    list<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    auto i = 0;
    for (auto ite=l.begin(); ite!=l.end(); ite++)
        i++;
    ASSERT_EQ(i, 100);
}

TEST (list, front) { 
    list<int> l;
    for (int i=0; i<100; i++)
    {
        l.push_front(i);
        ASSERT_EQ(i, l.front());
    }
}

TEST (list, back) { 
    list<int> l;
    for (int i=0; i<100; i++)
    {
        l.push_back(i);
        ASSERT_EQ(i, l.back());
    }
}

TEST (list, perfomance_1m_ints) {
    list<int> l;
    for (int i=0; i<1000000; i++)
        l.push_back(i);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}