#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define PROCESS_1 1
#define SIGN_IN 101
#define LOG_IN 102
#define PROCESS_2 2
#define PROCESS_3 3
#define PROCESS_4 4
#define PROCESS_5 5
#define PROCESS_6 6

typedef struct 
{
    char user_id[30];
    int diem;
    int is_player;
}player;
player play[100];
int count_player = 0;

char * get_file_name(int i){
    char *s;
    s = (char *)malloc(sizeof(char) *20);
    strcpy(s, "tmp_ .txt\0");
    s[4] = (48 + i);
    return s;
}
char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

void string_cut(char *str, char *str1, char *str2, char *str3){
    int i;
    char * token;
    token = strtok(str,"|");
    strcpy(str1,token);
    i = 0;
    while(token != NULL){
        i++;
        token = strtok(NULL,"|");
        if(i == 1) strcpy(str2,token);
        if(i == 2) strcpy(str3,token);
    }
}

int random_int(){
	int r;
	srand(time(NULL));
	r = rand() % 30;
	return r;
}


void string_cut2(char *str, char *str1, char *str2, char *str3, char *str4, char *str5, char *str6){
    int i;
    char * token;
    token = strtok(str,"|");
    strcpy(str1,token);
    i = 0;
    while(token != NULL){
        i++;
        token = strtok(NULL,"|");
        if(i == 1) strcpy(str2,token);
        if(i == 2) strcpy(str3,token);
        if(i == 3) strcpy(str4,token);
        if(i == 4) strcpy(str5,token);
        if(i == 5) strcpy(str6,token);
    }
}
