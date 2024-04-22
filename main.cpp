// Includes
#include <iostream>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <TlHelp32.h>
#include "defines/Offsets.h"
#include "defines/DIK_Define.h"

// Namespaces
using namespace std;


uintptr_t GetModuleBaseAddress(TCHAR* lpszModuleName, uintptr_t ProcessID) {
	uintptr_t dwModuleBaseAddress = 0;
	MODULEENTRY32 ModuleEntry32 = { 0 };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessID);
	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(hSnapshot, &ModuleEntry32))
	{
		do
		{
			if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) {
				dwModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
				break;
			}
		} while (Module32Next(hSnapshot, &ModuleEntry32));
	}
	CloseHandle(hSnapshot);
	return dwModuleBaseAddress;
}

uintptr_t GetPointerAddress(HWND hwnd, uintptr_t gameBaseAddr, std::vector<uintptr_t> offsets)
{
	DWORD pID = 0;
	GetWindowThreadProcessId(hwnd, &pID);

	HANDLE phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
	if (phandle == nullptr || phandle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	uintptr_t pointerAddress = gameBaseAddr;

	for (uintptr_t offset : offsets)
	{
		if (!ReadProcessMemory(phandle, reinterpret_cast<LPVOID>(pointerAddress), &pointerAddress, sizeof(uintptr_t), nullptr))
		{
			CloseHandle(phandle);
			return 0;
		}

		pointerAddress += offset;
	}

	CloseHandle(phandle);
	return pointerAddress;
}

int main()
{
    wchar_t gameName[] = L"Counter-Strike Source";
    HWND processHandle = FindWindowW(NULL, gameName);
    DWORD processId; GetWindowThreadProcessId(processHandle, &processId);
    HANDLE openProcess = OpenProcess(PROCESS_ALL_ACCESS, true, processId);
    DWORD_PTR moduleAddress = GetModuleBaseAddress(const_cast<TCHAR*>(TEXT("client.dll")),processId);
//-----------------------Verificaciones--------------------------
    if (!processHandle)
    {
        cout << "No se pudo encontrar la ventana." << endl;
        getchar();
        return 0;
    }

    if (!processId)
    {
        cout << "No se pudo encontrar la ID del proceso." << endl;
        getchar();
        return 0;
    }

    if (!openProcess)
    {
        cout << "No se pudo abrir el proceso." << endl;
        getchar();
        return 0;
    }
    cout << "Ventana encontrada.\nID del proceso encontrado.\nProceso abierto.\nTodo correcto, iniciando cheat." << endl;
    
    //--------------------------------------------------------------- 

    // Variables
    BOOL _isTouchin;

    while(true)
    {
        ReadProcessMemory(openProcess,(PVOID)(moduleAddress + LP_ISTOUCHING),&_isTouchin,sizeof(_isTouchin),0);
        if(GetAsyncKeyState(VK_SPACE) & 0x80001 && _isTouchin)
        {
            SendMessage(processHandle,WM_KEYDOWN,DIK_F8,DIK_F8);
            Sleep(1.5);
            SendMessage(processHandle,WM_KEYUP,DIK_F8,DIK_F8);
        }
        Sleep(1);
    }
    return 1;
}