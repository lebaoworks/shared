package storage

import (
	"log"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestVolumes(t *testing.T) {
	volumes, err := Volumes()
	assert.NoError(t, err)

	for _, volume := range volumes {
		log.Printf("Volume: %v", volume.GUID)
		for _, mountPoint := range volume.MountPoints {
			log.Printf("\tMountPoint: %v", mountPoint)
		}
		for i, extent := range volume.DiskExtents {
			log.Printf("\tExtent: %v", i)
			log.Printf("\t\tDiskNumber: %v", extent.DiskNumber)
			log.Printf("\t\tStartingOffset: %v", extent.StartingOffset)
			log.Printf("\t\tExtentsLength: %v", extent.ExtentsLength)
		}
	}
}
