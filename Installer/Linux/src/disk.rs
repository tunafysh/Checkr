use std::{fs::{self, File}, io::{Read, Write}};

pub fn writetodisk(disk: String) -> Result<(), std::io::Error>{ 
    let mut inpfile = File::open("/lib/boot.bin")?;

    let mut buffer = Vec::new();
    inpfile.read_to_end(&mut buffer)?;

    let mut outfile = File::create(disk)?;
    outfile.write_all(&buffer)?;

    Ok(())
}

pub fn copytoefi() -> Result<(), std::io::Error> {
    fs::remove_dir_all("/boot/EFI/").expect("failed to remove dirs");
    fs::remove_dir_all("/boot/efi/EFI/").expect("failed to remove dirs");
    fs::create_dir_all("/boot/EFI/BOOT/").expect("failed to create dirs");
    fs::copy("/lib/boot.efi", "/boot/EFI/BOOT/BOOTX64.EFI").expect("failed to copy file");
    Ok(())
}