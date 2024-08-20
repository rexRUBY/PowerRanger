#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <zbar.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "192.168.75.129" // 서버의 IP 주소 (가상으로 설정)

int main() {
    // 서버 소켓 설정
    int server_socket;
    struct sockaddr_in server_addr;
    char server_ip[] = SERVER_IP;
    int server_port = 8080; // 임의의 포트 번호

    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    // 서버에 연결
    if (connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(1);
    }

    // 카메라 열기
    CvCapture* capture = cvCreateCameraCapture(0);
    if (!capture) {
        printf("카메라를 열 수 없습니다.\n");
        return -1;
    }

    // zbar 스캐너 설정
    zbar_image_scanner_t* scanner = zbar_image_scanner_create();
    zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);

    // 카메라로부터 프레임 읽기
    while (1) {
        // 프레임 읽기
        IplImage* frame = cvQueryFrame(capture);
        if (!frame) {
            printf("프레임을 읽을 수 없습니다.\n");
            break;
        }

        // 이미지 표시
        cvShowImage("카메라 영상", frame);

        // 키 입력 대기
        char c = cvWaitKey(25);
        if (c == 27 || c == 'q') // ESC 또는 'q' 키를 누르면 종료
            break;
        else if (c == ' ') { // 스페이스 바를 누르면 QR/바코드 인식
            // 그레이스케일 이미지로 변환
            IplImage* gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
            cvCvtColor(frame, gray, CV_BGR2GRAY);

            // zbar 이미지 설정
            zbar_image_t* zimage = zbar_image_create();
            zbar_image_set_format(zimage, *(int*)"Y800");
            zbar_image_set_size(zimage, gray->width, gray->height);
            zbar_image_set_data(zimage, gray->imageData, gray->width * gray->height, NULL);

            // 바코드 스캔
            zbar_scan_image(scanner, zimage);

            // 바코드 결과 출력
            const zbar_symbol_t* symbol = zbar_image_first_symbol(zimage);
            for (; symbol; symbol = zbar_symbol_next(symbol)) {
                const char* data = zbar_symbol_get_data(symbol);
                char* space_pos = strchr(data, ' ');
                if (space_pos != NULL) {
                    // 띄어쓰기를 기준으로 문자열을 나눔
                    int name_len = space_pos - data;
                    char* product_name = (char*)malloc(name_len + 1);
                    strncpy(product_name, data, name_len);
                    product_name[name_len] = '\0';

                    char* product_code = strdup(space_pos + 1);

                    // 상품명과 상품코드 출력
                    printf("상품명: %s\n", product_name);
                    printf("상품코드: %s\n", product_code);

                    // 서버로 데이터 전송
                    char send_buffer[256];
                    sprintf(send_buffer, "%s %s", product_name, product_code);
                    write(server_socket, send_buffer, strlen(send_buffer));

                    free(product_name);
                    free(product_code);
                } else {
                    // 띄어쓰기가 없으면 전체 문자열을 출력
                    printf("바코드: %s\n", data);
                }
            }

            // 메모리 해제
            cvReleaseImage(&gray);
            zbar_image_destroy(zimage);
        }
    }

    // 자원 해제
    cvReleaseCapture(&capture);
    zbar_image_scanner_destroy(scanner);
    cvDestroyWindow("카메라 영상");

    // 소켓 닫기
    close(server_socket);

    return 0;
}
