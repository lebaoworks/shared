// gcc -shared -o sample_plugin.so -fPIC sample_plugin.c

#include <stdint.h>
#include <stdio.h>

int Init(uint32_t argc, void** argv, char** err)
{
    printf("Plugin version: 20\n");
    return 0;
}

