#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#define PROCESS_1 1
#define PROCESS_2 2
char ERROR[] = "404";
char SUCCESS[] = "200";

int client_sock;
char buffer[1024], recv_data[1024];
struct sockaddr_in server_addr;
int bytes_sent, bytes_received;

#include "./client.h"

int diem = 0;
int check = 0;
int check1 = 0;

int main() {
    int a, i, login,j;
    int process;
    char user_id[30], passwd[30];
    int oso;
    char cau_tra_loi[30];
    int is_player;

    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5500);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) != 0) {
        printf("\nError!Can not connect to sever!Client exit imediately!\n");
        return 0;
    }

    bytes_received = recv(client_sock, recv_data, 1024, 0);
    if (bytes_received == -1) {
        printf("\nError!Cannot receive data from sever!\n");
        close(client_sock);
        exit(-1);
    }
    recv_data[bytes_received] = '\0';
    puts(recv_data);

    process = PROCESS_1;
    while (1) {
        if (process == PROCESS_1) {
            printf("\n#########################\n");
            printf("### Chiec non ki dieu ###\n");
            printf("#########################\n\n");
            login = 0;
            do {
                printf("\n1. Dang ky.\n");
                printf("2. Dang nhap.\n");
                printf("3. Thoat.\n");
                printf("Chon: ");
                scanf("%d%*c", &a);
                switch (a) {
                    case 1:
                        sign_in(user_id, passwd);
                        break;
                    case 2:
                        login = log_in(user_id, passwd);
                        if (login == 0) exit(-1);
                        process = PROCESS_2;
                        break;
                    case 3:
                        exit(-1);
                    default:
                        printf("Hay dang nhap de tham gia tro choi!.\n");
                        break;
                }
            } while (login == 0);
        } else if (process == PROCESS_2) {
            process2();
            for (j = 0; j < strlen(o_chu); j++) {
                trang_thai_o_chu[j] = '*';
            }
            process = PROCESS_3;
        } else if (process == 3) {
          //  is_player = 0;
            // is_player = process3();
            bytes_sent = send(client_sock, "...", 3, 0);
            if (bytes_sent == -1) {
                printf("\nError!Cannot send data to sever!\n");
                close(client_sock);
                exit(-1);
            }

            bytes_received = recv(client_sock, recv_data, 1024, 0);
            if (bytes_received == -1) {
                printf("\nError!Cannot receive data from sever!\n");
                close(client_sock);
                exit(-1);
            }
            recv_data[bytes_received] = '\0';
            printf("REC : %s\n",recv_data);
            if (strcmp(SUCCESS, recv_data) == 0) {

                printf("Ban dang la nguoi duoc choi.\n");
                is_player = 1;
            } else if (strcmp(ERROR, recv_data) == 0) {

                printf("Chua den luot choi cua ban.\n");
                is_player = 0;
            } else {
                printf("ERROR in ps3 : %s\n", recv_data);
                is_player = 0;
            }

            printf("TIen trinh 3 isplayer = %d\n", is_player);
//			scanf("%[^\n]%*c", user_id);
            process = 4;
            // break;
        } else if (process == 4) {
            bytes_sent = send(client_sock, "AA", 2, 0);

            bytes_received = recv(client_sock, recv_data, 1024, 0);
            if (bytes_received == -1) {
                printf("\nError!Cannot receive data from sever!\n");
                close(client_sock);
                exit(-1);
            }
            recv_data[bytes_received] = '\0';
            int a = atoi(recv_data);
            check_result(a, oso, process);
            process = 5;
            printf("KT 4\n");
        } else if (process == PROCESS_5) {
            if (is_player == 1) {

                printf("Trang thai o chu : %s\n\n",trang_thai_o_chu);

                printf("---Nhap cau tra loi : ");
                gets(cau_tra_loi);
                bytes_sent = send(client_sock, cau_tra_loi, strlen(cau_tra_loi), 0);
            } else {
                bytes_sent = send(client_sock, "400", 3, 0);
                printf("---Dang doi nguoi choi chinh dua ra cau tra loi  ...\n");
            }
            bytes_received = recv(client_sock, recv_data, 1024, 0);
            if (bytes_received == -1) {
                printf("\nError!Cannot receive data from sever!\n");
                close(client_sock);
                exit(-1);
            }
            recv_data[bytes_received] = '\0';
            printf("DAU RA : %s\n",recv_data);
            if (strcmp(recv_data, o_chu) == 0) {
                printf("Da tra loi dung\n");
                process = 6;
            //    continue;
            } else if (strcmp(recv_data, ERROR) == 0) {
                process = 3;
                printf("tra loi sai");
               // continue;
//				scanf("%d",&i);
            } else if (strcmp(recv_data, o_chu) != 0 && strcmp(recv_data, ERROR) != 0) {
                printf("Trang thai o chu : %s\n\n", recv_data);
                strcpy(trang_thai_o_chu,recv_data);
                process = 4;
            //    continue;
            } else {
                printf("SAI\n");
              //  scanf("%d", &i);
            }

        } else if (process == 6) {
            puts("TIEN trinh 6\n");
            bytes_sent = send(client_sock, "tt6", 3, 0);
            bytes_received = recv(client_sock, recv_data, 1024, 0);
            if (bytes_received == -1) {
                printf("\nError!Cannot receive data from sever!\n");
                close(client_sock);
                exit(-1);
            }
            recv_data[bytes_received] = '\0';
            printf("KET QUA : %s\n",recv_data);
            break;
        }
    }

    close(client_sock);
    return 0;
}