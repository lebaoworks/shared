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

#define ERROR_SIZE 512

char* MALLOC_FAIL = "malloc memory for error infomation fail";

char* setError(char* str) {
    int len = strlen(str);
    char* err = malloc(len+1);
    if (err == NULL)
        return MALLOC_FAIL;
    memcpy(err, str, (ERROR_SIZE<len)?ERROR_SIZE:len);
    return err;
}

static void* PluginOpen(const char* path, char** err) {
    *err = NULL;
    void* h = dlopen(path, RTLD_NOW|RTLD_GLOBAL);
    if (h == NULL) {
        *err = setError(dlerror());
    }
    return h;
}

static int PluginClose(void* h, char** err) {
    *err = NULL;
    int error = dlclose(h);
    if (error) {
        *err = setError(dlerror());
    }
    return error;
}

static char* PluginCall(void* h, char* name, uint32_t argc, char** argv, char** err) {
    *err = 0;

    // look for symbol
    void* sym = dlsym(h, name);
    if (*err != NULL) {
        *err = setError(dlerror());
        return NULL;
    }

    // check if symbol is a function
    ElfW(Sym) *pElfSym;
    Dl_info i;
    if (!dladdr1(sym, &i, (void **)&pElfSym, RTLD_DL_SYMENT)) {
        *err = setError("can not get symbol informations");
        return NULL;
    }
    switch (ELF32_ST_TYPE(pElfSym->st_info)) {
        case STT_FUNC:
            break;
        default:
            *err = setError("symbol is not a function");
            return NULL;
    }

    return (*((PLUGINFUNC) sym))(argc, argv);
}


// void load_plugin(char* path) {
//     char* err = 0;

//     // Open
//     void* handle = PluginOpen(path, &err);
//     if (handle == NULL) {
//         printf("ERR: %s\n", err);
//         exit(1);
//     }
//     printf("Loaded at: %p\n", handle);

//     // Init
//     uint32_t argc = 5;
//     char* argv[] = {"asd", "zxc", "zxc", "qwe", "ert"};
//     if (PluginCall(handle, "Init", argc, argv, &err) != 0) {
//         printf("Init ERR: %s\n", err);
//         exit(1);
//     }
            
//     // Close
//     if (PluginClose(handle, &err) != 0) {
//         printf("Unload ERR: %s\n", err);
//         exit(1);
//     }
    
// }
// int main()
// {
//     load_plugin("./sample.so");
//     getc(stdin);
//     load_plugin("./sample.so");
//     return 0;
// }