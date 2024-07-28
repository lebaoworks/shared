package storage

type Data struct {
	Drives     []*Drive
	Partitions []*Partition
	Volumes    []*Volume
}

type Collector struct{}

func (obj *Collector) Collect() (any, error) {
	ret := &Data{}

	if drives, err := Drives(); err == nil {
		ret.Drives = drives
	}
	if partitions, err := Partitions(); err == nil {
		ret.Partitions = partitions
	}
	if volumes, err := Volumes(); err == nil {
		ret.Volumes = volumes
	}

	return ret, nil
}
