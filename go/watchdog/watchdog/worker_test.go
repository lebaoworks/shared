package watchdog

import (
	"log"
	"testing"
)

func TestWorkerSpawn(t *testing.T) {
	//path := "C:\\Users\\bao\\Downloads\\UltraViewer_setup_6.6_vi.exe"
	path := "D:\\Desktop\\watchdog\\cmd.exe"
	worker := &Worker{ImagePath: path}
	log.Printf("Spawn -> %v", worker.Spawn())
	log.Printf("IsDead -> %v", worker.IsDead())
}
