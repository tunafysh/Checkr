#!env python3

import os, shutil, platform, cursor, atexit, json
from colorama import Fore

atexit.register(cursor.show)

cursor.hide()

if os.path.exists("config.json") == False:
    print(f"{Fore.RED} Configuration file not found. Please ensure you have a configuration file.")
    exit(1)

conf = json.load(open("config.json"))

def verbose(fore, msg):
    if conf["verbose"] == "true": print(f"{fore} {msg} {Fore.RESET}")

def is_tool(name):
    """Check whether `name` is on PATH and marked as executable."""
    return shutil.which(name) is not None

def prepare():
    verbose(Fore.CYAN, "Preparing build environment...")
    verbose(Fore.CYAN, "Checking tools...")
    if platform.system() == "Linux":
        if not is_tool("make") == True: 
            print("make is not installed") 
            exit(1)
        
        if not is_tool("gcc") == True: 
            print("gcc is not installed")
            exit(1)
    elif platform.system() == "Windows":
        if not is_tool("msvc") == True: 
            print("nmake is not installed")
            exit(1)
        
    if not is_tool("git") == True: 
        print("git is not installed")
        exit(1)
        
    if not is_tool("nasm") == True: 
        print("nasm is not installed")
        exit(1)
    
    if conf["cleanup"] == "true": 
        cleanup()
        
    if os.path.exists("build") == False: os.mkdir("build")
    if os.path.exists("bin") == False: os.mkdir("bin")
    
    atexit.register(cleanup)
    
def makebios():
    print(f"{Fore.BLUE} BIOS bootloader is being built...")
    os.chdir("Bootloader/BIOS")
    os.system("make > /dev/null")
    shutil.copyfile("boot.bin", "../../bin/boot.bin")
    os.remove("boot.bin")
    done = True
    print(f"{Fore.GREEN} BIOS bootloader built!{Fore.RESET}")
    
def cleanup():
    verbose(Fore.CYAN, "Cleaning up...")
    if os.path.exists("bin"): shutil.rmtree("bin", ignore_errors=True)
    if os.path.exists("build"): shutil.rmtree("build", ignore_errors=True)
    verbose(Fore.GREEN, "Cleanup Done!\n{Fore.RESET}")
    
def makeefi():
    print(f"{Fore.BLUE} EFI bootloader is being built...{Fore.RESET}")
    os.chdir("../../build")
    verbose(Fore.CYAN, "Checking for edk2 installation...")
    if os.path.exists("edk2") != True: 
        verbose(Fore.CYAN, "Cloning edk2...")
        os.system("git clone https://github.com/tianocore/edk2.git")
        os.chdir("edk2")
        verbose(Fore.CYAN, "Initializing submodules...")
        os.system("git submodule update --init --recursive")
        verbose(Fore.CYAN, "Compiling tools...")
        os.system("make -C BaseTools > /dev/null")
    else:
        os.chdir("edk2")
    
    os.mkdir("CheckrPkg")
    os.chdir("CheckrPkg")
    verbose(Fore.CYAN, "Copying files...")
    shutil.copyfile("../../../Bootloader/EFI/CheckrPkg.dsc", "CheckrPkg.dsc")
    shutil.copyfile("../../../Bootloader/EFI/Checkr.inf", "Checkr.inf")
    shutil.copyfile("../../../Bootloader/EFI/Checkr.c", "Checkr.c")
    os.chdir("../")
    verbose(Fore.CYAN, "Configuring edk2...")
    os.system("bash -c \"source edksetup.sh\"")
    eficonf = open("Conf/target.txt").readlines()
    eficonf[19] = "ACTIVE_PLATFORM       = CheckrPkg/CheckrPkg.dsc\n"
    if conf["target"] == "DEBUG": eficonf[27] = "TARGET                = DEBUG\n"
    else: eficonf[27] = "TARGET                = RELEASE\n"
    if conf["arch"] == "x64": eficonf[43] = "TARGET_ARCH           = X64\n"
    elif conf["arch"] == "x86": eficonf[43] = "TARGET_ARCH           = IA32\n"
    elif conf["arch"] == "arm64": eficonf[43] = "TARGET_ARCH           = AArch64\n"
    eficonf[53] = "TOOL_CHAIN_TAG        = GCC5\n"
    with open("Conf/target.txt", "w") as f: f.writelines(eficonf)
    verbose(Fore.CYAN, "Building bootloader...")
    os.system("bash -c \"source edksetup.sh && build\"")
    shutil.copyfile(f"CheckrPkg/Build/{conf["target"]}_GCC5/{"X64" if conf["arch"] == "x64" else "IA32" if conf["arch"] == "x86" else "AArch64"}/Checkr.efi", "../../bin/boot.efi")
    print(f"{Fore.GREEN}EFI bootloader built!{Fore.RESET}")

def makelinux():
    print(f"{Fore.BLUE} Linux installer is being built...")
    os.chdir("Installer/Linux")
    verbose(Fore.CYAN, "Compiling forkbomb...")
    os.system("gcc -o fork fork.c")
    verbose(Fore.CYAN, "Copying files...")
    os.chdir("../../")
    
    print(f"{Fore.GREEN} Linux installer built!{Fore.RESET}")

prepare()

print(f"{Fore.MAGENTA} Configuration:\n Build type: {conf["buildtype"]}\n Architecture: {conf["arch"]}\n Package name: {conf["pkgname"]}\n")

if platform.system() == "Linux": print(f"{Fore.YELLOW}Checklist:\n + Build BIOS Bootloader\n + Build UEFI Bootloader\n + Build Installer\n")
if platform.system() == "Windows": print(f"{Fore.YELLOW}Checklist:\n + Build Installer\n")

if conf["buildtype"] == "all" and platform.system() == "Linux":
    makebios()
    makeefi()
    # makelinux()
elif conf["buildtype"] == "all" and platform.system() == "Windows":
    makebios()
    makeefi()
    makewindows()
elif conf["buildtype"] == "bootloader" and platform.system() == "Linux":
    makebios()
    makeefi()
elif conf["buildtype"] == "installer" and platform.system() == "Linux":
    makelinux()
elif conf["buildtype"] == "installer" and platform.system() == "Windows":
    makewindows()
else:
    print(f"{Fore.RED}Invalid build type: {conf["buildtype"]}")