#include <iostream>
#include <iomanip>
#include <vector>

#include "collections.h"
#include "memory.h"

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

Dummy make()
{
    Dummy x(5);
    return x;
}

int main()
{
    cpp::List<Dummy> l;
    Dummy x(2);
    //l.push_back(move(x));
    l.push_back(x);
    printf("x.var = %d\n", x.var);

    
    return 0;
}