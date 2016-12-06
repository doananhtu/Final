#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define TRUE   1
#define FALSE  0
#define PORT 5500

#define PROCESS_1 1
#define SIGN_IN 101
#define LOG_IN 102

#define PROCESS_2 2

typedef struct
{
    int session_id;
    char user_id[30];
}session;
session sess[30];

typedef struct 
{
    char user_id[30];
    int diem;
    int is_player;
}player;
player play[3];
int count_player = 0;
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
    for(i=0; i<count_player; i++){
        if(i == 0)
            play[i].is_player = 1;
        else play[i].is_player = 0;
    }
}
/////////////
char * get_file_name(int i){
    char *s;
    s = (char *)malloc(sizeof(char) *20);
    strcpy(s, "tmp_ .txt\0");
    s[4] = (48 + i);
    return s;
}

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

int log_in(char user_id[], char passwd[]){
    FILE *f_account;
    f_account = fopen("account.txt","r");
    char str[30], str2[30];
    if(f_account == NULL){
        printf("File account.txt not found!.\n");
    }
    if( check_log_in(user_id)==1 )
        return 403;
    while(!feof(f_account)){
        fscanf(f_account,"%[^\t]\t%[^\n]\n",str,str2);
        if(strcmp(str,user_id) == 0 && strcmp(str2,passwd)==0 ){ //dang nhap thanh cong
            create_player(count_player,user_id);
            if(count_player = 3)
                set_status();
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

int main(int argc , char *argv[])
{
    int opt = TRUE;
    int master_socket , addrlen , new_socket , client_socket[3] , max_clients = 3;
    int activity, i, j, valread , sd;
    int max_sd;
    struct sockaddr_in address;
    FILE *f[30], *f_account;
    int process[30], count, result;
    char recv_data[1024];
    char buffer[1024];
    char user_id[30], passwd[30], pro[4];
    
    //set of socket descriptors
    fd_set readfds;
    //a message
    char *message = "Chao mung ban da tham gia tro choi chiec non ki dieu 2016!\n";
  
    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++) 
    {
        client_socket[i] = 0;
    }
      
    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
  
    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
  
    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    //bind the socket to localhost port 5500
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);
     
    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
      
    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...\n");
     
    while(TRUE) 
    {
        //clear the socket set
        FD_ZERO(&readfds);
  
        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
         
        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++) 
        {
            //socket descriptor
            sd = client_socket[i];
             
            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);
             
            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }
  
        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    
        if ((activity < 0) && (errno!=EINTR)) 
        {
            printf("select error");
        }
          
        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) 
        {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
          
            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
        
            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
            {
                perror("send");
            }
            puts("Welcome to my server");
        
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++) 
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    // f[i] = fopen(get_file_name(i),"w+"); //file tam tuong ung voi tung client
                    process[i] = PROCESS_1;
                    printf("Adding to list of sockets as %d\n\n" , i);
                    break;
                }
            }
        }

        //
        //else its some IO operation on some other socket :)
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];
            if (FD_ISSET( sd , &readfds)) 
            {
                //Check if it was for closing , and also read the incoming message
                if(process[i] == PROCESS_1){
                    if ((valread = read( sd , recv_data, 1024)) == 0)
                    {
                        getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                        printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                        delete_session(sd);
                        delete_player();
                        close( sd );
                        client_socket[i] = 0;
                    }
                    else
                    {
                        recv_data[valread] = '\0';
                        printf("%s\n", recv_data);
                        string_cut(recv_data,pro,user_id,passwd); // xyz|user_id|passwd
                        if( strcmp(pro,"101")==0 )
                            process[i] = SIGN_IN; //101
                        else process[i] = LOG_IN; //102
                        // =>Sign in
                        if(process[i] == SIGN_IN){
                            result = sign_in(user_id,passwd);
                            if(result == 404){
                                strcpy(buffer,"404");
                                send(sd, buffer, strlen(buffer) ,0);
                                process[i] = PROCESS_1;
                            }
                            else if(result == 200) {
                                strcpy(buffer,"200");
                                send(sd, buffer, strlen(buffer), 0);
                                process[i] = PROCESS_1;
                            }
                        }
                        // =>Log in
                        else if(process[i] == LOG_IN){
                            result = log_in(user_id,passwd);
                            if(result == 403){
                                strcpy(buffer,"403");
                                send(sd, buffer, strlen(buffer) ,0);
                                process[i]=PROCESS_1;
                            }
                            if(result == 404){
                                strcpy(buffer,"404");
                                send(sd, buffer, strlen(buffer) ,0);
                                process[i]=PROCESS_1;
                            }
                            else if(result == 200){
                                create_session(client_socket[i],user_id);
                                process[i] = PROCESS_2;
                                strcpy(buffer,"200");
                                send(sd, buffer, strlen(buffer), 0);
                                process[i] = PROCESS_2;
                            }
                        }
                    }
                }else if(process[i] == 2){
                    if ((valread = read( sd , recv_data, 1024)) == 0)
                    {
                        getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                        printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                        delete_session(sd);
                        close( sd );
                        client_socket[i] = 0;
                    }
                    else
                    {
                        recv_data[valread] = '\0';
                    }
                }else if(process[i]==3){
                    valread = recv( sd , recv_data, 1024, 0);
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    close( sd );
                    client_socket[i] = 0;
                    fclose(f[i]);
                    process[i] = 0;
                }
            }
        }
    }
    return 0;
}
