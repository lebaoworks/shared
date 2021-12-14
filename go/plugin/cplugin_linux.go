package plugin

/*
#cgo linux LDFLAGS: -ldl

#include "../../c/plugin/cplugin_linux.c"
#include <stdlib.h>
*/
import "C"

import (
	"errors"
	"path/filepath"
	"unsafe"
)

type PluginStatus uint
const (
    UNLOADED = 0
    LOADED   = 1
)

var (
    cache = map[string]*CPlugin{}
)

type CPlugin struct {
    name    string
    version uint64
    path    string
    handle  unsafe.Pointer
    status  PluginStatus
}



func Get(path string) (p *CPlugin, err error) {
    if path, err = filepath.Abs(path); err != nil {
        return
    }
    if pl, ok := cache[path]; ok {
        return pl, nil
    }
    return nil, errors.New("plugin is not loaded")
}

func Load(path string) (p *CPlugin, err error) {
    if path, err = filepath.Abs(path); err != nil {
        return
    }
    // check cache
    if pl, ok := cache[path]; ok {
        if pl.status == LOADED {
            return pl, nil
        }
    }

    // load
    pl := CPlugin{
        path : path,
    }
    if err = pl.Load(); err != nil {
        return
    }

    // cache
    cache[path] = &pl
    return &pl, nil
}

func (p *CPlugin) Name() string {
    return p.name
}

func (p *CPlugin) Version() uint64 {
    return p.version
}

func (p *CPlugin) Load() (err error) {
    if p.path, err = filepath.Abs(p.path); err != nil {
        return
    }

    // open
    cPath := C.CString(p.path)
    defer C.free(unsafe.Pointer(cPath))
    var cErr *C.char
    handle := C.CPluginOpen((*C.char)(cPath), &cErr)
    if handle == nil {
        err = errors.New(C.GoString(cErr))
        return
    }

    // get info
    p.handle = handle
    p.status = LOADED
    cFuncName := C.CString("GetName")
    defer C.free(unsafe.Pointer(cFuncName))
    p.name = C.GoString(C.CPluginCall(handle, (*C.char)(cFuncName), &cErr))
    if cErr != nil {
        p.Unload()
        return errors.New("can not get plugin name ")
    }
    cFuncName = C.CString("GetVersion")
    defer C.free(unsafe.Pointer(cFuncName))
    p.version = uint64(*(*C.uint64_t) (unsafe.Pointer((C.CPluginCall(handle, (*C.char)(cFuncName), &cErr)))))
    if cErr != nil {
        p.Unload()
        return errors.New("can not get plugin version")
    }
    
    // cache
    cache[p.path] = p
    return nil
}

func (p *CPlugin) Unload() error {
    if (p.status == UNLOADED) {
        return nil
    }

    p.status = UNLOADED
    cPath := C.CString(p.path)
    defer C.free(unsafe.Pointer(cPath))
    var cErr *C.char
    if ret := C.CPluginClose(p.handle, &cErr); ret != 0 {
        return errors.New("got " + string(ret) + " " + C.GoString(cErr))
    }
    return nil
}

func (p *CPlugin) Call(fun string, params ...interface{}) interface{} {
    return nil
}
