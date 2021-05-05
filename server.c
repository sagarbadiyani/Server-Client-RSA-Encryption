#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

int main(int argc, char const *argv[]){
    if(argc < 2) return 0;
    char *ip = "127.0.0.1";
    int port = (int) strtol(argv[1], (char **) NULL, 10);

    char *EXIT = "exit";

    int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket == -1){
		printf("Error in creating a server side socket.\n");
		exit(1);
	}

    struct sockaddr_in server_address, cli1, cli2;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);

    int e = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if(e < 0){
        printf("Error in binding from server side.\n");
		exit(2);
    }

    e = listen(server_socket, 0);
    if(e < 0){
        printf("Error in listening from server side.\n");
		exit(3);
    }

    int clilen = sizeof(cli1);

    while(1){
        int client_socket1 = accept(server_socket, (struct sockaddr*) &cli1, (socklen_t *)&clilen);
        int client_socket2 = accept(server_socket, (struct sockaddr*) &cli2, (socklen_t *)&clilen);

        if(client_socket1 < 0 || client_socket2 < 0){
            printf("Acceptance Error.\n");
            exit(0);
        }

        printf("Both the clients Accepted.\n");

        pid_t pid1 = -1, pid2 = -1;
        pid1 = fork();
        if(pid1 != 0){
            pid2 = fork();
        }

        if(pid1 == 0){
            close(server_socket);
            char buffer[1024];
            while(1){
                memset(buffer, 0, 1024);
                int rd = recv(client_socket1, buffer, 256, 0);
                if(rd == 0) {send(client_socket2, EXIT, sizeof(EXIT), 0); exit(0);}
                // printf("Message recvd from Client1: %s\n", buffer);
                send(client_socket2, buffer, 256, 0);
            }
        }

        else if(pid2 == 0){
            close(server_socket);
            char buffer[1024];
            while(1){
                memset(buffer, 0, 1024);
                int rd = recv(client_socket2, buffer, 256, 0);
                if(rd == 0) {send(client_socket1, EXIT, sizeof(EXIT), 0); exit(0);}
                // printf("Message recvd from Client2: %s\n", buffer);
                send(client_socket1, buffer, 256, 0);
            }
        }

        while(wait(NULL) != -1);

        printf("Both the Clients Left.\n");
    }

    close(server_socket);
    return 0;
}