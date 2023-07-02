package main

type Redis struct {
}

func NewRedis() (*Redis, error) {
	obj := &Redis{}
	return obj, nil
}

func (obj *Redis) Check(key string) (*Response, error) {
	return &Response{Answer: "OK"}, nil
}
