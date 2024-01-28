import os, platform, sys, subprocess, shutil

osname = platform.os.name

buildexists = os.path.exists("build")

if buildexists:
    cleanup()

if osname == "posix":
    print("Build started...")
    os.mkdir("build")
    os.mkdir("build/deps")
    print("Building Bootloaders")
    subprocess.Popen("cd Bootloader/BIOS && Makefile", shell=True)
    subprocess.Popen("cd Bootloader/EFI && cargo build --target x86_64-unknown-uefi", shell=True)
    print("Copying files")
    shutil.copyfile("Bootloader/BIOS/boot.bin", "build/deps/boot.bin")
    shutil.copyfile("Bootloader/EFI/target/x86_64-unknown-uefi/release/checkr-efi.efi", "build/deps/bootx64.efi")
    shutil.copyfile("Installer/Linux/install", "build/install.sh")
    print("Packing it up")
    shutil.make_archive("Checkr", 'zip', "build")
    print("Cleaning it up")
    os.rmdir("build")
    os.remove("Bootloader/BIOS/boot.bin")
    os.rmdir("Bootloader/EFI/target")
    print("Build finished! The output file is: Checkr.zip")
else:
    print("Unsupported system. Use linux please.")
    
def cleanup():
    """Cleans up all the binaries.
    """
    os.rmdir("build")
    os.remove("Bootloader/BIOS/boot.bin")
    os.rmdir("Bootloader/EFI/target")