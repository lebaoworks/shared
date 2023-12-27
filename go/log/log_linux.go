package log

import (
	"io"
	"log/syslog"
)

func systemWriter() (io.Writer, error) {
	return syslog.New(syslog.LOG_SYSLOG, "")
}
