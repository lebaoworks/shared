package collector

type Collector interface {
	Collect() (any, error)
}
