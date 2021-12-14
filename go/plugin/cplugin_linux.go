package cplugin

/*
#cgo linux LDFLAGS: -ldl
#include "../../c/plugin/cplugin_linux.h"
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


func Load(path string) (p *CPlugin, err error) {
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

func (p *CPlugin) Call(fun string, params ...interface{}) []interface{} {
    f, err := p.getFunc(fun)
    if err != nil {
        return []interface{}{err}
    }
    return f(params...)
}
