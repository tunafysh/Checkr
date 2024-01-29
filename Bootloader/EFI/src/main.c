#include 
#include <efi/efilib.h>

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *systable)
{
    InitializeLib(ImageHandle, systable);
    
    Print(L"Hello, world!\n");

    

    return EFI_SUCCESS;
}