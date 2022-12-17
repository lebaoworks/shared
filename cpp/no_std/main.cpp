#include <iostream>
#include "_std.h"

class Dummy
{
public:
    int var = 0;

    Dummy()
    {
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
    ~Dummy()
    {
        printf("%s()\n", __FUNCTION__);
    }
};

int main()
{
    _std::list<Dummy> l;
    Dummy x(5);
    l.push_back(x);
    printf("front: %d", l.front().var);
    return 0;
}