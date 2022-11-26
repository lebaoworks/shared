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
    l.push_front(1);
    l.push_front(2);
    l.push_front(3);
    ASSERT_EQ(3, *l.begin());
    l.erase(l.begin());
    ASSERT_EQ(2, *l.begin());
    l.erase(l.begin());
    ASSERT_EQ(1, *l.begin());
}

TEST (List, push_back) { 
    List<int> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    ASSERT_EQ(1, *l.begin());
    l.erase(l.begin());
    ASSERT_EQ(2, *l.begin());
    l.erase(l.begin());
    ASSERT_EQ(3, *l.begin());
}

TEST (List, object) { 
    List<Dummy> l;
    Dummy dummy;
    l.push_back(Dummy(1));
    l.push_back(dummy);
    l.push_back(Dummy(2));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}