#include <efi/efi.h>
#include <efi/efiapi.h>

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    EFI_SIMPLE_TEXT_OUT_PROTOCOL *ConOut = SystemTable->ConOut;

    // Reset the screen to be sure the screen is clear
    ConOut->Reset(ConOut, FALSE);

    // Get screen size
    UINTN cols, rows;
    ConOut->QueryMode(ConOut, ConOut->Mode->Mode, &cols, &rows);

    // Prepare message
    CHAR16 *msg = L"Hello World!";
    UINTN msg_len = StrLen(msg);

    // Calculate starting column (for centering)
    UINTN col_start = (cols - msg_len) / 2;
    UINTN row_start = rows / 2; // Middle row

    // Set cursor position
    ConOut->SetCursorPosition(ConOut, col_start, row_start);

    // Print the message
    Print(L"%s\n", msg);

    // Wait for key press before exiting
    SystemTable->BootServices->Stall(5000000); // Stall for 5 seconds

    return EFI_SUCCESS;
}