#include "server.h"
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>


int main(int argc , char *argv[]){
    int opt = TRUE;
    int master_socket , addrlen , new_socket , client_socket[3] , max_clients = MAX_C;
    int activity, i, j, valread , sd;
    int max_sd;
    struct sockaddr_in address;
    FILE *f[30], *f_account;
    int process[30], count, result;
    char recv_data[1024];
    char buffer[1024];
    char user_id[30], passwd[30], pro[4];
    char cau_hoi[200];
    char o_chu[20];
    
    fd_set readfds; //set of socket descriptors
    char *message = "Chao mung ban da tham gia tro choi chiec non ki dieu 2016!\n";
  
    for (i = 0; i < max_clients; i++){
        client_socket[i] = 0; //initialise all client_socket[] to 0 so not checked
    }
      
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
  
    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);
     
    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
      
    addrlen = sizeof(address);
    puts("Waiting for connections ...\n");  //accept the incoming connection
     
    while(TRUE){
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
        for ( i = 0 ; i < max_clients ; i++){
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
    
        if ((activity < 0) && (errno!=EINTR)){
            printf("select error");
        }         
        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)){
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
                perror("accept");
                exit(EXIT_FAILURE);
            }          
            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));        
            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message) ){
                perror("send");
            }
            puts("Welcome to my server");
            for (i = 0; i < max_clients; i++){ //add new socket to array of sockets{
                if( client_socket[i] == 0 ){
                    client_socket[i] = new_socket;
                    process[i] = PROCESS_1;
                    printf("Adding to list of sockets as %d\n\n" , i);
                    break;
                }
            }
        }

        for (i = 0; i < max_clients; i++){
            sd = client_socket[i];
            if (FD_ISSET( sd , &readfds)){
                if(process[i] == PROCESS_1){
                    if ((valread = read( sd , recv_data, 1024)) == 0){
                        getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                        printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                        delete_session(sd);
                        delete_player();
                        close( sd );
                        client_socket[i] = 0;
                    }
                    else{
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
                            else if(result == 200){
                                strcpy(buffer,"200");
                                send(sd, buffer, strlen(buffer), 0);
                                process[i] = PROCESS_1;
                            }
                        }
                        // =>Log in
                        else if(process[i] == LOG_IN){
                            result = log_in(sd, user_id, passwd);
                            if(result == 402){
                                strcpy(buffer,"402");
                                send(sd, buffer, strlen(buffer) ,0);
                                process[i] = PROCESS_1;
                            }
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
                }else if(process[i] == PROCESS_2 && count_player == 3){
                    if ((valread = read( sd , recv_data, 1024)) == 0)
                    {
                        getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                        printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                        delete_session(sd);
                        delete_player();
                        close( sd );
                        client_socket[i] = 0;
                    }
                    else{
                        recv_data[valread] = '\0';
                        printf("%s\n", recv_data);
                        create_cauhoi(buffer,cau_hoi,o_chu);
                        printf("%s\n",buffer);
                        send(sd, buffer, strlen(buffer), 0);
                        process[i] = PROCESS_3;
                    }
                }else if(process[i] == PROCESS_3){
                    if ((valread = read( sd , recv_data, 1024)) == 0)
                    {
                        getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                        printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                        delete_session(sd);
                        delete_player();
                        close( sd );
                        client_socket[i] = 0;
                    }
                    else{
                        recv_data[valread] = '\0';
                        printf("%s\n", recv_data);
                        if(play[sd].is_player == 1){
                            strcpy(buffer,"200");
                            send(sd, buffer, strlen(buffer), 0);
                        }else{
                            strcpy(buffer,"404");
                            send(sd, buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }
    return 0;
}
