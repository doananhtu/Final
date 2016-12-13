#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#define PROCESS_1 1
#define PROCESS_2 2

int client_sock;
char buffer[1024],recv_data[1024];
struct sockaddr_in server_addr;
int bytes_sent,bytes_received;

#include "client.h"

int main(){
	int a, i, login;
	int process;
	char user_id[30], passwd[30];

	client_sock=socket(AF_INET,SOCK_STREAM,0);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5500);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(client_sock,(struct sockaddr*)&server_addr,sizeof(struct sockaddr))!=0){
		printf("\nError!Can not connect to sever!Client exit imediately!\n");
		return 0;
	}
	
	bytes_received = recv(client_sock,recv_data,1024,0);
	if(bytes_received == -1){
		printf("\nError!Cannot receive data from sever!\n");
		close(client_sock);
		exit(-1);
	}
	recv_data[bytes_received] = '\0';
	puts(recv_data);
	
	process = PROCESS_1;
	while(1){
		if(process == PROCESS_1){
			printf("\n#########################\n");
			printf("### Chiec non ki dieu ###\n");
			printf("#########################\n\n");
			login = 0;
			do{
				printf("\n1. Dang ky.\n");
				printf("2. Dang nhap.\n");
				printf("3. Thoat.\n");
				printf("Chon: ");
				scanf("%d%*c", &a);
				switch(a){
					case 1:
						sign_in(user_id,passwd);
						break;
					case 2:
						login = log_in(user_id,passwd);
						if(login == 0) exit(-1);
						process = PROCESS_2;
						break;
					case 3:
						exit(-1);
					default:
						printf("Hay dang nhap de tham gia tro choi!.\n");
						break;
				}
			}while(login == 0);
		}
		else if(process == PROCESS_2){
			process2();
			process = PROCESS_3;
		}
		else if(process == 3){
			process3();
			scanf("%[^\n]%*c", user_id);
			process == 4;
			// break;
		}
	}
	
	close(client_sock);
	return 0;
}