#include <efi/efi.h>
#include <efi/efiapi.h>

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    // Print the message
    Print("Hello World!");

    // Wait for key press before exiting
    SystemTable->BootServices->Stall(5000000); // Stall for 5 seconds

    return EFI_SUCCESS;
}