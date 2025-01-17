#include <filesystem>
#include <Windows.h>
#include <iostream>

std::filesystem::path get_exe_path()
{
    TCHAR buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, _countof(buffer));
    return std::filesystem::path(buffer).parent_path();
}

std::filesystem::path find_api_directory()
{
    return get_exe_path()/(TEXT(R"(ArkApi)"));
}

std::filesystem::path find_dll()
{
    return find_api_directory()/(TEXT(R"(AsaApi.dll)"));
}

std::filesystem::path get_sys_path()
{
	TCHAR sys_dir[MAX_PATH];
	GetSystemDirectoryW(sys_dir, MAX_PATH);
	return std::filesystem::path(sys_dir);
}

std::filesystem::path find_vdll()
{
	return get_sys_path()/(TEXT(R"(Version.dll)"));
}

HINSTANCE m_hinst_dll = nullptr;
HINSTANCE m_hinst_dll2 = nullptr;
extern "C" UINT_PTR mProcs[17]{ 0 };

LPCSTR import_names[] = {
	"GetFileVersionInfoA", "GetFileVersionInfoByHandle", "GetFileVersionInfoExA", "GetFileVersionInfoExW",
	"GetFileVersionInfoSizeA", "GetFileVersionInfoSizeExA", "GetFileVersionInfoSizeExW", "GetFileVersionInfoSizeW",
	"GetFileVersionInfoW", "VerFindFileA", "VerFindFileW", "VerInstallFileA", "VerInstallFileW", "VerLanguageNameA",
	"VerLanguageNameW", "VerQueryValueA", "VerQueryValueW"
};

BOOL WINAPI DllMain(HINSTANCE hinst_dll, DWORD fdw_reason, LPVOID)
{
	if (fdw_reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinst_dll);

		std::wstring vdllName = find_vdll().native();
		m_hinst_dll = LoadLibraryW(vdllName.c_str());
		if (m_hinst_dll == nullptr)
			return FALSE;

		for (int i = 0; i < 17; i++)
		{
			mProcs[i] = reinterpret_cast<UINT_PTR>(GetProcAddress(m_hinst_dll, import_names[i]));
		}

		std::wstring dllName = find_dll().native();
		m_hinst_dll2 = LoadLibraryW(dllName.c_str());
		if (m_hinst_dll2 == nullptr)
			return FALSE;

		using InitFuncType = void (*)();
		InitFuncType InitApi = reinterpret_cast<InitFuncType>(GetProcAddress(m_hinst_dll2, "InitApi"));
		if (InitApi)
			InitApi();
	}
	else if (fdw_reason == DLL_PROCESS_DETACH)
	{
		FreeLibrary(m_hinst_dll2);
		FreeLibrary(m_hinst_dll);
	}

	return TRUE;
}

extern "C" void GetFileVersionInfoA_wrapper();
extern "C" void GetFileVersionInfoByHandle_wrapper();
extern "C" void GetFileVersionInfoExA_wrapper();
extern "C" void GetFileVersionInfoExW_wrapper();
extern "C" void GetFileVersionInfoSizeA_wrapper();
extern "C" void GetFileVersionInfoSizeExA_wrapper();
extern "C" void GetFileVersionInfoSizeExW_wrapper();
extern "C" void GetFileVersionInfoSizeW_wrapper();
extern "C" void GetFileVersionInfoW_wrapper();
extern "C" void VerFindFileA_wrapper();
extern "C" void VerFindFileW_wrapper();
extern "C" void VerInstallFileA_wrapper();
extern "C" void VerInstallFileW_wrapper();
extern "C" void VerLanguageNameA_wrapper();
extern "C" void VerLanguageNameW_wrapper();
extern "C" void VerQueryValueA_wrapper();
extern "C" void VerQueryValueW_wrapper();
