//LDFLAGS: -ldl
#include <dlfcn.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>

#include <stdio.h>

typedef int (*PLUGININIT) (uint32_t, void**, char**);

static void* PluginOpen(const char* path, char** err) {
    void* h = dlopen(path, RTLD_NOW|RTLD_GLOBAL);
    if (h == NULL) {
        *err = (char*)dlerror();
    }
    return h;
}

static void* PluginLookup(void* h, char* name, char** err) {
    void* r = dlsym(h, name);
    if (r == NULL) {
        *err = (char*)dlerror();
    }
    return r;
}

static int PluginClose(void* h, char** err) {
    int error = dlclose(h);
    if (error) {
        *err = (char*) dlerror();
    }
    return error;
}

static int PluginInit(void* h, uint32_t argc, void** argv, char** err) {
    *err = 0;
    void* pPluginInit = PluginLookup(h, "Init", err);
    if (*err != NULL) {
        return 1;
    }
    printf("Init at: %p\n", pPluginInit);
    return (*((PLUGININIT) pPluginInit))(argc, argv, err);
}

void load_plugin(char* path) {
    char* err = 0;

    // Open
    void* handle = PluginOpen(path, &err);
    if (handle == NULL) {
        printf("ERR: %s\n", err);
        exit(1);
    }
    printf("Loaded at: %p\n", handle);

    // Init
    if (PluginInit(handle, 0, NULL, &err) != 0) {
        printf("Init ERR: %s\n", err);
        exit(1);
    }
    
    // Close
    if (PluginClose(handle, &err) != 0) {
        printf("Unload ERR: %s\n", err);
        exit(1);
    }
    
}
int main()
{
    load_plugin("./sample_plugin.so");
    getc(stdin);
    load_plugin("./sample_plugin.so");
    return 0;
}