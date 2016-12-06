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

int enter_account(char *user_id, char *passwd){
	int i;
	printf("Nhap ten tai khoan: ");
	scanf("%[^\n]%*c", user_id);
	i = 0;
	while(user_id[i]!='\0'){
		user_id[i] = toupper(user_id[i]);
		i++;
	}
	for(i=0; i<strlen(user_id); i++){
		if( (user_id[i]<48) || (57<user_id[i] && user_id[i]<65) || (user_id[i]>90 && user_id[i]<95) || (user_id[i]>95 && user_id[i]<97) || (user_id[i]>122) ){
			printf("Ten tai khoan chua ki tu khong hop le!\n");
			return 0;
		}
	}
	printf("Nhap mat khau: ");
	scanf("%[^\n]%*c", passwd);
	for(i=0; i<strlen(user_id); i++){
		if( (user_id[i]<48) || (57<user_id[i] && user_id[i]<65) || (user_id[i]>90 && user_id[i]<95) || (user_id[i]>95 && user_id[i]<97) || (user_id[i]>122) ){
			printf("Ten tai khoan chua ki tu khong hop le!\n");
			return 0;
		}
	}
	return 1;
}

int sign_in(char *user_id, char *passwd){
	int i=0 ;
	printf("### Dang ky tai khoan\n");
	do{
		i = enter_account(user_id,passwd);
	}while(i==0);
	buffer[0] = '\0';
	strcpy(buffer,"101|"); // 101|user_id|passwd
	strcat(user_id,"|"); strcat(user_id,passwd); 
	strcat(buffer,user_id);
	bytes_sent = send(client_sock,buffer,strlen(buffer),0);
	if(bytes_sent == -1){
		printf("\nError!Cannot send data to sever!\n");
		close(client_sock);
		exit(-1);
	}
	bytes_received = recv(client_sock,recv_data,1024,0);
	if(bytes_received == -1){
		printf("\nError!Cannot receive data from sever!\n");
		close(client_sock);
		exit(-1);
	}
	recv_data[bytes_received] = '\0';
	if(strcmp("200",recv_data) == 0){
		printf("Tao tai khoan moi thanh cong!.\n");
	}else printf("Tai khoan da ton tai!.\n");
}

int log_in(char *user_id, char *passwd){
	int count = 0;
	do{
		count ++;
		buffer[0] = '\0';
		strcpy(buffer,"102|"); // 102|user_id|passwd
		printf("### Dang nhap\n");
		enter_account(user_id,passwd);
		strcat(user_id,"|"); strcat(user_id,passwd); strcat(buffer,user_id);
		bytes_sent = send(client_sock,buffer,strlen(buffer),0);
		if(bytes_sent == -1){
		printf("\nError!Cannot send data to sever!\n");
		close(client_sock);
		exit(-1);
		}
		bytes_received = recv(client_sock,recv_data,1024,0);
		if(bytes_received == -1){
			printf("\nError!Cannot receive data from sever!\n");
			close(client_sock);
			exit(-1);
		}
		recv_data[bytes_received] = '\0';
		if(strcmp("200",recv_data) == 0){
			printf("Dang nhap thanh cong!\n");
			return 1;
		}
		else if(strcmp("403",recv_data) == 0){
			printf("Tai khoan da dang nhap!.\n");
		}
		else if(strcmp("404",recv_data) == 0){
			printf("Sai ten tai khoan hoac mat khau!.\n");
		}
	}while(count != 5);
	printf("Qua so luot dang nhap!.\n");
	return 0;
}

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
	
	bytes_received = recv(client_sock,buffer,1024,0);
	if(bytes_received == -1){
		printf("\nError!Cannot receive data from sever!\n");
		close(client_sock);
		exit(-1);
	}
	buffer[bytes_received] = '\0';
	puts(buffer);
	
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
		else if(process == 2){
			
		}
		else if(process == 3){

		}
		break;
	}
	
	close(client_sock);
	return 0;
}