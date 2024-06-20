#include <Windows.h>
#include <iostream>

typedef struct _PEB_LDR_DATA {
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _PEB {
	BOOLEAN InheritedAddressSpace;
	BOOLEAN ReadImageFileExecOptions;
	BOOLEAN BeingDebugged;
	BOOLEAN BitField;
	HANDLE Mutant;
	PVOID ImageBaseAddress;
	PPEB_LDR_DATA Ldr;
} PEB, * PPEB;

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

void PEBRedirect(LPCWSTR currentDllName, LPCWSTR newDllPath) {
#if _WIN64
	PPEB pPeb = (PPEB)__readgsqword(0x60);
#else
	PPEB pPeb = (PPEB)__readfsdword(0x30);
#endif
	PPEB_LDR_DATA pLdrData = pPeb->Ldr;
	PLIST_ENTRY pListEntry = &pLdrData->InLoadOrderModuleList;
	for (PLIST_ENTRY pEntry = pListEntry->Flink; pEntry != pListEntry; pEntry = pEntry->Flink)
	{
		PLDR_DATA_TABLE_ENTRY pLdrEntry = CONTAINING_RECORD(pEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if (wcsstr(pLdrEntry->FullDllName.Buffer, currentDllName))
		{
			LPVOID ModuleHandle = LoadLibraryW(newDllPath);
			PIMAGE_DOS_HEADER piDH_Custom = (PIMAGE_DOS_HEADER)(DWORD_PTR)ModuleHandle;
			PIMAGE_NT_HEADERS piNH_Custom = (PIMAGE_NT_HEADERS)((DWORD_PTR)ModuleHandle + piDH_Custom->e_lfanew);
			DWORD g_CustomSize = piNH_Custom->OptionalHeader.SizeOfImage;
			PVOID g_CustomEntryPoint = (PVOID)piNH_Custom->OptionalHeader.AddressOfEntryPoint;
			pLdrEntry->DllBase = (PVOID)ModuleHandle;
			pLdrEntry->EntryPoint = g_CustomEntryPoint;
			pLdrEntry->SizeOfImage = g_CustomSize;
			break;
		}
	}
}

int main()
{
	printf("Current kernel32.dll address: 0x%p\n", GetModuleHandleA("kernel32.dll"));
	printf("Current ntdll.dll address: 0x%p\n", GetModuleHandleA("ntdll.dll"));
	PEBRedirect(L"ntdll.dll", L"kernel32.dll"); //Redirect/Swap ntdll.dll with kernel32.dll
	printf("New ntdll.dll address: 0x%p\n", GetModuleHandleA("ntdll.dll")); //Should match kernel32.dll address after swap.
	system("pause");
	exit(0);
}