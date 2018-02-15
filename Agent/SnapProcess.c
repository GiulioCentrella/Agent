/*
 * SnapProcess.c
 *
 *  Created on: 23 nov 2017
 *      Author: Giulio Centrella
 */

#include "global.h"
#include <shellapi.h>

char buf[1024];

unsigned long hashCode(char* key) {

	unsigned long hash = 0;
	int c;

	while (c = *key++)
		hash += c;

	return hash;
}

struct DataItem *search(char* key) {
	//get the hash
	int hashIndex = hashCode(key);

	//move in array until an empty
	while (hashArray[hashIndex] != NULL) {

		if (hashArray[hashIndex]->key == key)
			return hashArray[hashIndex];

		//go to next cell
		++hashIndex;

		//wrap around the table
		hashIndex %= SIZE;
	}

	return NULL;
}


void insert(char* key, int data) {

	struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
	item->data = data;
	item->key = key;

	//get the hash
	int hashIndex = hashCode(key);

	//move in array until an empty or deleted cell
	while (hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != "\0") {
		//go to next cell
		++hashIndex;

		//wrap around the table
		hashIndex %= SIZE;
	}

	hashArray[hashIndex] = item;
}

void display() {
	int i;

	if (hashArray[i] != NULL) {
		printf(" (%s,%d)", hashArray[i]->key, hashArray[i]->data);
	} else
		printf("  ");

	printf("\n");
}

void *th_snapProcess(FILE *fprocess) {


	dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
	dummyItem->data = -1;
	dummyItem->key = "\0";

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//snapshot all process
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		_tprintf(_T("CreateToolhelp32Snapshot error \n"));
		exit(EXIT_FAILURE);
	}
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	//retrieve the process information
	if (!Process32First(hProcessSnap, &pe32)) {
		_tprintf(_T("Process32First error ! \n"));
		CloseHandle(hProcessSnap);
		exit(EXIT_FAILURE);
	}

	//print all process information, add process in file if not present in hashmap, insert in hashmap

	while (Process32Next(hProcessSnap, &pe32)) {

		if (search(pe32.szExeFile) == NULL) {
			insert(pe32.szExeFile, pe32.th32ProcessID);
			int off = -1;
			off = ftell(fprocess);
			off = fseek(fprocess, off, SEEK_SET);
			fprintf(fprocess, "%s  ", pe32.szExeFile);
			fprintf(fprocess, " %d\n", pe32.th32ProcessID);

			//commandLine tramite pid(handle)
			//system("wmic.exe path Win32_Process where handle='600' get name, commandline  /format:list");

			//get CommandLine process
			snprintf(buf, sizeof(buf), "wmic.exe path Win32_Process where name='%s' get commandline, name /format:list >> CommandLine.txt", pe32.szExeFile);
			system(buf);
		}

	}

}

