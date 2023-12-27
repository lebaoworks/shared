package log

import (
	"fmt"
	"io"
	"os"
	"path/filepath"
	"runtime"
	"strconv"
	"strings"
	"sync"
	"time"
	"unsafe"
)

const (
	ErrorLevel   = 1
	WarningLevel = 2
	InfoLevel    = 3
	DebugLevel   = 4
	TraceLevel   = 5
	AllLevel     = 6
)

var (
	level = InfoLevel
	out   = io.MultiWriter(os.Stdout)
	lock  sync.Mutex
)

func SetLevel(newLevel int) (oldLevel int) {
	oldLevel = level
	level = newLevel
	return
}

func AddWriter(writer io.Writer) {
	out = io.MultiWriter(out, writer)
}

func Fatal(format string, args ...any) {
	output("[FATAL]", fmt.Sprintf(format, args...))
	os.Exit(1)
}

func Error(format string, args ...any) {
	if level >= ErrorLevel {
		output("[ERROR]", fmt.Sprintf(format, args...))
	}
}

func Warning(format string, args ...any) {
	if level >= WarningLevel {
		output("[WARNING]", fmt.Sprintf(format, args...))
	}
}

func Info(format string, args ...any) {
	if level >= InfoLevel {
		output("[INFO]", fmt.Sprintf(format, args...))
	}
}

func Debug(format string, args ...any) {
	if level >= DebugLevel {
		output("[DEBUG]", fmt.Sprintf(format, args...))
	}
}

func Trace(format string, args ...any) {
	if level >= TraceLevel {
		output("[TRACE]", fmt.Sprintf(format, args...))
	}
}

func write(s string) {
	b := unsafe.Slice(unsafe.StringData(s), len(s))
	out.Write(b)
}

func output(level string, content string) {
	t := time.Now()
	tstr := t.Format("2006/01/02 15:04:05.1234")
	pc, file, line, _ := runtime.Caller(2)
	fileName := filepath.Base(file)
	packageName := func() string {
		funcName := runtime.FuncForPC(pc).Name()
		lastDot := strings.LastIndexByte(funcName, '.')
		return funcName[:lastDot]
	}()

	lock.Lock()
	defer lock.Unlock()
	write(tstr)
	write(" ")
	write(level)
	write(" ")
	write(packageName)
	write("/")
	write(fileName)
	write(":")
	write(strconv.Itoa(line))
	write(" ")
	write(content)
	write("\n")
}
