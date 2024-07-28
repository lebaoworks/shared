package storage

import (
	"log"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestDrives(t *testing.T) {
	drives, err := Drives()
	assert.NoError(t, err)

	for _, drive := range drives {
		log.Printf("Drive %v", drive.Number)
		log.Printf("\tVendor: %v", drive.Vendor)
		log.Printf("\tProduct: %v", drive.Product)
		log.Printf("\tSerial: %v]", drive.Serial)
	}
}
