package plugin

/*
#cgo linux LDFLAGS: -ldl
#include <dlfcn.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>

#include <stdio.h>

static uintptr_t pluginOpen(const char* path, char** err) {
	void* h = dlopen(path, RTLD_NOW|RTLD_GLOBAL);
	if (h == NULL) {
		*err = (char*)dlerror();
	}
	return (uintptr_t)h;
}

static void* pluginLookup(uintptr_t h, const char* name, char** err) {
	void* r = dlsym((void*)h, name);
	if (r == NULL) {
		*err = (char*)dlerror();
	}
	return r;
}
*/
import "C"

import (
	"unsafe"
    "plugin"
)

type CPluginFunc unsafe.Pointer
type CPlugin struct {
    name    string
    version uint64
    path    string
    handle  C.uintptr_t
    cache   map[string]CPluginFunc
}

func (p *CPlugin) Name() string {
    return p.name
}

func (p *CPlugin) Version() uint64 {
    return p.version
}
func (p *CPlugin) Call(fun string, params ...interface{}) []interface{} {
    f, err := p.GetFunc(fun)
    if err != nil {
        return []interface{}{err}
    }
    return f(params...)
}

func Load(path string) (p *Plugin, err error) {
    _p, e := plugin.Open(path)
    if e != nil {
        err = e
        return
    }
    nameSymb, e := _p.Lookup("Name")
    if e != nil {
        err = e
        return
    }

    verSymb, e := _p.Lookup("Version")
    if e != nil {
        err = e
        return
    }

    p.plugin = _p
    p.name = nameSymb.(string)
    p.version = verSymb.(uint64)
    return
}