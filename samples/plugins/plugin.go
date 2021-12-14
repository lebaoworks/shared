package main

/*
#include <stdint.h>
*/
import "C"
import (
    "fmt"
    "unsafe"
)

var (
    Name string = "Go-Plugin"
    Version uint64 = 0
    pluginName = C.CString(Name)
    pluginVersion = C.uint64_t(Version)
)


//export GetName
func GetName() *C.char {
    return pluginName
}

//export GetVersion
func GetVersion() *C.char {
    return (*C.char) (unsafe.Pointer(&pluginVersion))
}

//export Init
func Init(argc C.uint32_t, argv **C.char) *C.char {
    fmt.Println(int(argc))
    length := int(argc)
    tmpslice := (*[1 << 30]*C.char)(unsafe.Pointer(argv))[:length:length]
    for _, s := range tmpslice {
        fmt.Println(C.GoString(s))
    }
    return nil
}


func main() {}