/*
 * AgentMain.c
 *
 *  Created on: 24 nov 2017
 *      Author: Giulio Centrella
 */

#include "global.h"
#include <sdkddkver.h>
#include "preshut.h"

#define _WIN32_WINNT_WINTHRESHOLD           0x0A00 // Windows 10
#define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10

#ifndef SERVICE_CONTROL_PRESHUTDOWN
#define SERVICE_CONTROL_PRESHUTDOWN 0x0000000F
#endif
#ifndef SERVICE_ACCEPT_PRESHUTDOWN
#define SERVICE_ACCEPT_PRESHUTDOWN 0x00000100
#endif

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE ServiceStatusHandle;

int WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
DWORD WINAPI HandlerEx(DWORD CtrlCode, DWORD eventType, LPVOID eventData,
		LPVOID context);
void ReconfigureMyService(BOOL fDisable, LPSTR lpDesc);
void ConfigPreshut(BOOL fDisable);

static LPTSTR g_lpszServiceName = "AgentService";
SC_HANDLE schSCManager, schService;
SERVICE_PRESHUTDOWN_INFO info;

FILE *fprocess, *fprocess2;
char filename[] = "processRunning.txt";

int main() {
//	SERVICE_TABLE_ENTRY DispatchTable[] = { { g_lpszServiceName, ServiceMain }, {
//	NULL, NULL } };

	SERVICE_TABLE_ENTRY DispatchTable[2];

	DispatchTable[0].lpServiceName = g_lpszServiceName;
	DispatchTable[0].lpServiceProc = ServiceMain;
	DispatchTable[1].lpServiceName = NULL;
	DispatchTable[1].lpServiceProc = NULL;

	if (!StartServiceCtrlDispatcher(DispatchTable)) {
		printf("StartServiceCtrlDispatcher() failed, error: %ld.\n",
				GetLastError());
	} else {
		printf("StartServiceCtrlDispatcher() looks OK.\n");
	}

	return 0;
}

void ConfigPreshut(BOOL fDisable) {

	info.dwPreshutdownTimeout = 130000;

// Open a handle to the SC Manager database...
	schSCManager = OpenSCManager(
	NULL,                    // local machine
			NULL,      // SERVICES_ACTIVE_DATABASE database is opened by default
			SC_MANAGER_ALL_ACCESS);  // full access rights

	// Open a handle to the service.
	schService = OpenService(schSCManager,           // SCManager database
			g_lpszServiceName,        // name of service
			SERVICE_CHANGE_CONFIG); // need CHANGE access

	ChangeServiceConfig2(schService,                 // handle to service
			SERVICE_CONFIG_PRESHUTDOWN_INFO, // change: description
			&info);                      // value: new description
	// Release the database lock.

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return;

}

void ReconfigureMyService(BOOL fDisable, LPSTR lpDesc) {

	SC_LOCK sclLock;
	LPQUERY_SERVICE_LOCK_STATUS lpqslsBuf;
	SERVICE_DESCRIPTION sdBuf;
	DWORD dwBytesNeeded, dwStartType;

// Open a handle to the SC Manager database...
	schSCManager = OpenSCManager(
	NULL,                    // local machine
			NULL,      // SERVICES_ACTIVE_DATABASE database is opened by default
			SC_MANAGER_ALL_ACCESS);  // full access rights

	// Need to acquire database lock before reconfiguring.
	sclLock = LockServiceDatabase(schSCManager);
	// If the database cannot be locked, report the details.
	if (sclLock == NULL) {
		// Exit if the database is not locked by another process.
		if (GetLastError() != ERROR_SERVICE_DATABASE_LOCKED)
			printf("LockServiceDatabase() failed, error: %d.\n",
					GetLastError());
		else
			printf("LockServiceDatabase() is OK.\n");
		// Allocate a buffer to get details about the lock.
		lpqslsBuf = (LPQUERY_SERVICE_LOCK_STATUS) LocalAlloc(
		LPTR, sizeof(QUERY_SERVICE_LOCK_STATUS) + 256);
		if (lpqslsBuf == NULL)
			printf("LocalAlloc() failed, error: %d.\n", GetLastError());
		else
			printf("LocalAlloc() is OK.\n");
		// Get and print the lock status information.
		if (!QueryServiceLockStatus(schSCManager, lpqslsBuf,
				sizeof(QUERY_SERVICE_LOCK_STATUS) + 256, &dwBytesNeeded))
			printf("QueryServiceLockStatus() failed, error: %d.\n",
					GetLastError());
		else
			printf("QueryServiceLockStatus() looks OK.\n");
		if (lpqslsBuf->fIsLocked)
			printf("Locked by: %s, duration: %d seconds\n",
					lpqslsBuf->lpLockOwner, lpqslsBuf->dwLockDuration);
		else
			printf("No longer locked.\n");
		LocalFree(lpqslsBuf);
		printf("Could not lock the database.\n");
	}
	// The database is locked, so it is safe to make changes.
	// Open a handle to the service.
	schService = OpenService(schSCManager,           // SCManager database
			g_lpszServiceName,        // name of service
			SERVICE_CHANGE_CONFIG); // need CHANGE access

	dwStartType = (fDisable) ? SERVICE_DISABLED : SERVICE_DEMAND_START;
	// Make the changes.
	ChangeServiceConfig(schService,        // handle of service
			SERVICE_NO_CHANGE, // service type: no change
			dwStartType,       // change service start type
			SERVICE_NO_CHANGE, // error control: no change
			NULL,              // binary path: no change
			NULL,              // load order group: no change
			NULL,              // tag ID: no change
			NULL,              // dependencies: no change
			NULL,              // account name: no change
			NULL,              // password: no change
			NULL);             // display name: no change

	sdBuf.lpDescription = lpDesc;

	ChangeServiceConfig2(schService,                 // handle to service
			SERVICE_CONFIG_DESCRIPTION, // change: description
			&sdBuf);                      // value: new description

	ChangeServiceConfig(schService,        // handle of service
			SERVICE_NO_CHANGE, // service type: no change
			SERVICE_AUTO_START,       // change service start type
			SERVICE_NO_CHANGE, // error control: no change
			NULL,              // binary path: no change
			NULL,              // load order group: no change
			NULL,              // tag ID: no change
			NULL,              // dependencies: no change
			NULL,              // account name: no change
			NULL,              // password: no change
			NULL);             // display name: no change

	// Release the database lock.
	UnlockServiceDatabase(sclLock);

	// Close the handle to the service.
	CloseServiceHandle(schService);

	return;
}

int WINAPI ServiceMain(DWORD argc, LPTSTR *argv) {

	BOOL fDisable = FALSE;
	LPSTR lpDesc = "This is a MalwareServiceAgent ";

	ReconfigureMyService(fDisable, lpDesc);

	ConfigPreshut(fDisable);

	DWORD status;

// Type of service, application or driver...
	ServiceStatus.dwServiceType = SERVICE_WIN32;

// The service is starting...
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;

// The service can be stopped & can be paused and continued.
	ServiceStatus.dwControlsAccepted =  SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_POWEREVENT
			| SERVICE_ACCEPT_PRESHUTDOWN;
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;

	ServiceStatusHandle = RegisterServiceCtrlHandlerEx(g_lpszServiceName,
			(LPHANDLER_FUNCTION_EX) HandlerEx, NULL);

	if (ServiceStatusHandle == (SERVICE_STATUS_HANDLE) 0) {

		printf("RegisterServiceCtrlHandler() failed, error: %ld.\n",
				GetLastError());
		return 0;
	} else
		printf("RegisterServiceCtrlHandler() looks OK.\n");

// Handle error condition
	if (status != NO_ERROR) {

// The service is not running...
		ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwWaitHint = 0;
		ServiceStatus.dwWin32ExitCode = status;
		//ServiceStatus.dwServiceSpecificExitCode = specificError;
		SetServiceStatus(ServiceStatusHandle, &ServiceStatus);
		return 0;
	}

// Initialization complete - report running status.
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;

	if (!SetServiceStatus(ServiceStatusHandle, &ServiceStatus)) {
		status = GetLastError();
		printf("SetServiceStatus() error: %ld\n", status);
	} else
		printf("SetServiceStatus() looks OK.\n");

// This is where the service does its work...

	while (ServiceStatus.dwCurrentState == SERVICE_RUNNING) {

		time_t rawtime;
		struct tm * timeinfo;
		int th2, th3, th4;
		BOOL firstAccess = FALSE;

		printf("---Start thread Snap Process---\n");
		printf("---Start thread Snap Ip/Url---\n");

		fprocess = fopen(filename, "w");
		if (fprocess == NULL) {
			perror("Errore in apertura del file processi");
			exit(1);
		}

		fprocess2 = fopen(filename, "a");
		if (fprocess2 == NULL) {
			perror("Errore in apertura del file processi");
			exit(1);
		}

		//first shot
		printf("---First RegistryShot---\n");
		th_regControl();

		//snap_Ip
		th_snapIP_URL();

		while (TRUE) {

			time(&rawtime);
			timeinfo = localtime(&rawtime);
			printf("Current local time and date: %s\n", asctime(timeinfo));
			fflush(stdout);

//			if (timeinfo->tm_min % 4 == 0 && timeinfo->tm_sec == 00) {
			if (timeinfo->tm_hour == 19 && timeinfo->tm_min == 00
					&& timeinfo->tm_sec == 00) {
				printf("---Start thread RegShot---\n");
				if ((th3 = pthread_create(&thread[2], NULL, th_regControl, NULL))) {
					printf("Error; can't creat thread: %s\n", strerror(th3));
					return EXIT_FAILURE;
				}
			}

			th_snapProcess(fprocess2);

//			if (firstAccess == FALSE) {
//				th_snapProcess(fprocess);
//			} else {
//				th_snapProcess(fprocess2);
//			}

			//Thread Snap_IP/Url temporizzata ogni 3 minuti
			if (timeinfo->tm_min % 1 == 0 && timeinfo->tm_sec == 00) {
				if ((th2 = pthread_create(&thread[1], NULL, th_snapIP_URL, NULL))) {
					printf("Error; can't creat thread: %s\n", strerror(th2));
					return EXIT_FAILURE;
				}
			}

			//Thread Socket
//			if (timeinfo->tm_min % 5 == 0 && timeinfo->tm_sec == 00) {
			if (timeinfo->tm_hour == 20 && timeinfo->tm_min == 00
					&& timeinfo->tm_sec == 00) {
				printf("---Start thread Socket---\n");
				if ((th4 = pthread_create(&thread[3], NULL, th_sockClient, NULL))) {
					printf("Error; can't creat thread: %s\n", strerror(th4));
					return EXIT_FAILURE;
				}
			}

			/* block until all threads complete */
			for (int i = 0; i < 4; ++i) {
				pthread_join(thread[i], NULL);

			}

			firstAccess = TRUE;
			sleep(1);


		}

		return EXIT_SUCCESS;

	}
	return 0;
}

// Handler function - receives Opcode, calls SetServiceStatus()
DWORD WINAPI HandlerEx(DWORD CtrlCode, DWORD eventType, LPVOID eventData,
		LPVOID context) {

	DWORD status;

	switch (CtrlCode) {

	case SERVICE_CONTROL_PAUSE: {

// Do whatever it takes to pause here...
		ServiceStatus.dwCurrentState = SERVICE_RUNNING;

		break;
	}

	case SERVICE_CONTROL_CONTINUE: {

// Do whatever it takes to continue here...
		ServiceStatus.dwCurrentState = SERVICE_RUNNING;

		break;
	}
//	case SERVICE_CONTROL_STOP: {
//
//// Do whatever it takes to stop here...
//
//		ServiceStatus.dwWin32ExitCode = 0;
//		ServiceStatus.dwCurrentState = SERVICE_RUNNING;
//		ServiceStatus.dwCheckPoint = 0;
//		ServiceStatus.dwWaitHint = 0;
//
//		if (!SetServiceStatus(ServiceStatusHandle, &ServiceStatus)) {
//
//			status = GetLastError();
//			printf("[MY_SERVICE] SetServiceStatus() error: %ld\n", status);
//		}
//		th_regControl();
//		th_sockClient();
//
//		printf("Leaving MyService.\n");
//		break;
//	}

	case SERVICE_CONTROL_INTERROGATE:

// Fall through to send current status.
		break;

//	case SERVICE_CONTROL_POWEREVENT: {
//		if (eventType == PBT_APMSUSPEND) {
//			//Sleep(dwTimeout);
//
//
//			th_regControl();
//			th_sockClient();
//
//			ServiceStatus.dwWin32ExitCode = 0;
//			ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
//			//ServiceStatus.dwWaitHint=60000;
//			SetServiceStatus(ServiceStatusHandle, &ServiceStatus);
//
//
//
//		}
//		return NO_ERROR;
//	}

	case SERVICE_CONTROL_PRESHUTDOWN: {

		ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus(ServiceStatusHandle, &ServiceStatus);

		th_regControl();
		th_sockClient();

		return NO_ERROR;
	}

	default:
// else
		printf("Unrecognized opcode %ld.\n", CtrlCode);
	}

// Send current status.
	if (!SetServiceStatus(ServiceStatusHandle, &ServiceStatus)) {

		status = GetLastError();
		printf("SetServiceStatus error %ld.\n", status);
		return NO_ERROR;
	}

	else
		printf("SetServiceStatus() is OK.\n");

	return NO_ERROR;
}
