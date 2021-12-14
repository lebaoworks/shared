//LDFLAGS: -ldl
#define _GNU_SOURCE
#include "cplugin_linux.h"

#include <dlfcn.h>
#include <elf.h>
#include <link.h>

#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void* functionLookup(void* h, char* name);

// =================================== //
// ============ERROR HANDLE=========== //
// =================================== //

#define ERROR_SIZE 512
char* MALLOC_FAIL = "malloc memory for error infomation fail";
static char* setError(char* str) {
    int len = strlen(str);
    char* err = malloc(len+1);
    if (err == NULL)
        return MALLOC_FAIL;
    memcpy(err, str, (ERROR_SIZE<len)?ERROR_SIZE:len);
    return err;
}

// =================================== //
// =============IMPLEMENTS============ //
// =================================== //

typedef char* (*PLUGINFUNC) (void);
typedef char* (*PLUGINFUNCARGS) (uint32_t, char**);

void* CPluginOpen(const char* path, char** err) {
    *err = NULL;
    void* h = dlopen(path, RTLD_NOW|RTLD_GLOBAL);
    if (h == NULL) {
        *err = setError(dlerror());
    }
    return h;
}

int CPluginClose(void* h, char** err) {
    *err = NULL;
    int error = dlclose(h);
    if (error) {
        *err = setError(dlerror());
    }
    return error;
}

char* CPluginCall(void* h, char* name, char** err) {
    *err = 0;
    
    // look for symbol
    void* sym = functionLookup(h, name);
    if (sym == NULL) {
        *err = setError("symbol not found / not function");
        return NULL;
    }

    return (*((PLUGINFUNC) sym))();
}

char* CPluginCallArgs(void* h, char* name, uint32_t argc, char** argv, char** err) {
    *err = 0;
    
    // look for symbol
    void* sym = functionLookup(h, name);
    if (sym == NULL) {
        *err = setError("symbol not found / not function");
        return NULL;
    }

    return (*((PLUGINFUNCARGS) sym))(argc, argv);
}

static void* functionLookup(void* h, char* name) {
    // look for symbol
    void* sym = dlsym(h, name);
    if (sym == NULL) {
        return NULL;
    }

    // check if symbol is a function
    ElfW(Sym) *pElfSym;
    Dl_info i;
    if (!dladdr1(sym, &i, (void**) &pElfSym, RTLD_DL_SYMENT)) {
        return NULL;
    }
    switch (ELF32_ST_TYPE(pElfSym->st_info)) {
        case STT_FUNC:
            break;
        default:
            return NULL;
    }
    return sym;
}


// void load_plugin(char* path) {
//     char* err = 0;

//     // Open
//     void* handle = CPluginOpen(path, &err);
//     if (handle == NULL) {
//         printf("ERR: %s\n", err);
//         exit(1);
//     }
//     printf("Loaded at: %p\n", handle);

//     // Init
//     // uint32_t argc = 5;
//     // char* argv[] = {"asd", "zxc", "zxc", "qwe", "ert"};
//     char* name = CPluginCall(handle, "GetName", &err);
//     if (name != 0) {
//         printf("Name: %s\n", name);
//     }
            
//     // Close
//     if (CPluginClose(handle, &err) != 0) {
//         printf("Unload ERR: %s\n", err);
//         exit(1);
//     }
//     printf("Exit\n");
    
// }
// int main()
// {
//     load_plugin("./plugin.so");
//     getc(stdin);
//     load_plugin("./plugin.so");
//     return 0;
// }