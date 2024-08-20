//Server IP 192.168.63.10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    //Server IP
    char *server_ip = "192.168.63.10";


    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
	exit(EXIT_FAILURE);
    }


    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);


    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        perror("Connection Failed");
	exit(EXIT_FAILURE);
    }

    
    while(1) {
        printf("Enter message: ");
	fgets(buffer, BUFFER_SIZE, stdin);
	send(client_socket, buffer, BUFFER_SIZE, 0);
	printf("Server: %s\n", buffer);
	memset(buffer,0, sizeof(buffer));
    }

    close(client_socket);
    return 0;
}
