#include <iostream>
#include <windows.h>
#include <lm.h>

#pragma comment(lib, "netapi32.lib")

bool IsWindows10OrGreater() {
    OSVERSIONINFOEX osvi;
    DWORDLONG dwlConditionMask = 0;
    int op = VER_GREATER_EQUAL;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osvi.dwMajorVersion = 10;
    osvi.dwMinorVersion = 0;

    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
    VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);

    return VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION, dwlConditionMask) != FALSE;
}

int main() {
    // Récupérer le nom de l'ordinateur
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName);
    if (!GetComputerNameA(computerName, &size)) {
        std::cerr << "Erreur lors de la récuperation du nom de l'ordinateur.\n";
        return 1;
    }
    std::cout << "Nom de l'ordinateur : " << computerName << std::endl;

    // Récupérer le nombre de processeurs
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    std::cout << "Nombre de processeurs : " << systemInfo.dwNumberOfProcessors << std::endl;

    // Récupérer la mémoire restante
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);
    GlobalMemoryStatusEx(&memoryStatus);
    std::cout << "Memoire restante : " << memoryStatus.ullAvailPhys << " octets\n";

    // Afficher des informations sur le système d'exploitation
    std::cout << "Systeme d'exploitation : ";
    if (IsWindows10OrGreater()) {
        std::cout << "Windows 10 ou version ulterieure\n";
    }
    else {
        std::cout << "Une version anterieure de Windows\n";
    }

    // Récupérer le nom d'utilisateur Windows
    char username[UNLEN + 1];
    size = UNLEN + 1;
    if (!GetUserNameA(username, &size)) {
        std::cerr << "Erreur lors de la récuperation du nom d'utilisateur.\n";
        return 1;
    }
    std::cout << "Nom d'utilisateur : " << username << std::endl;

    // Récupérer les groupes d'appartenance de l'utilisateur Windows
    wchar_t usernameW[UNLEN + 1];
    size = UNLEN + 1;
    if (!MultiByteToWideChar(CP_ACP, 0, username, -1, usernameW, UNLEN + 1)) {
        std::cerr << "Erreur lors de la conversion du nom d'utilisateur.\n";
        return 1;
    }

    LOCALGROUP_USERS_INFO_0* groups;
    DWORD entriesRead, totalEntries;
    DWORD result = NetUserGetLocalGroups(nullptr, usernameW, 0, LG_INCLUDE_INDIRECT, (LPBYTE*)&groups, MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries);
    if (result == NERR_Success) {
        std::cout << "Groupes d'appartenance :\n";
        for (DWORD i = 0; i < entriesRead; ++i) {
            std::wcout << groups[i].lgrui0_name << std::endl;
        }
        NetApiBufferFree(groups);
    }
    else {
        std::cerr << "Erreur lors de la récuperation des groupes d'appartenance.\n";
    }

    return 0;
}
