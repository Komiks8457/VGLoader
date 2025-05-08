#include "StaticPatches.h"
#include "Memory/MemoryUtility.h"
#include "../version/VersionInfo.h"

char STATIC_VERSION[100];

std::vector<HWND> g_ApplicationWindows;
DWORD g_ProcessId;

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    DWORD windowProcessId;
    GetWindowThreadProcessId(hWnd, &windowProcessId);

    if (windowProcessId == g_ProcessId) {
        g_ApplicationWindows.push_back(hWnd);
    }

    return TRUE;
}

std::vector<HWND> GetAllWindowsOfCurrentProcess()
{
    g_ApplicationWindows.clear();
    g_ProcessId = GetCurrentProcessId();
    EnumWindows(EnumWindowsProc, 0);
    return g_ApplicationWindows;
}

void HideVanguardConsole(bool showhide)
{
    std::vector<HWND> windowList = GetAllWindowsOfCurrentProcess();

    if (!windowList.empty())
    {
        for (std::vector<HWND>::const_iterator it = windowList.begin(); it != windowList.end(); ++it)
        {
            HWND hWnd = *it;
            char windowTitle[256];
            if (!GetWindowTextA(hWnd, windowTitle, 256))
                continue;
            if (hWnd == GetConsoleWindow() && showhide)
                ShowWindow(hWnd, SW_HIDE);
        }
    }
}

void VersionWrite(DWORD dwAddr1, DWORD dwAddr2, DWORD dwAddr3, DWORD dwAddr4)
{
    int MAJOR = PRODUCT_VERSION_MAJOR;
    int MINOR = PRODUCT_VERSION_MINOR;
    int PATCH = PRODUCT_VERSION_PATCH;

    sprintf(STATIC_VERSION, "%%s %%s Compiled - VGLoader v%d.%d.%d", MAJOR, MINOR, PATCH);

    MEMUTIL_WRITE_VALUE(const char*, dwAddr1 + 1, STATIC_VERSION)
    MEMUTIL_WRITE_VALUE(const char*, dwAddr2 + 1, STATIC_VERSION)
    MEMUTIL_WRITE_VALUE(const char*, dwAddr3 + 1, STATIC_VERSION)
    MEMUTIL_WRITE_VALUE(const char*, dwAddr4 + 1, STATIC_VERSION)
}

extern "C" __declspec(dllexport) void LoadCustoms()
{
    //import placeholder, do nothing here.
    return;
}

void LoadDLL(const char* dllFile, const char* title = "Load")
{
    char errorMsg[1024], titleStr[20];
    const std::string& pathString(dllFile);
    size_t lastSeparator = pathString.find_last_of("\\/");
    sprintf(titleStr, "%s Error", title);

    if (std::string::npos == lastSeparator) {
        sprintf(errorMsg, "Failed to load %s", pathString.c_str());
    } else {
        sprintf(errorMsg, "Failed to load %s", pathString.substr(lastSeparator + 1).c_str());
    }

    HMODULE loadLibrary = LoadLibrary(dllFile);
    if (!loadLibrary)
    {
        MessageBoxA(NULL, _T(errorMsg), _T(titleStr), MB_ICONERROR | MB_OK);
        FreeLibrary(loadLibrary);
        exit(1);
    }
}

void APIENTRY LibraryLoader()
{
    TCHAR szFullPath[MAX_PATH], szDriveLetter[MAX_PATH], szDirectory[MAX_PATH], szFileName[MAX_PATH], szPluginDLL[MAX_PATH], szINI[MAX_PATH];

    GetModuleFileName(NULL, szFullPath, MAX_PATH);

    _splitpath(szFullPath, szDriveLetter, szDirectory, szFileName, NULL);

    sprintf(szPluginDLL, "%s%sPlugins\\%s.dll", szDriveLetter, szDirectory, szFileName);

    if (std::strchk(szFullPath).endswith("agentserver.exe"))
    {
        VersionWrite(0x0046F06C, 0x0046F9C0, 0x0048B072, 0x0048B67A);

        CStaticPatches::AgentServerCertPatch();

        LoadDLL("Vanguard\\AgentServer.dll", "Vanguard");

        sprintf(szINI, "%s%sPlugins.ini", szDriveLetter, szDirectory);

        if (std::file(szINI).exists())
        {
            std::inifile config(szINI);
            const std::string& dllFile = config.ReadString(szFileName, "DllFile", "NULL");

            if (dllFile != "NULL" && std::file(dllFile).exists())
            {
                LoadDLL(dllFile.c_str(), "Plugin");
            }
        }
        else if (std::file(szPluginDLL).exists())
        {
            LoadDLL(szPluginDLL, "Plugin");
        }

        std::inifile vanguard(".\\Vanguard\\AgentServer.ini");
        const std::string& showConsole(vanguard.ReadString("AgentServer", "HideConsole", "FALSE"));
        HideVanguardConsole(std::strchk(showConsole).equal("true"));
    }

    if (std::strchk(szFullPath).endswith("downloadserver.exe"))
    {
        VersionWrite(0x0144D2EC, 0x014862B0, 0x01486852, 0x014872AA);

        CStaticPatches::DownloadServerCertPatch();

        LoadDLL("Vanguard\\DownloadServer.dll", "Vanguard");

        if (std::file(szPluginDLL).exists())
        {
            LoadDLL(szPluginDLL, "Plugin");
        }

        std::inifile vanguard(".\\Vanguard\\DownloadServer.ini");
        const std::string& showConsole(vanguard.ReadString("DownloadServer", "HideConsole", "FALSE"));
        HideVanguardConsole(std::strchk(showConsole).equal("true"));
    }

    if (std::strchk(szFullPath).endswith("farmmanager.exe"))
    {
        VersionWrite(0x0145583C, 0x014938B0, 0x01493E52, 0x0149445A);

        CStaticPatches::FarmManagerCertPatch();

        if (std::file(szPluginDLL).exists())
        {
            LoadDLL(szPluginDLL, "Plugin");
        }
    }

    if (std::strchk(szFullPath).endswith("gatewayserver.exe"))
    {
        VersionWrite(0x014CD38C, 0x014D0490, 0x014DD122, 0x014DDB7A);

        CStaticPatches::GatewayServerCertPatch();

        LoadDLL("Vanguard\\GatewayServer.dll", "Vanguard");

        if (std::file(szPluginDLL).exists())
        {
            LoadDLL(szPluginDLL, "Plugin");
        }

        std::inifile vanguard(".\\Vanguard\\GatewayServer.ini");
        const std::string& showConsole(vanguard.ReadString("GatewayServer", "HideConsole", "FALSE"));
        HideVanguardConsole(std::strchk(showConsole).equal("true"));
    }

    if (std::strchk(szFullPath).endswith("globalmanager.exe"))
    {
        VersionWrite(0x0172BB5C, 0x01735A60, 0x01750342, 0x0175094A);

        CStaticPatches::GlobalManagerCertPatch();

        if (std::file(szPluginDLL).exists())
        {
            LoadDLL(szPluginDLL, "Plugin");
        }
    }

    if (std::strchk(szFullPath).endswith("machinemanager.exe"))
    {
        VersionWrite(0x0148D09C, 0x014CEE20, 0x014CF3C2, 0x014CF91A);

        CStaticPatches::MachineManagerCertPatch();

        if (std::file(szPluginDLL).exists())
        {
            LoadDLL(szPluginDLL, "Plugin");
        }
    }

    if (std::strchk(szFullPath).endswith("gameserver.exe"))
    {
        VersionWrite(0x00BC9A8C, 0x00BCDD40, 0x00BF94B2, 0x00BF9A2A);

        CStaticPatches::GameServerCertPatch();

        LoadDLL("Vanguard\\GameServer.dll", "Vanguard");

        sprintf(szINI, "%s%sPlugins.ini", szDriveLetter, szDirectory);

        if (std::file(szINI).exists())
        {
            std::inifile config(szINI);
            const std::string& dllFile = config.ReadString(szFileName, "DllFile", "NULL");
            
            if (dllFile != "NULL" && std::file(dllFile).exists())
            {
                LoadDLL(dllFile.c_str(), "Plugin");
            }
        }
        else if (std::file(szPluginDLL).exists())
        {
            LoadDLL(szPluginDLL, "Plugin");
        }

        std::inifile vanguard(".\\Vanguard\\GameServer.ini");
        const std::string& showConsole(vanguard.ReadString("GameServer", "HideConsole", "FALSE"));
        HideVanguardConsole(std::strchk(showConsole).equal("true"));
    }

    if (std::strchk(szFullPath).endswith("shardmanager.exe"))
    {
        VersionWrite(0x009F113C, 0x009F1FC0, 0x00A034C2, 0x00A03A3A);

        CStaticPatches::ShardManagerCertPatch();

        LoadDLL("Vanguard\\ShardManager.dll", "Vanguard");

        if (std::file(szPluginDLL).exists())
        {
            LoadDLL(szPluginDLL, "Plugin");
        }

        std::inifile vanguard(".\\Vanguard\\ShardManager.ini");
        const std::string& showConsole(vanguard.ReadString("ShardManager", "HideConsole", "FALSE"));
        HideVanguardConsole(std::strchk(showConsole).equal("true"));
    }
}

BOOL APIENTRY DllMain(HMODULE, const DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            LibraryLoader();
            break;
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
        default:
            break;
    }

    return TRUE;
}