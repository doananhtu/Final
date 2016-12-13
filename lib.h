#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define PROCESS_1 1
#define SIGN_IN 101
#define LOG_IN 102
#define PROCESS_2 2
#define PROCESS_3 3

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