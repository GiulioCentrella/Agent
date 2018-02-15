/*
 * SnapModul.c
 *
 *  Created on: 09 feb 2018
 *      Author: admin
 */

#include "global.h"

FILE *fModulo;
void printError(TCHAR* msg);


BOOL *th_SnapModul(FILE *fprocess,DWORD dwPid) {

		fModulo = fopen("Moduli.txt", "a");
					if (fModulo == NULL) {
						perror("Errore in apertura del file processi");
						exit(1);
					}


	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	// Take a snapshot of all modules in the specified process.
	hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPid);

	sample();

	//SetPrivilege(hProcessSnap,(const char *) TEXT(&pe32.szExeFile),true );

	if (hModuleSnap == INVALID_HANDLE_VALUE) {
		printError(TEXT("CreateToolhelp32Snapshot (of modules)"));
		return ( FALSE);
	}

	// Set the size of the structure before using it.
	me32.dwSize = sizeof(MODULEENTRY32);

	// Retrieve information about the first module,
	// and exit if unsuccessful
	if (!Module32First(hModuleSnap, &me32)) {
		printError(TEXT("Module32First"));  // show cause of failure
		CloseHandle(hModuleSnap);           // clean the snapshot object
		return ( FALSE);
	}

	// Now walk the module list of the process,
	// and display information about each module

	do {
		fprintf(fprocess, "Modul Name: %s\n", me32.szModule);
				fprintf(fprocess, "Path: %s\n", me32.szExePath);
				fprintf(fprocess, "ProcessId: %s\n\n", me32.th32ProcessID);


		} while (Module32Next(hModuleSnap, &me32));

	//fclose(fModulo);

	CloseHandle(hModuleSnap);
	return ( TRUE);
}

void printError(TCHAR* msg) {
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError();
	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	NULL, eNum, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	sysMsg, 256, NULL);

	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while ((*p > 31) || (*p == 9))
		++p;
	do {
		*p-- = 0;
	} while ((p >= sysMsg) && ((*p == '.') || (*p < 33)));

	// Display the message
	_tprintf(TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum,
			sysMsg);
}

void sample() {
	/////////////////////////////////////////////////////////
	//   Note: Enabling SeDebugPrivilege adapted from sample
	//     MSDN @ http://msdn.microsoft.com/en-us/library/aa446619%28VS.85%29.aspx
	// Enable SeDebugPrivilege
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tokenPriv;
	LUID luidDebug;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,
			&hToken) != FALSE) {
		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidDebug) != FALSE) {
			tokenPriv.PrivilegeCount = 1;
			tokenPriv.Privileges[0].Luid = luidDebug;
			tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, 0, NULL,
			NULL) != FALSE) {
				// Always successful, even in the cases which lead to OpenProcess failure
				printf("\nSUCCESSFULLY CHANGED TOKEN PRIVILEGES");
			} else {
				printf("FAILED TO CHANGE TOKEN PRIVILEGES, CODE: %u\n",
						GetLastError());
			}
		}
	}
	CloseHandle(hToken);
}
