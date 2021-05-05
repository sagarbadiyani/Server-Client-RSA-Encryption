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
#include <openssl/pem.h>
#include <openssl/rsa.h>

#define READ_SIZE 200
#define WRITE_SIZE 256

int main(int argc, char const *argv[]){
    if(argc < 5) return 0;
    char *ip = (char *) argv[3];
    int port = (int) strtol(argv[4], (char **) NULL, 10);
    FILE *pvtKey = fopen(argv[1], "r");
    FILE *pubKey = fopen(argv[2], "r");

    char *EXIT = "exit\n";

    RSA *privateRSA;
    RSA *publicRSA;

    char inp[READ_SIZE], encText[WRITE_SIZE], cipher[WRITE_SIZE], decText[READ_SIZE];

    privateRSA = PEM_read_RSAPrivateKey(pvtKey, NULL, NULL, NULL); 
    if(!privateRSA){
        printf("Private Key Error.\n");
        return 0;
    }

    publicRSA = PEM_read_RSA_PUBKEY(pubKey, NULL, NULL, NULL);
    if(!publicRSA){
        printf("Public Key Error.\n");
        return 0;
    }

    fclose(pvtKey);
    fclose(pubKey);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket == -1){
		printf("Error in creating a client side socket.\n");
		exit(1);
	}

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);

    int con_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if (con_status < 0){
		printf("There was an error making the connection.\n");
        exit(2);
	}

    pid_t pid = fork();

    if(pid == 0){
        printf("Enter String: ");
        fflush(stdout);
        // char buffer[1024];
        memset(inp, 0, READ_SIZE);
        memset(encText, 0, WRITE_SIZE);
        while(1){
            int rd;
            while((rd = read(0, inp, READ_SIZE))){
                if(rd < 0){
                    printf("Error in Reading.\n");
                    exit(0);
                }
                int en = RSA_public_encrypt(rd, inp, encText, publicRSA, RSA_PKCS1_PADDING);
                send(client_socket, encText, en, 0);
                // printf("Batao bhai: %s\n", inp);
                if(strlen(inp) >= 4){
                    
                    if(strcmp(inp, EXIT) == 0){
                        kill(getppid(), SIGKILL);
                        exit(0);
                    }
                }
                
                memset(inp, 0, READ_SIZE);
                memset(encText, 0, WRITE_SIZE);
            }
        }
    }
    else{
        while(1){
            memset(cipher, 0, WRITE_SIZE);
            memset(decText, 0, READ_SIZE);
            int wr = recv(client_socket, cipher, WRITE_SIZE, 0);

            if(!wr){
                kill(pid, SIGKILL);
                exit(0);
            }

            printf("Cipher Text received: ");
            int i = 0;
            while(i<256){
                printf("%c", cipher[i]);
                i++;
            }
            printf("\n");
            int dc = RSA_private_decrypt(wr, cipher, decText, privateRSA, RSA_PKCS1_PADDING);
            printf("Decrypted Text is: %s\n", decText);

            if(strcmp(decText, EXIT) == 0){
                kill(pid, SIGKILL);
                exit(0);
            }
        }
        
    }

    return 0;
}