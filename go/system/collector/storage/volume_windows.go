package storage

import (
	"fmt"
	"reflect"
	"regexp"
	"syscall"
	"unsafe"

	"golang.org/x/sys/windows"
)

type Volume struct {
	GUID        string
	MountPoints []string
	DiskExtents []*DISK_EXTENT
}

const IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS = 0x00560000

type DISK_EXTENT struct {
	DiskNumber     uint32
	StartingOffset int64
	ExtentsLength  int64
}

type VOLUME_DISK_EXTENTS struct {
	NumberOfDiskExtents uint32
	Extents             [1]DISK_EXTENT
}

func getVolumeMountPoints(guid string) ([]string, error) {
	guidPath := fmt.Sprintf(`\\?\Volume{%v}\`, guid)
	volumeName := windows.StringToUTF16Ptr(guidPath)

	buffer := make([]uint16, 4096)
	var returned uint32
	err := windows.GetVolumePathNamesForVolumeName(
		volumeName,
		&buffer[0],
		uint32(len(buffer)),
		&returned)
	if err != nil {
		return nil, fmt.Errorf("GetVolumePathNamesForVolumeName error: %v", err)
	}

	ret := []string{}
	for i := 0; i < int(returned); {
		mountPoint := windows.UTF16PtrToString(&buffer[i])
		if len(mountPoint) == 0 {
			break
		}
		ret = append(ret, mountPoint)
		i += len(mountPoint) + 1
	}
	return ret, nil
}

func getVolumeDiskExtents(guid string) ([]*DISK_EXTENT, error) {
	guidPath := fmt.Sprintf(`\\?\Volume{%v}`, guid)
	handle, err := windows.CreateFile(
		windows.StringToUTF16Ptr(guidPath),
		windows.GENERIC_READ,
		windows.FILE_SHARE_READ|windows.FILE_SHARE_WRITE,
		nil,
		windows.OPEN_EXISTING,
		0,
		0,
	)
	if err != nil {
		return nil, fmt.Errorf("CreateFile error: %v", err)
	}
	defer windows.CloseHandle(handle)

	var returned uint32
	buffer := make([]byte, 4096)
	err = windows.DeviceIoControl(
		handle,
		IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
		nil,
		0,
		&buffer[0],
		uint32(len(buffer)),
		&returned,
		nil,
	)
	if err != nil {
		return nil, fmt.Errorf("DeviceIoControl error: %v", err)
	}
	volumeDiskExtents := (*VOLUME_DISK_EXTENTS)(unsafe.Pointer(&buffer[0]))

	if volumeDiskExtents.NumberOfDiskExtents == 0 {
		return []*DISK_EXTENT{}, nil
	}

	extents := []DISK_EXTENT{}
	sliceHeader := (*reflect.SliceHeader)((unsafe.Pointer(&extents)))
	sliceHeader.Cap = int(volumeDiskExtents.NumberOfDiskExtents)
	sliceHeader.Len = int(volumeDiskExtents.NumberOfDiskExtents)
	sliceHeader.Data = uintptr(unsafe.Pointer(&volumeDiskExtents.Extents[0]))

	ret := []*DISK_EXTENT{}
	for _, extent := range extents {
		e := &DISK_EXTENT{}
		*e = extent
		ret = append(ret, e)
	}

	return ret, nil
}

func Volumes() ([]*Volume, error) {
	buffer := make([]uint16, syscall.MAX_PATH)
	handle, err := windows.FindFirstVolume(&buffer[0], uint32(len(buffer)))
	if err != nil {
		return nil, fmt.Errorf("FindFirstVolume error: %v", err)
	}
	defer windows.FindVolumeClose(handle)

	ret := []*Volume{}
	rxGUID := regexp.MustCompile(`{(.*)}`)
	for {
		// Extract GUID
		volumeGUIDPath := windows.UTF16PtrToString(&buffer[0])
		foundGUID := rxGUID.FindStringSubmatch(volumeGUIDPath)
		if len(foundGUID) == 0 {
			return nil, fmt.Errorf("got invalid guid")
		}
		volumeGUID := foundGUID[1]

		p := &Volume{
			GUID: volumeGUID,
		}
		ret = append(ret, p)

		// Get MountPoints
		mountPoints, err := getVolumeMountPoints(volumeGUID)
		if err == nil {
			p.MountPoints = mountPoints
		}

		// Get Extents
		extents, err := getVolumeDiskExtents(volumeGUID)
		if err == nil {
			p.DiskExtents = extents
		}

		if err := windows.FindNextVolume(handle, &buffer[0], uint32(len(buffer))); err != nil {
			if err == windows.ERROR_NO_MORE_FILES {
				break
			}
			return nil, fmt.Errorf("FindNextVolume error: %v", err)
		}
	}

	return ret, nil
}
