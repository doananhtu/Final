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
    strcpy(play[id].user_id,user_id);
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

void reset_status(int id, int client_socket[]){
    int i, j;
    if(client_socket[0] == id){
        play[client_socket[0]].is_player = 0;
        play[client_socket[1]].is_player = 1;
        play[client_socket[2]].is_player = 0;

       // return 0;
    }
    if(client_socket[1] == id){
        play[client_socket[1]].is_player = 0;
        play[client_socket[2]].is_player = 1;
        play[client_socket[0]].is_player = 0;

     //   return 0;
    }
    if(client_socket[2] == id){
        play[client_socket[2]].is_player = 0;
        play[client_socket[0]].is_player = 1;
        play[client_socket[1]].is_player = 0;

       // return 0;
    }
}

//
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
//
int get_ket_qua_quay(int soluongketqua) {
    srand(time(NULL));
    int r = rand() % soluongketqua;
    return r;
}
int xac_nhan_cau_tra_loi(char cau_hoi[], char trang_thai_cau_hoi[], char cau_tra_loi[]) {
    int i = 0, dem = 0;
//    printf("DO DAI CAU HOI : %d\n",strlen(cau_hoi));
//    printf("DO DAI CAU TRA LOI : %d\n",strlen(cau_tra_loi));
//    printf("DO DAI TTCAU TRA LOI : %d\n",strlen(trang_thai_cau_hoi));
//    printf("%c\n",cau_tra_loi[0]);

    if (strlen(cau_tra_loi) == 1) {
        //printf("C1\n");
        for (i = 0; i < strlen(cau_hoi); i++) {
            if (cau_hoi[i] == cau_tra_loi[0]) {
                trang_thai_cau_hoi[i] = cau_tra_loi[0];
                dem++;
            }
        }
        // printf("%s\n",trang_thai_cau_hoi);
        if (strcmp(trang_thai_cau_hoi, cau_hoi) == 0) return 0;
        if (dem == 0) return -1;
        else return dem;
    } else {
        //printf("C2\n");
        if (strcmp(cau_hoi, cau_tra_loi) == 0)
        {
            strcpy(trang_thai_cau_hoi,cau_hoi);
            return 0;
        }
        else
            return -1;
    }
}

int get_point(int oso)
{
    switch(oso)
    {
        case 0 : return 100;break;
        case 1 : return 200;break;
        case 2 : return 300;break;
        case 3 : return 400;break;
        case 4 : return 500;break;
        case 5 : return 1000;break;
        case 6 :
            return 0;
            break;
        default:
            return 0;break;
    }
}
int check_result1(int a)
{
    int oso;
    if (0<=a&&a<=3)
    {
        oso = 0;

    }
    else if (4<=a&&a<=6)
    {
        oso =1;

    }
    else if (7<=a&&a<=9)
    {
        oso = 2;

    }
    else if (10<=a&&a<=12)
    {
        oso = 3;

    }
    else if (13<=a&&a<=14)
    {
        oso = 4;

    }
    else if (a==15)
    {
        oso = 5;

    }
    else if (a==16) {
        oso = 6;
    }
    return  oso;
}


