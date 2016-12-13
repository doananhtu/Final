#define TRUE   1
#define FALSE  0
#define PORT 5500
#define SO_CAU_HOI 30
#define MAX_C 30

#include "lib.h"

typedef struct
{
    int session_id;
    char user_id[30];
}session;
session sess[30];

/////////////
void create_player(int id, char *user_id){
    strcmp(play[id].user_id,user_id);
    play[id].diem = 0;
    count_player ++;
}
void delete_player(){
    count_player --;
}
/////////////
void set_status(){
    int i;
    for(i=0; i< MAX_C; i++){
        if(i == 4)
            play[i].is_player = 1;
        else play[i].is_player = 0;
    }
}
/////////////
int sign_in(char user_id[], char passwd[]){
    FILE *f_account;
    f_account = fopen("account.txt","r+");
    char str[30], str2[30];
    if(f_account == NULL){
        printf("File account.txt not found!.\n");
    }
    while(!feof(f_account)){
        fscanf(f_account,"%[^\t]",str);
        if(strcmp(str,user_id) == 0){
            return 404;
        }
        fgets(str,81,f_account);
    }
    fprintf(f_account, "%s\t%s\n", user_id,passwd);
    fclose(f_account);
    return 200;
}

int log_in(int id, char user_id[], char passwd[]){
    FILE *f_account;
    f_account = fopen("account.txt","r");
    char str[30], str2[30];
    if(f_account == NULL){
        printf("File account.txt not found!.\n"); exit(-1);
    }
    if( check_log_in(user_id)==1 )
        return 403;
    while(!feof(f_account)){
        fscanf(f_account,"%[^\t]\t%[^\n]\n",str,str2);
        if(strcmp(str,user_id) == 0 && strcmp(str2,passwd)==0 ){ //dang nhap thanh cong
            create_player(id, user_id);
            set_status();
            if(count_player == 4)
            	return 402;
            return 200;
        }
    }
    fclose(f_account);
    return 404;
}
///////////////
void create_session(int client_socket, char *user_id){
    strcpy(sess[client_socket].user_id,user_id);
}
void delete_session(int client_socket){
    sess[client_socket].user_id[0] = '\0';
}
int check_log_in(char *user_id){
    int i;
    for(i=4; i<=34; i++){
        if( strcmp(sess[i].user_id ,user_id)==0 )
            return 1;
        return 0;
    }
}

//////////////
void create_cauhoi(char *buff, char *cau_hoi, char *o_chu){
    FILE * f = fopen("cau_hoi.txt","r");
    int r,i;
    char *str;
    if(f == NULL){
        printf("File cau_hoi.txt not found!\n"); exit(-1);
    }
    r = random_int();
    i = 0;
    while(!feof(f)){
        fscanf(f,"%[^|]|%[^\n]%*c",cau_hoi,o_chu);
        if(i == r) break;
        i++;
    }
    strcpy(str,"201|");  // xxx|cau_hoi|o_chu
    strcat(str,cau_hoi); strcat(str,"|");
    strcat(str,o_chu);
    strcpy(buff,str);
}
