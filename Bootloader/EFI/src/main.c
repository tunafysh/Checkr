#include <efi/efi.h>
#include <efi/efilib.h>

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *systable)
{
    InitializeLib(ImageHandle, systable);
    
    uefi_call_wrapper(systable->ConOut->ClearScreen, 1, systable->ConOut);
    Print(L"Hello, world!\n");
    int i;
    for (i = 0; i < 10; i++){
        uefi_call_wrapper(systable->BootServices->Stall, 1, 1000000);
    }
    uefi_call_wrapper(systable->RuntimeServices->ResetSystem, 1, 4);
    

    return EFI_SUCCESS;
}