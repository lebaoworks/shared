#include "nstd.hpp"

#include <list>

#include <Windows.h>
#include <winternl.h>
#pragma comment(lib, "ntdll.lib")

std::wstring GetVolumeNameFromMountPoint(const std::wstring mount_point)
{
    auto path = (mount_point.back() == L'\\') ? mount_point : mount_point + L'\\';
    auto buffer = std::make_unique<WCHAR[]>(4096);
    if (GetVolumeNameForVolumeMountPointW(path.c_str(), buffer.get(), 4096) == FALSE)
        throw nstd::runtime_error("api return error %d", GetLastError());
    return buffer.get();
}

/// @brief Get mount points
/// @param volume GUID path in the form "\\\\?\\Volume{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\\".
std::list<std::wstring> GetMountPointsForVolumeName(const std::wstring volume)
{
    DWORD needed = 0;
    if (GetVolumePathNamesForVolumeNameW(volume.c_str(), NULL, 0, &needed) == TRUE)
        throw nstd::runtime_error("api return unexpected value");

    auto err = GetLastError();
    if (err != ERROR_MORE_DATA)
        throw nstd::runtime_error("api return error: %d", err);

    auto buffer = std::make_unique<WCHAR[]>(needed);
    if (GetVolumePathNamesForVolumeNameW(volume.c_str(), buffer.get(), needed, &needed) == FALSE)
        throw nstd::runtime_error("api return error: %d", GetLastError());

    std::list<std::wstring> ret;
    WCHAR* path = buffer.get();
    while (*path != L'\0')
    {
        ret.push_back(path);
        path += wcslen(path) + 1;
    }
    return ret;
}

int _main()
{
    std::wstring mount_path = LR"(C:\D\)";

    auto volume = GetVolumeNameFromMountPoint(mount_path);
    printf("Volume -> %ws\n", volume.c_str());

    auto mount_points = GetMountPointsForVolumeName(volume);
    for (auto& mount_point : mount_points)
        printf("Mountpoint -> %ws\n", mount_point.c_str());

    return 0;
}


typedef struct _REPARSE_DATA_BUFFER {
    ULONG  ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    union {
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            ULONG  Flags;
            WCHAR  PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR  PathBuffer[1];
        } MountPointReparseBuffer;
        struct {
            UCHAR DataBuffer[1];
        } GenericReparseBuffer;
    } DUMMYUNIONNAME;
} REPARSE_DATA_BUFFER, * PREPARSE_DATA_BUFFER;

std::wstring GetMountedVolume(const std::wstring& nt_path)
{
    UNICODE_STRING unicodeFilePath;
    RtlInitUnicodeString(&unicodeFilePath, nt_path.c_str());

    OBJECT_ATTRIBUTES objectAttributes;
    InitializeObjectAttributes(&objectAttributes, &unicodeFilePath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK ioStatusBlock;
    HANDLE hFile;

    NTSTATUS status = NtCreateFile(
        &hFile,
        FILE_READ_ATTRIBUTES | SYNCHRONIZE,
        &objectAttributes,
        &ioStatusBlock,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        FILE_OPEN,
        FILE_OPEN_FOR_BACKUP_INTENT | FILE_OPEN_REPARSE_POINT | FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0);
    if (status != 0)
        throw nstd::runtime_error("nt api return error: %X", status);
    defer{ NtClose(hFile); };

    // Get reparse point
    auto buffer = std::make_unique<BYTE[]>(4096);
    DWORD bytesReturned = 0;
    auto result = DeviceIoControl(
        hFile,
        FSCTL_GET_REPARSE_POINT,
        NULL,
        0,
        buffer.get(),
        4096,
        &bytesReturned,
        NULL);
    if (result == FALSE)
        throw nstd::runtime_error("device io error: %d", GetLastError());

    // FSCTL_GET_REPARSE_POINT may return either REPARSE_DATA_BUFFER or REPARSE_GUID_DATA_BUFFER
    // Reference: https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/fsctl-get-reparse-point
    // At this point, we just parse what are known, Microsoft defined tags

    REPARSE_DATA_BUFFER* reparseData = reinterpret_cast<REPARSE_DATA_BUFFER*>(buffer.get());
    switch (reparseData->ReparseTag)
    {
        /*case IO_REPARSE_TAG_SYMLINK:
        {
            wchar_t* target = (wchar_t*)((BYTE*)reparseData->SymbolicLinkReparseBuffer.PathBuffer + reparseData->SymbolicLinkReparseBuffer.PrintNameOffset);
            target[reparseData->SymbolicLinkReparseBuffer.PrintNameLength / sizeof(wchar_t)] = L'\0';
            printf("-> Symlink: %ws\n", target);
            break;
        }*/
    case IO_REPARSE_TAG_MOUNT_POINT:
    {
        wchar_t* target = (wchar_t*)((BYTE*)reparseData->MountPointReparseBuffer.PathBuffer + reparseData->MountPointReparseBuffer.PrintNameOffset);
        target[reparseData->MountPointReparseBuffer.PrintNameLength / sizeof(wchar_t)] = L'\0';
        return target;
    }
    default:
        throw nstd::runtime_error("unknown tag: %X", reparseData->ReparseTag);
    }
}

#define MOUNTDEVCONTROLTYPE                         0x0000004D // 'M'
#define IOCTL_MOUNTDEV_QUERY_DEVICE_NAME    CTL_CODE(MOUNTDEVCONTROLTYPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _MOUNTDEV_NAME {
    USHORT NameLength;
    WCHAR  Name[1];
} MOUNTDEV_NAME, * PMOUNTDEV_NAME;

std::wstring GetVolumeDevice(const std::wstring& volume)
{
    auto path = (volume.back() == L'\\') ? volume.substr(0, volume.size() -1) : volume;
    
    // Get Device
    UNICODE_STRING unicodeFilePath;
    RtlInitUnicodeString(&unicodeFilePath, path.c_str());

    OBJECT_ATTRIBUTES objectAttributes;
    InitializeObjectAttributes(&objectAttributes, &unicodeFilePath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK ioStatusBlock;
    HANDLE hFile;

    NTSTATUS status = NtOpenFile(
        &hFile,
        FILE_READ_ATTRIBUTES | SYNCHRONIZE,
        &objectAttributes,
        &ioStatusBlock,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        FILE_SYNCHRONOUS_IO_ALERT);
    if (status != 0)
        throw nstd::runtime_error("nt api return error: %X", status);
    defer {NtClose(hFile);};
    
    // Get reparse point
    auto buffer = std::make_unique<BYTE[]>(4096);
    DWORD bytesReturned = 0;
    auto result = DeviceIoControl(
        hFile,
        IOCTL_MOUNTDEV_QUERY_DEVICE_NAME,
        NULL,
        0,
        buffer.get(),
        4096,
        &bytesReturned,
        NULL);
    if (result == FALSE)
        throw nstd::runtime_error("device io error: %d", GetLastError());

    PMOUNTDEV_NAME mountDevName = (PMOUNTDEV_NAME) buffer.get();
    return std::wstring(mountDevName->Name, wcsnlen(mountDevName->Name, mountDevName->NameLength / 2));    

    return L"";
}

int _nt_main()
{
    std::wstring mount_path = LR"(C:\D)";

    // Get mounted volume
    auto nt_mount_path = L"\\??\\" + mount_path;
    auto volume = GetMountedVolume(nt_mount_path);
    printf("Volume -> %ws\n", volume.c_str());

    auto device = GetVolumeDevice(volume);
    printf("Device -> %ws\n", device.c_str());
    return 0;
}

int main()
{
    try { return _main(); }
    catch (std::exception& e) { printf("[!] Got exception: %s", e.what()); return ERROR_UNHANDLED_EXCEPTION; }
}