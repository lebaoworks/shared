// Hook.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*
* Windows
*/
#include <Windows.h>

DWORD (*old_GetCurrentProcessId)() = GetCurrentProcessId;

/*
* C++
*/
#include <iostream>
using namespace std;

class SelfHook {
public:
    SelfHook()
    {
        this->handle = GetCurrentProcess();
        this->ParseProcess();
    }
    bool Hook(void* old_func, void* new_function)
    {
        IMAGE_IMPORT_DESCRIPTOR* import_directory_table = (IMAGE_IMPORT_DESCRIPTOR*)this->RVA2VA(this->nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
        for (IMAGE_IMPORT_DESCRIPTOR* import_descriptor = import_directory_table; import_descriptor->Characteristics != NULL; import_descriptor++)
            if (import_descriptor->FirstThunk != 0)
            {
                IMAGE_THUNK_DATA* ILT_entry = (PIMAGE_THUNK_DATA)(this->RVA2VA(import_descriptor->OriginalFirstThunk));
                IMAGE_THUNK_DATA* IAT_entry = (PIMAGE_THUNK_DATA)(this->RVA2VA(import_descriptor->FirstThunk));
                for (; ILT_entry->u1.AddressOfData; ILT_entry++, IAT_entry++)
                    if (IAT_entry->u1.Function == (uintptr_t)old_func)
                    {
                        DWORD dwOldProtect = NULL;
                        VirtualProtect(&IAT_entry->u1.Function, sizeof(IAT_entry->u1.Function), PAGE_READWRITE, &dwOldProtect);
                        IAT_entry->u1.Function = (uintptr_t)new_function;
                        VirtualProtect(&IAT_entry->u1.Function, sizeof(IAT_entry->u1.Function), dwOldProtect, &dwOldProtect);
                        return true;
                    }
            }
        return false;
    }

private:
    HANDLE handle = NULL;
    IMAGE_DOS_HEADER* dos_header;
    IMAGE_NT_HEADERS* nt_header;

    void* RVA2VA(DWORD rva) {return (void*)((uintptr_t)this->dos_header + rva);}
    DWORD VA2RVA(void* va) {return (DWORD)((uintptr_t)va - (uintptr_t)this->dos_header);}

    DWORD ParseProcess()
    {
        HMODULE base_module_handle = GetModuleHandleA(NULL);    // never returns NULL, module's handle is always the base address of the module
        this->dos_header = (IMAGE_DOS_HEADER*)base_module_handle;
        this->nt_header = (IMAGE_NT_HEADERS*)((uintptr_t)this->dos_header + this->dos_header->e_lfanew);
        return 0;
    }
};

DWORD hello()
{
    printf("%s\n", __FUNCTION__);
    return old_GetCurrentProcessId() - 1;
}

int main()
{
    printf("Old func -> %u\n", GetCurrentProcessId());

    SelfHook self_hook = SelfHook();
    self_hook.Hook(GetCurrentProcessId, hello);
    printf("Hooked, calling...\n");
    printf("PID -> %u\n", GetCurrentProcessId());
    return 0;
}
