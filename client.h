#include "lib.h"
char cau_hoi[200];
char o_chu[50], pro[4];
char trang_thai_o_chu[50];
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
			printf("Waiting for other players\n");
			return 1;
		}
		else if(strcmp("403",recv_data) == 0){
			printf("Tai khoan da dang nhap!.\n");
		}
		else if(strcmp("404",recv_data) == 0){
			printf("Sai ten tai khoan hoac mat khau!.\n");
		}
		else if(strcmp("402",recv_data) == 0){
			printf("Da du so luong nguoi choi!.\n");
			exit(-1);
		}
	}while(count != 5);
	printf("Qua so luot dang nhap!.\n");
	return 0;
}
/////////////////////////
int process2(){
	int i;
	printf("\n### Tro choi bat dau! ###\n\n");
	bytes_sent = send(client_sock,"...",3,0);
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
	string_cut(recv_data, pro, cau_hoi, o_chu);
	i = strlen(o_chu);
	printf("O chu cua chung ta gom %d chu cai!\n", i);
	printf("Cau hoi la: %s\n", cau_hoi);
	return 0;
}

int process3(){
	bytes_sent = send(client_sock,"...",3,0);
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
	if(strcmp("200",recv_data)==0){

		printf("Ban dang la nguoi duoc choi.\n");
		return 1;

	}
	else if(strcmp("404",recv_data)==0){

		printf("Chua den luot choi cua ban.\n");
		return 0;
	}
    else printf("AA : %s\n", recv_data);
	return 0;
}
void check_result(int a,int oso,int process)
{
	if (0<=a&&a<=3)
	{
		printf("Nguoi choi da quay duoc o 100 diem\n\n");
		oso = 0;

	}
	else if (4<=a&&a<=6)
	{
		printf("Nguoi choi da quay duoc o 200 diem\n\n");
		oso =1;

	}
	else if (7<=a&&a<=9)
	{
		printf("Nguoi choi da quay duoc o 300 diem\n\n");
		oso = 2;

	}
	else if (10<=a&&a<=12)
	{
		printf("Nguoi choi da quay duoc o 400 diem\n\n");
		oso = 3;

	}
	else if (13<=a&&a<=14)
	{
		printf("Nguoi choi da quay duoc o 500 diem\n\n");
		oso = 4;

	}
	else if (a==15)
	{
		printf("Nguoi choi da quay duoc o 1000 diem\n\n");
		oso = 5;

	}
	else if (a==16) {
		printf("Nguoi choi da quay duoc o mat luot\n\n");
		oso = 6;
		process = 6;
	}

}
