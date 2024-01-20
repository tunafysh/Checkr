#include <WinUser.h>
#include <Windows.h>
#include <iostream>

int main()
{
    int testmsgbox = MessageBox(NULL, L"test", L"test", MB_OK | MB_ICONINFORMATION);
    return 0;
}