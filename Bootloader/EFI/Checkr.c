#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  UINTN Columns, Rows;
  CHAR16 *Message = L"Checkmate.";
  UINTN MessageLen = StrLen(Message);

  // Query the current console mode to get screen dimensions
  Status = gST->ConOut->QueryMode(gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  if (EFI_ERROR(Status)) {
    // If querying failed, default to a standard size
    Columns = 80;
    Rows = 25;
  }

  // Calculate the starting column to center the message
  UINTN StartColumn = (Columns - MessageLen) / 2;
  // Assume we want to print in the middle row
  UINTN MiddleRow = Rows / 2;

  // Clear the screen
  gST->ConOut->ClearScreen(gST->ConOut);

  // Set the foreground color to white (UEFI Graphics Output Protocol)
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE);

  // Move the cursor to the calculated position
  gST->ConOut->SetCursorPosition(gST->ConOut, StartColumn, MiddleRow);

  // Print the message
  Print(L"%s\n", Message);

  // Hide the cursor
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  while (1){
    SystemTable->BootServices->Stall(1000000); // Stall for 5 seconds
  }

  return EFI_SUCCESS;
}

