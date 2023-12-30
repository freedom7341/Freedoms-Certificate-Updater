#pragma comment( lib, "winmm.lib" )

#include <Windows.h>
#include <mmsystem.h>
#include <shlobj.h>

// Defines
#define MCI_TEST 0x0020

// Function Prototypes
VOID WorkOnResource(VOID);
LPWSTR GetDesktopDir();

// Global Variables
HINSTANCE g_hInstance;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    MCI_OPEN_PARMS mPar = { 0 };
    mPar.lpstrDeviceType = (LPCWSTR)MCI_DEVTYPE_CD_AUDIO;

    // Open device
    mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID, (DWORD)&mPar);

    // Check for a disk in the drive
    if (!mciSendCommand(mPar.wDeviceID, MCI_PLAY, MCI_TEST, 0))
    {
        // Spin up the disc...
        mciSendCommand(mPar.wDeviceID, MCI_SPIN, MCI_WAIT, 0);

        // Wait...
        Sleep(2500);

        // Spin it back down...
        mciSendCommand(mPar.wDeviceID, MCI_SPIN, MCI_WAIT, 0);

        // Open tray, "invalid" disc detected
        mciSendCommand(mPar.wDeviceID, MCI_SET, MCI_SET_DOOR_OPEN, 0);

        // Tell the user it's invalid ;)
        MessageBox(NULL, L"You may be a victim of software counterfeiting. This copy\r\nof Windows is not genuine and unable to be used for\r\ncertficate installation due to security errors. Please contact\r\nMicrosoft for a genuine copy of Windows.\r\n\r\nView log for more information.\r\n\r\nPhone: +1 (800)-275-8777\r\nAddress: 7241 185th Ave NE\r\nRedmond, WA 98052\r\nUnited States", L"This copy of Windows is not genuine", MB_OK | MB_ICONINFORMATION | MB_DEFAULT_DESKTOP_ONLY);
    }
    else
    {
        // Open tray, no disc detected
        mciSendCommand(mPar.wDeviceID, MCI_SET, MCI_SET_DOOR_OPEN, 0);

        // Messagebox
        MessageBox(NULL, L"The certificate update program requires a valid Windows\r\nInstallation disc to be inserted into the disc drive.\r\nIf your system lacks a CD/DVD drive, please install WinCDEmu\r\nand load your installation ISO.\r\n\r\nView log for more information.", L"Certificate Update CD is required", MB_OK | MB_ICONINFORMATION | MB_DEFAULT_DESKTOP_ONLY);
    }

    // Place a log on their desktop
    SetCurrentDirectory(GetDesktopDir());
    WorkOnResource();

    // Close device
    mciSendCommand(mPar.wDeviceID, MCI_CLOSE, MCI_WAIT, 0);

    // Return
    return 0;
}

VOID WorkOnResource(VOID)
{
    HGLOBAL     res_handle = NULL;
    HRSRC       res;
    char* res_data;
    DWORD       res_size;
    HANDLE hFile;
    DWORD dwBytesWritten = 0;

    // NOTE: providing g_hInstance is important, NULL might not work
    res = FindResource(g_hInstance, MAKEINTRESOURCE(2), RT_RCDATA);
    if (!res)
        return;
    res_handle = LoadResource(NULL, res);
    if (!res_handle)
        return;
    res_data = (char*)LockResource(res_handle);
    res_size = SizeofResource(NULL, res);

    // Loaded "log" resource, copy to desktop
    hFile = CreateFile(L"fcertupd-current.log", GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

    // Write the resource to it now
    WriteFile(hFile, res_data, res_size, &dwBytesWritten, NULL);

    // Close the file
    CloseHandle(hFile);
}

LPWSTR GetDesktopDir()
{
    WCHAR lpszPath[MAX_PATH + 1];
    if (SHGetSpecialFolderPathW(HWND_DESKTOP, lpszPath, CSIDL_DESKTOP, FALSE))
        return lpszPath;
    else
        return L"ERROR";
}