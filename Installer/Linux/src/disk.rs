use serde_json::{Result, Value};
use std::process;

pub fn get_current_disk() -> Result<Value> {
    let disk = process::Command::new("findmnt")
        .arg("-J -M /")
        .output()
        .expect("failed to execute process");
    serde_json::from_slice(&disk.stdout)
}

fn parse_disk(disk: Value) -> String {
    let disk = disk["filesystems"][0]["mountpoint"].to_string();
    disk.replace('"', "")
}

pub fn find_efi(disk: Value) -> String {
    let disk = parse_disk(disk);
    let mut efi_disk = disk.to_string();
    if disk.contains("sda") {
        efi_disk = "/dev/sda1".to_string();
    }
    else if disk.contains("nvme0n1") {
        efi_disk = "/dev/nvme0n1p1".to_string();
    }
    else if disk.contains("hda") {
        efi_disk = "/dev/hda1".to_string();
    }

    efi_disk
}