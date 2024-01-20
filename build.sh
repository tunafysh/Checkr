#!/bin/bash

clean() {
    rm -f Bootloader/boot.bin
    rm -f Installer/boot.bin
}

if [[ $1 == "clean" ]]
then

    echo -e "\e[1;34m[+] Cleaning the directory"

    clean

else

echo -e "\e[1;34m[+] Checking for dependencies"

command -v nasm >> /dev/null

if [[ $? == "" ]]
then
    echo -e "\e[1;32m[+] Please install the following dependencies: nasm, zip."
fi

echo -e "\e[1;34m[+] Copying files."

cd Bootloader/BIOS && make >> /dev/null

cd ..

cp Bootloader/boot.bin Installer

echo -e "\e[1;34m[+] Packing it up."

zip -r installer.zip ./Installer/* >> /dev/null

echo -e "\e[1;34m[+] Cleaning the directory"

clean

echo -e "\e[1;32m[+] Done."

fi