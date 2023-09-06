package watchdog

import (
	"log"
	"os"

	"github.com/shirou/gopsutil/v3/process"
)

type Worker struct {
	ImagePath string
	process   *process.Process
}

func (obj *Worker) IsDead() bool {
	if obj.process == nil {
		return true
	}
	ret, err := obj.process.IsRunning()
	if err != nil {
		log.Printf("Unexpected: %v", err)
	}
	return ret == false
}

func (obj *Worker) Spawn() error {
	procAttr := new(os.ProcAttr)
	procAttr.Files = []*os.File{os.Stdin, os.Stdout, os.Stderr}
	p, err := os.StartProcess(obj.ImagePath, []string{obj.ImagePath}, procAttr)
	if err == nil {
		log.Printf("New pid -> %v", p.Pid)
		obj.process = &process.Process{
			Pid: int32(p.Pid),
		}
	} else {
		obj.process = nil
	}
	return err
}
