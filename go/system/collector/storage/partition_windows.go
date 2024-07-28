package storage

import (
	"fmt"
	"reflect"
	"unsafe"

	"golang.org/x/sys/windows"
	"golang.org/x/sys/windows/registry"
)

// Partition

type Partition struct {
	DiskNumber uint64

	PARTITION_INFORMATION_EX
}

const (
	IOCTL_DISK_GET_DRIVE_LAYOUT_EX = 0x00070050
)

type DRIVE_LAYOUT_INFORMATION_EX struct {
	PartitionStyle uint32
	PartitionCount uint32
	LayoutUnion    [40]byte // DRIVE_LAYOUT_INFORMATION_MBR or DRIVE_LAYOUT_INFORMATION_GPT
	PartitionEntry [1]PARTITION_INFORMATION_EX
}

type PARTITION_INFORMATION_EX struct {
	PartitionStyle     uint32
	StartingOffset     int64
	PartitionLength    int64
	PartitionNumber    uint32
	RewritePartition   uint8
	IsServicePartition uint8
	Reserved           [2]uint8
	PartitionInfoUnion [112]byte // PARTITION_INFORMATION_MBR or PARTITION_INFORMATION_GPT
}

func getPartitionsInfoEx(devicePath string) ([]*PARTITION_INFORMATION_EX, error) {
	// Open device
	path, err := windows.UTF16PtrFromString(devicePath)
	if err != nil {
		return nil, err
	}
	h, err := windows.CreateFile(
		path,
		windows.GENERIC_READ|windows.GENERIC_WRITE,
		0,
		nil,
		windows.OPEN_EXISTING,
		0,
		0,
	)
	if err != nil {
		return nil, err
	}
	defer windows.CloseHandle(h)

	// Get drive layout information
	var returned uint32
	buffer := make([]byte, 4096)
	err = windows.DeviceIoControl(
		h,
		IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
		nil,
		0,
		&buffer[0],
		uint32(len(buffer)),
		&returned,
		nil,
	)
	if err != nil {
		return nil, err
	}
	driveLayout := (*DRIVE_LAYOUT_INFORMATION_EX)(unsafe.Pointer(&buffer[0]))

	if driveLayout.PartitionCount == 0 {
		return []*PARTITION_INFORMATION_EX{}, nil
	}

	// Parse partition data
	partitionInfos := []PARTITION_INFORMATION_EX{}
	sliceHeader := (*reflect.SliceHeader)((unsafe.Pointer(&partitionInfos)))
	sliceHeader.Cap = int(driveLayout.PartitionCount)
	sliceHeader.Len = int(driveLayout.PartitionCount)
	sliceHeader.Data = uintptr(unsafe.Pointer(&driveLayout.PartitionEntry[0]))

	ret := []*PARTITION_INFORMATION_EX{}
	for _, info := range partitionInfos {
		e := &PARTITION_INFORMATION_EX{}
		*e = info
		ret = append(ret, e)
	}
	return ret, nil
}

func Partitions() ([]*Partition, error) {
	key, err := registry.OpenKey(registry.LOCAL_MACHINE, `SYSTEM\CurrentControlSet\Services\Disk\Enum`, registry.QUERY_VALUE)
	if err != nil {
		return nil, err
	}
	defer key.Close()

	count, _, err := key.GetIntegerValue("Count")
	if err != nil {
		return nil, err
	}

	ret := []*Partition{}
	for i := uint64(0); i < count; i++ {
		devicePath := fmt.Sprintf(`\\.\PhysicalDrive%v`, i)
		infos, err := getPartitionsInfoEx(devicePath)
		if err != nil {
			// TODO: log warning here
			continue
		}
		for _, info := range infos {
			p := &Partition{}
			p.PARTITION_INFORMATION_EX = *info
			p.DiskNumber = i
			ret = append(ret, p)
		}
	}
	return ret, nil
}
