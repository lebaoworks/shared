package watchdog

import (
	"log"
	"strings"
	"time"

	"github.com/shirou/gopsutil/v3/process"
)

type WatchDog struct {
	workers  []*Worker
	Interval time.Duration
}

func NewWatchDog(interval time.Duration) *WatchDog {
	return &WatchDog{
		Interval: interval,
	}
}

func (obj *WatchDog) AddWorker(worker *Worker) {
	obj.workers = append(obj.workers, worker)
}

func (obj *WatchDog) Start() {
	// Kill existing workers
	exePathMap := make(map[string]bool)
	for _, worker := range obj.workers {
		exePathMap[strings.ToLower(worker.ImagePath)] = true
	}
	l, e := process.Processes()
	if e == nil {
		for _, p := range l {
			exePath, _ := p.Exe()
			if _, ok := exePathMap[strings.ToLower(exePath)]; ok {
				log.Printf("Kill %v -> error: %v", p.Pid, p.Kill())
			}
		}
	}

	// Start workers
	for _, worker := range obj.workers {
		log.Printf("Spawn worker %v -> error: %v", worker.ImagePath, worker.Spawn())
	}
	go func() {
		for {
			time.Sleep(obj.Interval)
			for _, worker := range obj.workers {
				if worker.IsDead() {
					log.Printf("%v is dead!", worker.ImagePath)
					log.Printf("Respawn %v -> error: %v", worker.ImagePath, worker.Spawn())
				}
			}
		}
	}()
}
