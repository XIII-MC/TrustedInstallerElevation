#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

void ToggleTrustedInstaller(const bool status) {

    const SC_HANDLE hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!hSCManager) {

        std::cerr << "Failed to connect to Service Control Manager" << std::endl;

        return;

    }

    const SC_HANDLE hService = OpenService(hSCManager, "TrustedInstaller.exe", (status == true ? SERVICE_START : SERVICE_STOP));
    if (!hService) {

        std::cerr << "Failed to open TrustedInstaller" << std::endl;

        CloseServiceHandle(hSCManager);
        return;

    }

    if (!StartService(hService, 0, nullptr)) {

        if (const DWORD err = GetLastError(); err == ERROR_SERVICE_ALREADY_RUNNING) std::cout << "Service already running." << std::endl;
        else std::cerr << "Failed to start service. Error code: " << err << std::endl;

    } else std::cout << "Service started successfully." << std::endl;

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

}

DWORD GetTrustedInstallerPID() {

    const HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);

    if (Process32First(hSnapshot, &pe)) {

        do {

            std::string exe(pe.szExeFile);
            std::string exeName(exe.begin(), exe.end());

            if (_stricmp(exeName.c_str(), "TrustedInstaller.exe") == 0) {

                CloseHandle(hSnapshot);
                return pe.th32ProcessID;

            }

        } while (Process32Next(hSnapshot, &pe));

    }

    CloseHandle(hSnapshot);
    return 0;

}

bool EnableDebugPrivilege() {

    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return false;

    if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &tp.Privileges[0].Luid)) {

        CloseHandle(hToken);

        return false;

    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr);
    CloseHandle(hToken);

    return GetLastError() == ERROR_SUCCESS;

}

void ElevateSubProcess() {

    HANDLE hParent = OpenProcess(PROCESS_CREATE_PROCESS, FALSE, GetTrustedInstallerPID());
    if (!hParent) {

        std::cerr << "Failed to open TrustedInstaller. Error: " << GetLastError() << "\n";

        return;

    }

    SIZE_T size = 0;
    STARTUPINFOEXA siex = { 0 };
    siex.StartupInfo.cb = sizeof(siex);

    InitializeProcThreadAttributeList(nullptr, 1, 0, &size);
    siex.lpAttributeList = static_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(HeapAlloc(GetProcessHeap(), 0, size));
    InitializeProcThreadAttributeList(siex.lpAttributeList, 1, 0, &size);

    UpdateProcThreadAttribute(
        siex.lpAttributeList, 0,
        PROC_THREAD_ATTRIBUTE_PARENT_PROCESS,
        &hParent, sizeof(HANDLE), nullptr, nullptr
    );

    PROCESS_INFORMATION pi = {};
    const BOOL success = CreateProcessA(
        nullptr,
        const_cast<LPSTR>("cmd.exe"),
        nullptr, nullptr, FALSE,
        EXTENDED_STARTUPINFO_PRESENT | CREATE_NEW_CONSOLE,
        nullptr, nullptr,
        &siex.StartupInfo,
        &pi
    );

    if (success) {

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

    }

    DeleteProcThreadAttributeList(siex.lpAttributeList);
    HeapFree(GetProcessHeap(), 0, siex.lpAttributeList);
    CloseHandle(hParent);

}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    EnableDebugPrivilege();

    ToggleTrustedInstaller(false);
    ToggleTrustedInstaller(true);

    ElevateSubProcess();

    return 0;

}
