package storage

import (
	"log"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestPartitions(t *testing.T) {
	partitions, err := Partitions()
	assert.NoError(t, err)

	for i, partition := range partitions {
		log.Printf("Partition %v", i)
		log.Printf("\tDiskNumber: %v", partition.DiskNumber)
		log.Printf("\tNumber: %v", partition.PartitionNumber)
		log.Printf("\tStyle: %v", partition.PartitionStyle)
		log.Printf("\tOffset: %v", partition.StartingOffset)
		log.Printf("\tLength: %v", partition.PartitionLength)
	}
}
