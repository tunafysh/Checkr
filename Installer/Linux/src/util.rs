use std::fs;

pub fn copytoefi() -> Result<(), std::io::Error> {
    fs::remove_dir_all("/boot/EFI/").expect("failed to remove dirs");
    fs::create_dir_all("/boot/EFI/BOOT/").expect("failed to create dirs");
    fs::copy("Checkr.efi", "/boot/EFI/BOOT/BOOTX64.EFI").expect("failed to copy file");
    Ok(())
}