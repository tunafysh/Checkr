#include <efi/efi.h>
#include <efi/efilib.h>

EFI_STATUS
EFIAPI 

 efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);

    // Set the foreground and background colors
    ST->ConOut->SetAttribute(ST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);

    // Get the screen dimensions
    UINTN columns, rows;
    ST->ConOut->QueryMode(ST->ConOut, ST->ConOut->Mode->Mode, &columns, &rows);

    // Calculate the position to print "Checkmate" in the center
    UINTN xPos = (columns - 9) / 2; // 9 is the length of "Checkmate"
    UINTN yPos = rows / 2;

    // Print "Checkmate" in the center
    Print(L"%*sCheckmate", xPos, L"");

    // Wait for a key press before exiting
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    
    return EFI_SUCCESS;
}
