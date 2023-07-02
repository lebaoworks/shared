package main

import (
	"fmt"
	"log"
	"sync"
	"time"
)

var (
	responseChan = make(chan *Response, 1000)
)

type Request struct {
	Key string
}
type Response struct {
	Answer string
}
type RequestManger struct {
	requests map[string]*Request
	lock     sync.Mutex
	stop     chan bool
	redis    *Redis
	worker   sync.WaitGroup
}

func NewRequestManager() (*RequestManger, error) {
	obj := &RequestManger{}
	obj.requests = make(map[string]*Request)
	obj.stop = make(chan bool)
	redis, err := NewRedis()
	if err != nil {
		return nil, err
	}
	obj.redis = redis
	obj.worker.Add(1)
	go obj.Job()
	return obj, nil
}

func (obj *RequestManger) Stop() {
	close(obj.stop)
	obj.worker.Wait()
}
func (obj *RequestManger) Job() {
	log.Printf("Job...")
	defer obj.worker.Done()
	for {
		select {
		case <-obj.stop:
			return
		case <-time.After(10 * time.Second):
			obj.checkResponse()
		}
	}
}
func (obj *RequestManger) Add(request *Request) {
	obj.lock.Lock()
	obj.requests[request.Key] = request
	obj.lock.Unlock()
}
func (obj *RequestManger) checkResponse() {
	log.Printf("CheckResponse...")
	keys := []string{}
	obj.lock.Lock()
	for key := range obj.requests {
		keys = append(keys, key)
	}
	obj.lock.Unlock()
	log.Printf("CheckResponse -> %v", keys)

	for _, key := range keys {
		if response, err := obj.redis.Check(key); err == nil {
			obj.lock.Lock()
			delete(obj.requests, key)
			obj.lock.Unlock()
			responseChan <- response
		}
	}
}

func main() {
	log.Printf("Hello")
	manager, err := NewRequestManager()
	if err != nil {
		log.Fatalf("New manager -> %v", err)
	}

	go func() {
		for {
			response := <-responseChan
			log.Printf("got response: %v", response)
		}
	}()

	i := 0
	for {
		i += 1
		manager.Add(&Request{Key: fmt.Sprintf("%v", i)})
		time.Sleep(1 * time.Second)
		log.Printf("Add %v", i)
	}
}
