package process

import (
	"github.com/shirou/gopsutil/v3/process"
)

const processEntrySize = 568 // unsafe.Sizeof(windows.ProcessEntry32{})

type Process struct {
	Pid     uint32
	ExePath string
}

func ListProcesses() ([]*Process, error) {

	l, e := process.Processes()
	if e != nil {
		return nil, e
	}
	var ret []*Process
	for _, p := range l {
		exePath, _ := p.Exe()
		ret = append(ret, &Process{
			ExePath: exePath,
			Pid:     uint32(p.Pid),
		})
	}
	return ret, nil
}

// func ListProcesses2() ([]*Process, error) {
// 	h, e := windows.CreateToolhelp32Snapshot(windows.TH32CS_SNAPPROCESS, 0)
// 	if e != nil {
// 		return nil, e
// 	}
// 	defer windows.CloseHandle(h)

// 	var ret []*Process
// 	p := windows.ProcessEntry32{Size: processEntrySize}
// 	for {
// 		e := windows.Process32Next(h, &p)
// 		if e != nil {
// 			break
// 		}
// 		ret = append(ret, &Process{
// 			ExePath: windows.UTF16ToString(p.ExeFile[:]),
// 			Pid:     p.ProcessID,
// 		})
// 	}
// 	return ret, nil
// }
