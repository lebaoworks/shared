#include <gtest/gtest.h>
#include <stdio.h>

#include "collections.h"
using namespace cpp;

class Dummy
{
public:
    int var = 0;

    Dummy() {
        printf("%s()\n", __FUNCTION__);
    }
    Dummy(int x) : var(x)
    {
        printf("%s(int)\n", __FUNCTION__);
    }
    Dummy(const Dummy& b)
    {
        printf("%s(const Dummy&)\n", __FUNCTION__);
        var = b.var;
    }
    Dummy(Dummy&& b)
    {
        printf("%s(Dummy&&)\n", __FUNCTION__);
        var = b.var;
        b.var = 0;
    }
};


TEST (List, push_front) { 
    List<int> l;
    for (int i=0; i<100; i++)
        l.push_front(i);
}

TEST (List, push_back) { 
    List<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
}

TEST (List, erase_1) {
    List<int> l;
    for (int i=0; i<100; i++)
    {
        l.push_back(i);
        l.erase(l.begin());
    }
}

TEST (List, erase_2) {
    List<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    for (int i=0; i<100; i++)
        l.erase(l.begin());
}

TEST (List, empty) {
    List<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    ASSERT_EQ(l.empty(), false);

    for (int i=0; i<100; i++)
        l.erase(l.begin());
    ASSERT_EQ(l.empty(), true);
}

TEST (List, iterator_1) {
    List<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    auto ite = l.begin();
    for (int i=0; i<100; i++, ite++)
        ASSERT_EQ(*ite, i);
}

TEST (List, iterator_2) {
    List<int> l;
    for (int i=0; i<100; i++)
        l.push_back(i);
    auto i = 0;
    for (auto ite=l.begin(); ite!=l.end(); ite++, i++)
        ASSERT_EQ(*ite, i);
}

TEST (List, object) { 
    List<Dummy> l;
    Dummy dummy;
    l.push_back(Dummy(1));
    l.push_back(dummy);
    l.push_back(Dummy(2));
}

TEST (List, perfomance) {
    List<int> l;
    for (int i=0; i<1000000; i++)
        l.push_back(i);
}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}