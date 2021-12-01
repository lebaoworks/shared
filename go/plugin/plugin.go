package plugin

import (
    "errors"
    "plugin"
    "reflect"
    "log"
    "fmt"
)

type PluginFunc func(...interface{}) []interface{}

type pluginFuncInfo struct {
    fn       PluginFunc
    rfv      reflect.Value
    rft      reflect.Type
    inTypes  []reflect.Type
    outTypes []reflect.Type
}

type Plugin struct {
    name    string
    version uint64
    path    string
    plugin  *plugin.Plugin
    cache   map[string]*pluginFuncInfo
}

func (p *Plugin) Name() string {
    return p.name
}

func (p *Plugin) Version() uint64 {
    return p.version
}
func (p *Plugin) Call(fun string, params ...interface{}) []interface{} {
    f, err := p.GetFunc(fun)
    if err != nil {
        return []interface{}{err}
    }
    return f(params...)
}

func (p *Plugin) GetFunc(fun string) (f func(...interface{}) []interface{}, err error) {
    if p.plugin == nil {
        err = errors.New("plugin not loaded")
        return
    }
    info, ok := p.cache[fun]
    if ok {
        return info.fn, nil
    }
    f1, err := p.plugin.Lookup(fun)
    if err != nil {
        return nil, err
    }
    info = &pluginFuncInfo{}
    info.rfv = reflect.ValueOf(f1)
    info.rft = reflect.TypeOf(f1)
    li := info.rfv.Type().NumIn()
    lo := info.rfv.Type().NumOut()
    info.inTypes = make([]reflect.Type, li)
    info.outTypes = make([]reflect.Type, lo)
    for i := 0; i < li; i++ {
        info.inTypes[i] = info.rfv.Type().In(i)
    }
    for i := 0; i < lo; i++ {
        info.inTypes[i] = info.rfv.Type().Out(i)
    }
    f = func(params ...interface{}) []interface{} {
        out := make([]interface{}, len(info.outTypes))
        if len(params) != len(info.inTypes) {
            err := errors.New("The number of params is not adapted.")
            out[len(out)-1] = err
            return out
        }
        in := make([]reflect.Value, len(params))
        for k, param := range params {
            in[k] = reflect.ValueOf(param)
            if info.inTypes[k].Name() != in[k].Type().Name() {
                err := fmt.Sprintf("the type of params is not adapted, params[%d] require type %s", k, info.inTypes[k].Name())
                err = fmt.Sprintf("failed to call [%s], %s", info.rft.Name(), err)
                log.Println(err)
                out[len(out)-1] = errors.New(err)
                return out
            }
        }
        result := info.rfv.Call(in)
        for i := 0; i < len(result); i++ {
            out[i] = result[i].Interface()
        }

        return out
    }
    info.fn = f
    p.cache[fun] = info
    return f, nil
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