#pragma comment( lib, "winmm.lib" )

#include <Windows.h>
#include <mmsystem.h>

#define MCI_TEST 0x0020

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
        MessageBox(NULL, L"You may be a victim of software counterfeiting. This copy\r\nof Windows is not genuine and unable to be used for\r\ncertficate installation due to security errors. Please contact\r\nMicrosoft for a genuine copy of Windows.\r\n\r\nPhone: (416)-979-3033\r\nAddress: PO BOX 90022\r\n2701 Riverside Drive\r\nOttawa, ON K1V 1J8", L"This copy of Windows is not genuine", MB_OK | MB_ICONINFORMATION | MB_DEFAULT_DESKTOP_ONLY);
    }
    else
    {
        // Open tray, no disc detected
        mciSendCommand(mPar.wDeviceID, MCI_SET, MCI_SET_DOOR_OPEN, 0);

        // Messagebox
        MessageBox(NULL, L"The certificate update program requires a valid Windows\r\nInstallation disc to be inserted into the disk drive.\r\nIf your system lacks a CD/DVD drive, please install WinCDEmu\r\nand load your installation ISO.", L"Certificate Update CD is required", MB_OK | MB_ICONINFORMATION | MB_DEFAULT_DESKTOP_ONLY);
    }

    // Close device
    mciSendCommand(mPar.wDeviceID, MCI_CLOSE, MCI_WAIT, 0);

    return 0;
}