import os, platform, sys, argparse, subprocess, shutil

parser = argparse.ArgumentParser()

parser.add_argument("--wsl", help="Use if you want to use wsl on windows.")

args = parser.parse_args()

print(args.wsl)

osname = platform.os.name

if osname == "nt":
    print("Build started...")
    subprocess.Popen("cd Bootloader/BIOS && Makefile", shell=True)
else:
    print("Unsupported system. Use linux please.")
 
