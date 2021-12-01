package fault

import (
	"errors"
	"fmt"
	"runtime"
)


func CatchFault(err *error) {
	if e := recover(); e == nil {
		*err = nil
		return
	}
	pc, file, line, _ := runtime.Caller(4)
	f := runtime.FuncForPC(pc)
    if f == nil {
        *err = errors.New(fmt.Sprintf("crash in function at (%x)", pc))
    } else {
        *err = errors.New(fmt.Sprintf("crash in %v(), line: %v, file: %v", f.Name(), line, file))
    }
}