#include <efi/efi.h>
#include <efi/efi_loader.h>
#include <efi/efi_system_table.h>
#include <stdlib.h>
#include <string.h>

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput;
  EFI_GRAPHICS_MODE_INFORMATION *ModeInfo;
  UINTN ModeNumber;
  UINTN HorizontalResolution, VerticalResolution;
  UINTN SizeOfInfo, FrameBufferBase, FrameBufferSize;
  CHAR16 *OutputString;
  EFI_STATUS Status;

  Status = SystemTable->ConOut->QueryMode (SystemTable->ConOut,
                                            SystemTable->ConOut->Mode,
                                            &SizeOfInfo,
                                            &HorizontalResolution,
                                            &VerticalResolution,
                                            &ModeNumber);
  if (EFI_ERROR (Status))
    return Status;

  ModeInfo = AllocatePool (SizeOfInfo);
  if (ModeInfo == NULL)
    return EFI_OUT_OF_RESOURCES;

  Status = SystemTable->ConOut->GetMode (SystemTable->ConOut, ModeNumber,
                                         ModeInfo);
  if (EFI_ERROR (Status))
    return Status;

  FrameBufferBase = (UINTN) ModeInfo->FrameBufferBase;
  FrameBufferSize = ModeInfo->FrameBufferSize;

  FreePool (ModeInfo);

  GraphicsOutput = SystemTable->BootServices->OpenProtocol (ImageHandle,
                                                              &gEfiGraphicsOutputProtocolGuid,
                                                              (void **) &GraphicsOutput,
                                                              ImageHandle,
                                                              NULL,
                                                              EFI_OPEN_PROTOCOL_GET_PROTOCOL);

  if (EFI_ERROR (GraphicsOutput))
    return GraphicsOutput->StatusCode;

  OutputString = AllocatePool (sizeof (CHAR16) * (strlen ("Checkmate.") + 1));
  if (OutputString == NULL)
    return EFI_OUT_OF_RESOURCES;

  StrCpy (OutputString, L"Checkmate.");

  GraphicsOutput->SetCursorPosition (GraphicsOutput,
                                     HorizontalResolution / 2,
                                     VerticalResolution / 2);
  GraphicsOutput->ClearScreen (GraphicsOutput);
  GraphicsOutput->SetAttribute (GraphicsOutput, EFI_TEXT_ATTR (EFI_LIGHTGRAY, 0));
  GraphicsOutput->DrawHank (GraphicsOutput, FrameBufferBase, HorizontalResolution,
                             VerticalResolution, 0, 0, (UINTN) OutputString,
                             sizeof (CHAR16) * (strlen (OutputString) + 1));

  FreePool (OutputString);
  GraphicsOutput->Close (GraphicsOutput);

  return EFI_SUCCESS;
}