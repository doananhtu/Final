#include "server.h"
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

int ketquaquay = -1;
int kt = -2;
int diem_cua_cau = 0;
int d = 0;
char ERROR[] = "404";
char SUCCESS[] = "200";
int sovong = 0;
char str[30];
void string_cat(player a[],int client_socket[]){
    char str1[30], str2[30],str3[30],str4[30],str5[30],str6[30];
    int j =0;
//    while(i<3) {
//        strcpy(str1, a[0].user_id);
//        printf("str1 : %s\n", str1);
//        itoa(a[0].diem, str2);
//        printf("str2 : %s\n", str2);
//
//        str2[strlen(str2)] = '\0';
//        strcpy(str3, a[1].user_id);
//        itoa(a[1].diem, str4);
//        str4[strlen(str4)] = '\0';
//
//        strcpy(str5, a[2].user_id);
//        itoa(a[2].diem, str6);
//        str6[strlen(str6)] = '\0';
//    }
//
    strcpy(str,"");
    for (j = 0; j < MAX_C; j++) {
        int sd1 = client_socket[j];
        if (sd1 != 0){
            strcat(str,a[sd1].user_id);
            strcat(str,"|");
            char s[30];
            itoa(a[sd1].diem, s);
            strcat(str,s);
            strcat(str,"|");
        }
    }

    printf("%s\n",str);
}

int main(int argc, char *argv[]) {
    int opt = TRUE;
    int master_socket, addrlen, new_socket, client_socket[MAX_C], max_clients = MAX_C;
    int activity, i, j, valread, sd;
    int max_sd;
    struct sockaddr_in address;
    FILE *f[30], *f_account;
    int process[30], count, result;
    char recv_data[1024];
    char buffer[1024];
    char user_id[30], passwd[30], pro[4];
    char cau_hoi[200], trang_thai_cau_hoi[30];
    char o_chu[30];
    char oso[30];
    fd_set readfds; //set of socket descriptors
    char *message = "Chao mung ban da tham gia tro choi chiec non ki dieu 2016!\n";

    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0; //initialise all client_socket[] to 0 so not checked
    }

    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(master_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(address);
    puts("Waiting for connections ...\n");  //accept the incoming connection

    while (TRUE) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
        for (i = 0; i < max_clients; i++) {
            //socket descriptor
            sd = client_socket[i];
            //if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);
            //highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;
        }
        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }
        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket,
                   inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            //send new connection greeting message
            if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
                perror("send");
            }
            puts("Welcome to my server");
            for (i = 0; i < max_clients; i++) { //add new socket to array of sockets{
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    process[i] = PROCESS_1;
                    printf("Adding to list of sockets as %d\n\n", i);
                    break;
                }
            }
        }

        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                if (process[i] == PROCESS_1) {
                    if ((valread = read(sd, recv_data, 1024)) == 0) {
                        getpeername(sd, (struct sockaddr *) &address, (socklen_t * ) & addrlen);
                        printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr),
                               ntohs(address.sin_port));
                        delete_session(sd);
                        delete_player();
                        close(sd);
                        client_socket[i] = 0;
                    } else {
                        recv_data[valread] = '\0';
                        printf("%s\n", recv_data);
                        string_cut(recv_data, pro, user_id, passwd); // xyz|user_id|passwd
                        if (strcmp(pro, "101") == 0)
                            process[i] = SIGN_IN; //101
                        else process[i] = LOG_IN; //102
                        // =>Sign in
                        if (process[i] == SIGN_IN) {
                            result = sign_in(user_id, passwd);
                            if (result == 404) {
                                strcpy(buffer, "404");
                                send(sd, buffer, strlen(buffer), 0);
                                process[i] = PROCESS_1;
                            } else if (result == 200) {
                                strcpy(buffer, "200");
                                send(sd, buffer, strlen(buffer), 0);
                                process[i] = PROCESS_1;
                            }
                        }
                            // =>Log in
                        else if (process[i] == LOG_IN) {
                            result = log_in(sd, user_id, passwd);
                            if (result == 402) {
                                strcpy(buffer, "402");
                                send(sd, buffer, strlen(buffer), 0);
                                process[i] = PROCESS_1;
                            }
                            if (result == 403) {
                                strcpy(buffer, "403");
                                send(sd, buffer, strlen(buffer), 0);
                                process[i] = PROCESS_1;
                            }
                            if (result == 404) {
                                strcpy(buffer, "404");
                                send(sd, buffer, strlen(buffer), 0);
                                process[i] = PROCESS_1;
                            } else if (result == 200) {
                                create_session(client_socket[i], user_id);
                                process[i] = PROCESS_2;
                                strcpy(buffer, "200");
                                send(sd, buffer, strlen(buffer), 0);
                                process[i] = PROCESS_2;
                            }
                        }
                    }
                } else if (process[i] == PROCESS_2 && count_player == 4) {
                    if ((valread = read(sd, recv_data, 1024)) == 0) {
                        getpeername(sd, (struct sockaddr *) &address, (socklen_t * ) & addrlen);
                        printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr),
                               ntohs(address.sin_port));
                        delete_session(sd);
                        delete_player();
                        close(sd);
                        client_socket[i] = 0;
                    } else {
                        recv_data[valread] = '\0';
                        printf("%s\n", recv_data);
                        create_cauhoi(buffer, cau_hoi, o_chu);
//                        printf("do dai %d", strlen(o_chu));
                        for (j = 0; j < strlen(o_chu); j++) {
                            trang_thai_cau_hoi[j] = '*';
                        }
                        //printf("%s\n", trang_thai_cau_hoi);
                        send(sd, buffer, strlen(buffer), 0);
                        process[i] = PROCESS_3;
                    }
                } else if (process[i] == PROCESS_3) {
                    if ((valread = read(sd, recv_data, 1024)) == 0) {
                        getpeername(sd, (struct sockaddr *) &address, (socklen_t * ) & addrlen);
                        printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr),
                               ntohs(address.sin_port));
                        delete_session(sd);
                        delete_player();
                        close(sd);
                        client_socket[i] = 0;
                    } else {
                        recv_data[valread] = '\0';
                        // printf("%s\n", recv_data);
                        if (play[sd].is_player == 1) {
                            send(sd, "200", 3, 0);
                        } else {
                            send(sd, "404", 3, 0);
                        }
                        process[i] = PROCESS_4;
                    }
                } else if (process[i] == 4) {
                    if ((valread = read(sd, recv_data, 1024)) == 0) {
                        getpeername(sd, (struct sockaddr *) &address, (socklen_t * ) & addrlen);
                        printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr),
                               ntohs(address.sin_port));
                        delete_session(sd);
                        delete_player();
                        close(sd);
                        client_socket[i] = 0;
                    } else {
                        printf("PROCESS : %d",process[i]);
                        recv_data[valread] = '\0';
                        //  puts(recv_data);
                        ketquaquay = -1;
                        if (play[sd].is_player == 1) {
                            ketquaquay = get_ket_qua_quay(16);
                        }

                        if (ketquaquay != -1) {
                            diem_cua_cau = get_point(check_result1(ketquaquay));
                            itoa(ketquaquay, oso);
                            oso[strlen(oso)] = '\0';
                            for (j = 0; j < max_clients; j++) {
                                int sd1 = client_socket[j];
                                //if (sd1 != sd)
                                    send(sd1, oso, strlen(oso), 0);
                                process[j] = 5;
                            }
                           // send(sd, oso, strlen(oso), 0);
//                            process[i] = 4;
//                        send(sd, oso, strlen(oso), 0);

//                            if (ketquaquay >= 0 && ketquaquay <= 15) {
//                                process[i] = PROCESS_5;//o diem
//                            } else if (ketquaquay == 16) {
//                                process[i] = PROCESS_3;
//                            }
                        }
                    }
                } else if (process[i] == 5) {
                    if ((valread = read(sd, recv_data, 1024)) == 0) {
                        getpeername(sd, (struct sockaddr *) &address, (socklen_t * ) & addrlen);
                        printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr),
                               ntohs(address.sin_port));
                        delete_session(sd);
                        delete_player();
                        close(sd);
                        client_socket[i] = 0;
                    } else {
                        kt = -2;

                        recv_data[valread] = '\0';
                        //puts(recv_data);
                        if (play[sd].is_player == 1) {
                            kt = xac_nhan_cau_tra_loi(o_chu, trang_thai_cau_hoi, recv_data);
                            printf("KET QUA KIEM TRA %d\n", kt);
                        }
//                            printf("%d\n", kt);
                            //    }
                            //  if (kt >= -1) {
                            if (kt == 0) {
                                process[i] = PROCESS_6;
                                play[sd].diem += 3000;
                                string_cat(play,client_socket);
                                //send all
                                for (j = 0; j < max_clients; j++) {
                                    int sd1 = client_socket[j];
                                    if (sd1 != sd)
                                    {
                                        send(sd1, trang_thai_cau_hoi, strlen(trang_thai_cau_hoi), 0);
                                        process[j] = 6;
                                    }
                                }
                                send(sd, trang_thai_cau_hoi, strlen(trang_thai_cau_hoi), 0);

                            }
                            else
                            if (kt > 0) {
                                    process[i] = PROCESS_4;
                                    play[sd].diem += kt * diem_cua_cau;
                                  //  printf("DIEM : %d", play[sd].diem);
                                //send all
                                for (j = 0; j < max_clients; j++) {
                                    int sd1 = client_socket[j];
                                    if (sd1 != sd)
                                        send(sd1, trang_thai_cau_hoi, strlen(trang_thai_cau_hoi), 0);
                                }
                                send(sd, trang_thai_cau_hoi, strlen(trang_thai_cau_hoi), 0);

                            }
                            else if (kt == -1) {
                                printf("RESSS\n");
                               // scanf("%d",&j);
                                for (j = 0; j < max_clients; j++) {
                                    int sd1 = client_socket[j];
                                    if (sd1 != sd)
                                        send(sd1, ERROR, strlen(ERROR), 0);
                                }
                                send(sd, ERROR, strlen(ERROR), 0);
                                reset_status(sd, client_socket);
                                d++;
                                printf("So lan %d\n",d);
                                for (j = 0; j < max_clients; j++) {
                                    if(i!=j)
                                    process[j] = 3;
                                }
                                process[i] = 3;
                                play[sd].is_player=0;
                              //  continue;
                        }
                            //        }

                    }
                    // printf("%d\n",kt);

                } else if (process[i] == 6) {
                    if ((valread = read(sd, recv_data, 1024)) == 0) {
                        getpeername(sd, (struct sockaddr *) &address, (socklen_t * ) & addrlen);
                        printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr),
                               ntohs(address.sin_port));
                        delete_session(sd);
                        delete_player();
                        close(sd);
                        client_socket[i] = 0;
                    } else {
                        send(sd,str,strlen(str),0);
                        delete_session(sd);
                        delete_player();
                        close(sd);
                        client_socket[i] = 0;

                    }
                        break;
                }
            }
        }
    }
    return 0;
}
