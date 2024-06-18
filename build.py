import os, shutil, platform, cursor, atexit, json
from colorama import Fore

atexit.register(cursor.show)

cursor.hide()

def is_tool(name):
    """Check whether `name` is on PATH and marked as executable."""
    return shutil.which(name) is not None

def prepare():
    if not is_tool("make") == True: 
        print("make is not installed") 
        exit(1)
        
    if not is_tool("git") == True: 
        print("git is not installed")
        exit(1)
        
    if not is_tool("nasm") == True: 
        print("nasm is not installed")
        exit(1)
        
    if not is_tool("gcc") == True: 
        print("gcc is not installed")
        exit(1)
        
    

    # os.mkdir("build")
    # os.mkdir("bin")
    
    atexit.register(cleanup)
    
def makebios():
    print(f"{Fore.CYAN} BIOS bootloader is being built...")
    os.chdir("Bootloader/BIOS")
    os.system("make > /dev/null")
    shutil.copyfile("boot.bin", "../../bin/boot.bin")
    os.remove("boot.bin")
    done = True
    print(f"{Fore.GREEN} BIOS bootloader built!{Fore.RESET}")
    
def cleanup():
    print(f"{Fore.CYAN} Cleaning up...")
    if os.path.exists("bin"): shutil.rmtree("bin", ignore_errors=True)
    if os.path.exists("build"): shutil.rmtree("build", ignore_errors=True)
    print(f"{Fore.GREEN} Done!")
    
def makeefi():
    os.chdir("../../build")
    if os.path.exists("edk2") != True: os.system("git clone https://github.com/tianocore/edk2.git")
    os.chdir("edk2")
    # os.system("git submodule update --init --recursive")
    # os.system("make -C BaseTools > /dev/null")
    # os.mkdir("CheckrPkg")
    os.chdir("CheckrPkg")
    shutil.copyfile("../../../Bootloader/EFI/CheckrPkg.dsc", "CheckrPkg.dsc")
    shutil.copyfile("../../../Bootloader/EFI/Checkr.inf", "Checkr.inf")
    shutil.copyfile("../../../Bootloader/EFI/Checkr.c", "Checkr.c")
    os.chdir("../")
    print(os.listdir())
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
    os.system("bash -c \"source edksetup.sh && build\"")
    shutil.copyfile(f"CheckrPkg/Build/{conf["target"]}_GCC5/{"X64" if conf["arch"] == "x64" else "IA32" if conf["arch"] == "x86" else "AArch64"}/Checkr.efi", "../../bin/boot.efi")
    
if os.path.exists("config.json") == False:
    print(f"{Fore.RED} Configuration file not found. Please ensure you have a configuration file.")
    exit(1)

conf = json.load(open("config.json"))

prepare()

print(f"{Fore.MAGENTA} Configuration:\n Build type: {conf["buildtype"]}\n Architecture: {conf["arch"]}\n Package name: {conf["pkgname"]}\n")

if platform.system() == "Linux": print(f"{Fore.YELLOW}Checklist:\n + Build BIOS Bootloader\n + Build UEFI Bootloader\n + Build Installer\n")
if platform.system() == "Windows": print(f"{Fore.YELLOW}Checklist:\n + Build Installer\n")

if conf["buildtype"] == "all":
    makebios()
    makeefi()
    # makelinux()
elif conf["buildtype"] == "bootloader" and platform.system() == "Linux":
    makebios()
    makeefi()
elif conf["buildtype"] == "installer" and platform.system() == "Linux":
    makelinux()
elif conf["buildtype"] == "installer" and platform.system() == "Windows":
    makewindows()
else:
    print(f"{Fore.RED}Invalid build type: {conf["buildtype"]}")