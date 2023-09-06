package watchdog

import (
	"testing"
	"time"
)

func TestWatchDog(t *testing.T) {
	//path := "C:\\Users\\bao\\Downloads\\UltraViewer_setup_6.6_vi.exe"
	dog := NewWatchDog(5 * time.Second)

	path := "D:\\Desktop\\watchdog\\cmd.exe"
	worker := &Worker{ImagePath: path}
	dog.AddWorker(worker)

	dog.Start()
	select {}
}
