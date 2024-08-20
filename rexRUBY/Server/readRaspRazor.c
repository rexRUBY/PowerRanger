#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_socket, new_socket;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];

    // 서버 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 소켓 주소 구조체 초기화
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // 소켓을 포트에 바인딩
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // 클라이언트의 연결 요청을 대기하는 대기 큐의 크기 설정
    if (listen(server_socket, 10) == 0) {
        printf("Listening....\n");
    } else {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    addr_size = sizeof(new_addr);
    // 클라이언트와의 연결 수락
    new_socket = accept(server_socket, (struct sockaddr*)&new_addr, &addr_size);
    if (new_socket < 0) {
        perror("Acceptance failed");
        exit(EXIT_FAILURE);
    }

    // 클라이언트로부터 메시지 수신 후 다시 전송
    while (1) {
        recv(new_socket, buffer, BUFFER_SIZE, 0);
        printf("Client: %s\n", buffer);
        send(new_socket, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    close(new_socket);
    close(server_socket);
    return 0;
}

