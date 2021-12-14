// gcc -shared -o plugin.so -fPIC plugin.c

#include <stdint.h>
#include <stdio.h>

char*    pluginName    = "C-Plugin";
uint64_t pluginVersion = 0;

//export GetName
char* GetName() {
    return pluginName;
}

char* GetVersion() {
    return (char*) &pluginVersion;
}

char* Init(uint32_t argc, char** argv)
{
    printf("%d\n", argc);
    for (int i=0; i<argc; i++)
        printf("%s\n", argv[i]);
    return NULL;
}

