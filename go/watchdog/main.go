package main

import (
	"fmt"
	"log"
	"watchdog/process"
)

func main() {
	fmt.Printf("hello")

	l, e := process.ListProcesses()
	if e != nil {
		panic(e)
	}
	for _, p := range l {
		log.Printf("%v", p)
	}
}
