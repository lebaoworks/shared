package plugin

import (
	"errors"
	"fmt"
	"log"
	"plugin"
	"reflect"
	"unsafe"
)

type CPluginFunc unsafe.Pointer
type CPlugin struct {
    name    string
    version uint64
    path    string
    plugin  *plugin.Plugin
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