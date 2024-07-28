package storage

import (
	"fmt"
	"unsafe"

	"golang.org/x/sys/windows"
	"golang.org/x/sys/windows/registry"
)

type Drive struct {
	Number uint64

	Removable bool
	Vendor    string
	Product   string
	Serial    string
}

const (
	IOCTL_STORAGE_QUERY_PROPERTY = 0x2D1400
	PropertyStandardQuery        = 0
	StorageDeviceProperty        = 0
)

type STORAGE_PROPERTY_QUERY struct {
	PropertyId uint32
	QueryType  uint32
	Additional [1]byte
}

type STORAGE_DEVICE_DESCRIPTOR struct {
	Version               uint32
	Size                  uint32
	DeviceType            byte
	DeviceTypeModifier    byte
	RemovableMedia        byte
	CommandQueueing       byte
	VendorIdOffset        uint32
	ProductIdOffset       uint32
	ProductRevisionOffset uint32
	SerialNumberOffset    uint32
	BusType               byte
	RawPropertiesLength   uint32
	RawDeviceProperties   [1]byte
}

func getDriveInfo(devicePath string) (*Drive, error) {
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

	// Get Device Descriptor
	query := STORAGE_PROPERTY_QUERY{
		PropertyId: StorageDeviceProperty,
		QueryType:  PropertyStandardQuery,
	}
	var returned uint32
	buffer := make([]byte, 512)
	err = windows.DeviceIoControl(
		h,
		IOCTL_STORAGE_QUERY_PROPERTY,
		(*byte)(unsafe.Pointer(&query)),
		uint32(unsafe.Sizeof(query)),
		&buffer[0],
		uint32(len(buffer)),
		&returned,
		nil,
	)
	if err != nil {
		return nil, err
	}
	descriptor := (*STORAGE_DEVICE_DESCRIPTOR)(unsafe.Pointer(&buffer[0]))

	ret := &Drive{}
	ret.Removable = descriptor.RemovableMedia == 1
	if descriptor.VendorIdOffset != 0 {
		ret.Vendor = windows.BytePtrToString((*byte)(unsafe.Pointer(&buffer[descriptor.VendorIdOffset])))
	}
	if descriptor.ProductIdOffset != 0 {
		ret.Product = windows.BytePtrToString((*byte)(unsafe.Pointer(&buffer[descriptor.ProductIdOffset])))
	}
	if descriptor.SerialNumberOffset != 0 {
		ret.Serial = windows.BytePtrToString((*byte)(unsafe.Pointer(&buffer[descriptor.SerialNumberOffset])))
	}
	return ret, nil
}

func Drives() ([]*Drive, error) {
	key, err := registry.OpenKey(registry.LOCAL_MACHINE, `SYSTEM\CurrentControlSet\Services\Disk\Enum`, registry.QUERY_VALUE)
	if err != nil {
		return nil, err
	}
	defer key.Close()

	count, _, err := key.GetIntegerValue("Count")
	if err != nil {
		return nil, err
	}

	ret := []*Drive{}
	for i := uint64(0); i < count; i++ {
		devicePath := fmt.Sprintf(`\\.\PhysicalDrive%v`, i)
		info, err := getDriveInfo(devicePath)
		if err != nil {
			// TODO: log warning here
			continue
		}
		info.Number = i
		ret = append(ret, info)
	}
	return ret, nil
}
