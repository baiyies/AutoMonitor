#pragma once

#include <windows.h>
#include <winhttp.h>
#include <stdint.h>

typedef struct _UNICODE_STR {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR pBuffer;
} UNICODE_STR, * PUNICODE_STR;

// WinDbg> dt -v ntdll!_LDR_DATA_TABLE_ENTRY
//__declspec( align(8) )
typedef struct _LDR_DATA_TABLE_ENTRY {
	// LIST_ENTRY InLoadOrderLinks; // As we search from PPEB_LDR_DATA->InMemoryOrderModuleList we dont use the first
	// entry.
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STR FullDllName;
	UNICODE_STR BaseDllName;
	ULONG Flags;
	SHORT LoadCount;
	SHORT TlsIndex;
	LIST_ENTRY HashTableEntry;
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

// WinDbg> dt -v ntdll!_PEB_LDR_DATA
typedef struct _PEB_LDR_DATA  //, 7 elements, 0x28 bytes
{
	DWORD dwLength;
	DWORD dwInitialized;
	LPVOID lpSsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	LPVOID lpEntryInProgress;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

// WinDbg> dt -v ntdll!_PEB_FREE_BLOCK
typedef struct _PEB_FREE_BLOCK  // 2 elements, 0x8 bytes
{
	struct _PEB_FREE_BLOCK* pNext;
	DWORD dwSize;
} PEB_FREE_BLOCK, * PPEB_FREE_BLOCK;

// struct _PEB is defined in Winternl.h but it is incomplete
// WinDbg> dt -v ntdll!_PEB
typedef struct __PEB  // 65 elements, 0x210 bytes
{
	BYTE bInheritedAddressSpace;
	BYTE bReadImageFileExecOptions;
	BYTE bBeingDebugged;
	BYTE bSpareBool;
	LPVOID lpMutant;
	LPVOID lpImageBaseAddress;
	PPEB_LDR_DATA pLdr;
	LPVOID lpProcessParameters;
	LPVOID lpSubSystemData;
	LPVOID lpProcessHeap;
	PRTL_CRITICAL_SECTION pFastPebLock;
	LPVOID lpFastPebLockRoutine;
	LPVOID lpFastPebUnlockRoutine;
	DWORD dwEnvironmentUpdateCount;
	LPVOID lpKernelCallbackTable;
	DWORD dwSystemReserved;
	DWORD dwAtlThunkSListPtr32;
	PPEB_FREE_BLOCK pFreeList;
	DWORD dwTlsExpansionCounter;
	LPVOID lpTlsBitmap;
	DWORD dwTlsBitmapBits[2];
	LPVOID lpReadOnlySharedMemoryBase;
	LPVOID lpReadOnlySharedMemoryHeap;
	LPVOID lpReadOnlyStaticServerData;
	LPVOID lpAnsiCodePageData;
	LPVOID lpOemCodePageData;
	LPVOID lpUnicodeCaseTableData;
	DWORD dwNumberOfProcessors;
	DWORD dwNtGlobalFlag;
	LARGE_INTEGER liCriticalSectionTimeout;
	DWORD dwHeapSegmentReserve;
	DWORD dwHeapSegmentCommit;
	DWORD dwHeapDeCommitTotalFreeThreshold;
	DWORD dwHeapDeCommitFreeBlockThreshold;
	DWORD dwNumberOfHeaps;
	DWORD dwMaximumNumberOfHeaps;
	LPVOID lpProcessHeaps;
	LPVOID lpGdiSharedHandleTable;
	LPVOID lpProcessStarterHelper;
	DWORD dwGdiDCAttributeList;
	LPVOID lpLoaderLock;
	DWORD dwOSMajorVersion;
	DWORD dwOSMinorVersion;
	WORD wOSBuildNumber;
	WORD wOSCSDVersion;
	DWORD dwOSPlatformId;
	DWORD dwImageSubsystem;
	DWORD dwImageSubsystemMajorVersion;
	DWORD dwImageSubsystemMinorVersion;
	DWORD dwImageProcessAffinityMask;
	DWORD dwGdiHandleBuffer[34];
	LPVOID lpPostProcessInitRoutine;
	LPVOID lpTlsExpansionBitmap;
	DWORD dwTlsExpansionBitmapBits[32];
	DWORD dwSessionId;
	ULARGE_INTEGER liAppCompatFlags;
	ULARGE_INTEGER liAppCompatFlagsUser;
	LPVOID lppShimData;
	LPVOID lpAppCompatInfo;
	UNICODE_STR usCSDVersion;
	LPVOID lpActivationContextData;
	LPVOID lpProcessAssemblyStorageMap;
	LPVOID lpSystemDefaultActivationContextData;
	LPVOID lpSystemAssemblyStorageMap;
	DWORD dwMinimumStackCommit;
} _PEB, * _PPEB;

typedef struct {
	WORD offset : 12;
	WORD type : 4;
} IMAGE_RELOC, * PIMAGE_RELOC;

#define cast(t, a) ((t)(a))
#define cast_offset(t, p, o) ((t)((uint8_t *)(p) + (o)))


#define Kernel32Lib_Hash 0x1cca9ce6

// kernel32
#define GetProcAddress_Hash 0x1AB9B854
typedef void* (__stdcall* _GetProcAddress)(HMODULE, char*);

#define LoadLibraryA_Hash 0x7F201F78
typedef HMODULE(__stdcall* _LoadLibraryA)(LPCSTR lpLibFileName);

#define VirtualAlloc_Hash 0x5E893462
typedef LPVOID(__stdcall* _VirtualAlloc)(LPVOID lpAddress,        // region to reserve or commit
	SIZE_T dwSize,           // size of region
	DWORD flAllocationType,  // type of allocation
	DWORD flProtect          // type of access protection
	);

#define VirtualFree_Hash 0x6488073
typedef BOOL(__stdcall* _VirtualFree)(LPVOID lpAddress,   // address of region
	SIZE_T dwSize,      // size of region  
	DWORD dwFreeType    // operation type
	);

#define CloseHandle_Hash 0x698402C
typedef BOOL(__stdcall* _CloseHandle)(
	_In_ _Post_ptr_invalid_ HANDLE hObject
	);

#define Sleep_Hash 0x3F858053
typedef VOID(__stdcall* _Sleep)(
	_In_ DWORD dwMilliseconds
	);

#define OutputDebugStringA_Hash 0x457DD222
typedef VOID(__stdcall* _OutputDebugStringA)(
	_In_opt_ LPCSTR lpOutputString
	);

#define lstrlenW_Hash 0x62C9CFDF
typedef int(__stdcall* _lstrlenW)(
	_In_ LPCWSTR lpString
	);

#define lstrlenA_Hash 0x62C9CFC9
typedef int(__stdcall* _lstrlenA)(
	_In_ LPCSTR lpString
);

#define GetComputerNameA_Hash 0x7B8E4C5
typedef BOOL
(__stdcall* _GetComputerNameA)(
	_Out_writes_to_opt_(*nSize, *nSize + 1) LPSTR lpBuffer,
	_Inout_ LPDWORD nSize
);

#define GetTickCount64_Hash 0x33D747F2
typedef ULONGLONG
(__stdcall* _GetTickCount64)(
	VOID
);

#define CreateThread_Hash 0x9515A5A
typedef
_Ret_maybenull_
HANDLE
(__stdcall* _CreateThread)(
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ SIZE_T dwStackSize,
	_In_ LPTHREAD_START_ROUTINE lpStartAddress,
	_In_opt_ __drv_aliasesMem LPVOID lpParameter,
	_In_ DWORD dwCreationFlags,
	_Out_opt_ LPDWORD lpThreadId
);

//user32
#define SetProcessDPIAware_Hash 0xFF33772
typedef BOOL(__stdcall* _SetProcessDPIAware)(
	VOID);

#define GetSystemMetrics_Hash 0x1028C0F2
typedef int (__stdcall* _GetSystemMetrics)(
	_In_ int nIndex);

#define GetLastInputInfo_Hash 0x99056FC
typedef
BOOL
(__stdcall* _GetLastInputInfo)(
	_Out_ PLASTINPUTINFO plii);

//msvcrt
#define memcpy_Hash 0x12F23F2F
typedef void* (__cdecl* _memcpy)(void* _Dst, void const* _Src, size_t _Size);

#define malloc_Hash 0x4C9B7308
typedef void* (__cdecl* _malloc)(
	size_t _Size
	);

#define free_Hash 0xDC8F9D8
typedef void(__cdecl* _free)(
	void* _Block
	);

#define memset_Hash 0x12F66A19
typedef void* (__cdecl* _memset)(
	_Out_writes_bytes_all_(_Size) void* _Dst,
	_In_                          int    _Val,
	_In_                          size_t _Size
	);

//winhttp
#define WinHttpOpen_Hash 0x40E50912
typedef HINTERNET(__stdcall* _WinHttpOpen)(
	_In_opt_z_ LPCWSTR pszAgentW,
	_In_ DWORD dwAccessType,
	_In_opt_z_ LPCWSTR pszProxyW,
	_In_opt_z_ LPCWSTR pszProxyBypassW,
	_In_ DWORD dwFlags
	);

#define WinHttpReceiveResponse_Hash 0x2AC8C650
typedef BOOL(__stdcall* _WinHttpReceiveResponse)
(
	IN HINTERNET hRequest,
	IN LPVOID lpReserved
	);

#define WinHttpReadData_Hash 0x1BF52ADC
typedef BOOL(__stdcall* _WinHttpReadData)
(
	IN HINTERNET hRequest,
	_Out_writes_bytes_to_(dwNumberOfBytesToRead, *lpdwNumberOfBytesRead) __out_data_source(NETWORK) LPVOID lpBuffer,
	IN DWORD dwNumberOfBytesToRead,
	OUT LPDWORD lpdwNumberOfBytesRead
	);

#define WinHttpQueryHeaders_Hash 0x3C642D46
typedef BOOL(__stdcall* _WinHttpQueryHeaders)
(
	IN     HINTERNET hRequest,
	IN     DWORD     dwInfoLevel,
	IN     LPCWSTR   pwszName OPTIONAL,
	_Out_writes_bytes_to_opt_(*lpdwBufferLength, *lpdwBufferLength) __out_data_source(NETWORK) LPVOID lpBuffer,
	IN OUT LPDWORD   lpdwBufferLength,
	IN OUT LPDWORD   lpdwIndex OPTIONAL
	);

#define WinHttpOpenRequest_Hash 0x6E7C3319
typedef HINTERNET(__stdcall* _WinHttpOpenRequest)
(
	IN HINTERNET hConnect,
	IN LPCWSTR pwszVerb,
	IN LPCWSTR pwszObjectName,
	IN LPCWSTR pwszVersion,
	IN LPCWSTR pwszReferrer OPTIONAL,
	IN LPCWSTR FAR* ppwszAcceptTypes OPTIONAL,
	IN DWORD dwFlags
	);

#define WinHttpSetOption_Hash 0x35D1D383
typedef BOOL(__stdcall* _WinHttpSetOption)(
	_In_opt_ HINTERNET hInternet,
	_In_ DWORD dwOption,
	_When_((dwOption == WINHTTP_OPTION_USERNAME ||
		dwOption == WINHTTP_OPTION_PASSWORD ||
		dwOption == WINHTTP_OPTION_PROXY_USERNAME ||
		dwOption == WINHTTP_OPTION_PROXY_PASSWORD ||
		dwOption == WINHTTP_OPTION_USER_AGENT),
		_At_((LPCWSTR)lpBuffer, _In_reads_(dwBufferLength)))
	_When_((dwOption == WINHTTP_OPTION_CLIENT_CERT_CONTEXT),
		_In_reads_bytes_opt_(dwBufferLength))
	_When_((dwOption != WINHTTP_OPTION_USERNAME &&
		dwOption != WINHTTP_OPTION_PASSWORD &&
		dwOption != WINHTTP_OPTION_PROXY_USERNAME &&
		dwOption != WINHTTP_OPTION_PROXY_PASSWORD &&
		dwOption != WINHTTP_OPTION_CLIENT_CERT_CONTEXT &&
		dwOption != WINHTTP_OPTION_USER_AGENT),
		_In_reads_bytes_(dwBufferLength))
	LPVOID lpBuffer,
	_In_ DWORD dwBufferLength
	);

#define WinHttpCloseHandle_Hash 0x6E4D5C3C
typedef BOOL(__stdcall* _WinHttpCloseHandle)
(
	IN HINTERNET hInternet
	);

#define WinHttpSendRequest_Hash 0x27378CB7
typedef BOOL(__stdcall* _WinHttpSendRequest)
(
	IN HINTERNET hRequest,
	_In_reads_opt_(dwHeadersLength) LPCWSTR lpszHeaders,
	IN DWORD dwHeadersLength,
	_In_reads_bytes_opt_(dwOptionalLength) LPVOID lpOptional,
	IN DWORD dwOptionalLength,
	IN DWORD dwTotalLength,
	IN DWORD_PTR dwContext
	);

#define WinHttpConnect_Hash 0xCFD5CCA
typedef HINTERNET(__stdcall* _WinHttpConnect)
(
	IN HINTERNET hSession,
	IN LPCWSTR pswzServerName,
	IN INTERNET_PORT nServerPort,
	IN DWORD dwReserved
	);

#define WinHttpCrackUrl_Hash 0x3DB9CB21
typedef BOOL(__stdcall* _WinHttpCrackUrl)
(
	_In_reads_(dwUrlLength) LPCWSTR pwszUrl,
	_In_ DWORD dwUrlLength,
	_In_ DWORD dwFlags,
	_Inout_ LPURL_COMPONENTS lpUrlComponents
	);

#define WinHttpAddRequestHeaders_Hash 0x6A412CAC
typedef BOOL(__stdcall* _WinHttpAddRequestHeaders)
(
	IN HINTERNET hRequest,
	_When_(dwHeadersLength == (DWORD)-1, _In_z_)
	_When_(dwHeadersLength != (DWORD)-1, _In_reads_(dwHeadersLength))
	LPCWSTR lpszHeaders,
	IN DWORD dwHeadersLength,
	IN DWORD dwModifiers
	);

#define CreateDCW_Hash 0x543DFD18
typedef HDC(__stdcall* _CreateDCW)(_In_opt_ LPCWSTR pwszDriver, _In_opt_ LPCWSTR pwszDevice, _In_opt_ LPCWSTR pszPort, _In_opt_ CONST DEVMODEW* pdm);

#define CreateCompatibleDC_Hash 0x3288E377
typedef HDC(__stdcall* _CreateCompatibleDC)(_In_opt_ HDC hdc);

#define CreateCompatibleBitmap_Hash 0x3239F2EB
typedef HBITMAP(__stdcall* _CreateCompatibleBitmap)(_In_ HDC hdc, _In_ int cx, _In_ int cy);

#define SelectObject_Hash 0x40EDCEC3
typedef HGDIOBJ(__stdcall* _SelectObject)(_In_ HDC hdc, _In_ HGDIOBJ h);

#define GetObjectW_Hash 0x1C857DD2
typedef int(__stdcall* _GetObjectW)(_In_ HANDLE h, _In_ int c, _Out_writes_bytes_opt_(c) LPVOID pv);

#define BitBlt_Hash 0x1AD7E525
typedef BOOL(__stdcall* _BitBlt)(_In_ HDC hdc, _In_ int x, _In_ int y, _In_ int cx, _In_ int cy, _In_opt_ HDC hdcSrc, _In_ int x1, _In_ int y1, _In_ DWORD rop);

#define DeleteObject_Hash 0x4A77AB92
typedef BOOL(__stdcall* _DeleteObject)(_In_ HGDIOBJ ho);

#define DeleteDC_Hash 0x19DD01DA
typedef BOOL(__stdcall* _DeleteDC)(_In_ HDC hdc);

//ole32
#define CoInitializeEx_Hash 0x54273423
typedef HRESULT(__stdcall* _CoInitializeEx)(
	_In_opt_ LPVOID pvReserved,
	_In_ DWORD dwCoInit);

#define CoCreateInstance_Hash 0x2235659
typedef HRESULT(__stdcall* _CoCreateInstance)(
	_In_ REFCLSID rclsid,
	_In_opt_ LPUNKNOWN pUnkOuter,
	_In_ DWORD dwClsContext,
	_In_ REFIID riid,
	_COM_Outptr_ _At_(*ppv, _Post_readable_size_(_Inexpressible_(varies))) LPVOID  FAR* ppv
);

typedef struct Func {
	// kernel32
	_GetProcAddress GetProcAddress;
	_LoadLibraryA LoadLibraryA;
	_CloseHandle CloseHandle;
	_Sleep Sleep;
	_OutputDebugStringA OutputDebugStringA;
	_lstrlenW lstrlenW;
	_lstrlenA lstrlenA;
	_GetComputerNameA GetComputerNameA;
	_GetTickCount64 GetTickCount64;
	_CreateThread CreateThread;
	//user32
	_SetProcessDPIAware SetProcessDPIAware;
	_GetSystemMetrics GetSystemMetrics;
	_GetLastInputInfo GetLastInputInfo;
	//msvcrt
	_memcpy memcpy;
	_malloc malloc;
	_free free;
	_memset memset;
	//winhttp
	_WinHttpOpen WinHttpOpen;
	_WinHttpReceiveResponse WinHttpReceiveResponse;
	_WinHttpReadData WinHttpReadData;
	_WinHttpQueryHeaders WinHttpQueryHeaders;
	_WinHttpOpenRequest WinHttpOpenRequest;
	_WinHttpSetOption WinHttpSetOption;
	_WinHttpCloseHandle WinHttpCloseHandle;
	_WinHttpSendRequest WinHttpSendRequest;
	_WinHttpConnect WinHttpConnect;
	_WinHttpCrackUrl WinHttpCrackUrl;
	_WinHttpAddRequestHeaders WinHttpAddRequestHeaders;
	//gdi32
	_CreateDCW CreateDCW;
	_CreateCompatibleDC CreateCompatibleDC;
	_CreateCompatibleBitmap CreateCompatibleBitmap;
	_SelectObject SelectObject;
	_GetObjectW GetObjectW;
	_BitBlt BitBlt;
	_DeleteObject DeleteObject;
	_DeleteDC DeleteDC;
	//ole32
	_CoInitializeEx CoInitializeEx;
	_CoCreateInstance CoCreateInstance;

} func_t, * func_p;


#ifdef RtlZeroMemory
# undef RtlZeroMemory
#endif  // RtlZeroMemory

#ifdef RtlMoveMemory
# undef RtlMoveMemory
#endif  // RtlMoveMemory

HMODULE get_kernel32_base();
uint32_t calc_hashW2(wchar_t* str, int len);
uint32_t calc_hash(char* str);
void* get_proc_address_from_hash(HMODULE module, uint32_t func_hash, _GetProcAddress get_proc_address);