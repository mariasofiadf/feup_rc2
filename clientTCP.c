/**      (C)2000-2021 FEUP
 *       tidy up some includes and parameters
 * */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#define SERVER_PORT 21
#define SERVER_ADDR "193.137.29.15"

int get_resp(int sockfd, int code, char* resp){
    int num;
    while(read(sockfd, resp, 255) >= 0){
        num = atoi(resp);
        if(code == num)
            return 0;
    }
    return 0;
}

int rcv(int sockfd){
    char recBuf[1000];
    while(read(sockfd, recBuf, 255) >= 0){
        printf("%s", recBuf);
    }
    return 0;
}

int send_cmd(int sockfd, char* cmd){
    int bytes = write(sockfd, cmd, strlen(cmd));
    //bytes = write(sockfd, buf, strlen(buf));
    if (bytes <= 0){
        perror("write()");
        exit(-1);
    }
    return 0;
}

int anonymous_login(int sockfd){
    send_cmd(sockfd, "user anonymous\n");

    send_cmd(sockfd, "pass password\n");

    return 0;
}

int get_port(char*string){
    
    int num, sum = 0, n1,n2;
    
    for(int i = 0; i < 4; i++){
        sscanf(string, "%d Entering Passive Mode (%d,%d,%d,%d,%d,%d)",&num,&num,&num,&num,&num,&n1,&n2);
    }

    return 256*n1 + n2;

}

int enter_pasv(int sockfd){

    send_cmd(sockfd, "pasv\n");

    char r[255]; get_resp(sockfd, 227,&r);

    printf("RESPONSE: %s", r);

    printf("PORT: %d", get_port(r));

    send_cmd(sockfd, "retr pub/kodi/timestamp.txt\n");

    return 0;
}

int main(int argc, char **argv) {

    int sockfd;
    struct sockaddr_in server_addr;
    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(SERVER_PORT);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }
    /*connect to the server*/
    if (connect(sockfd,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }

    anonymous_login(sockfd);

    enter_pasv(sockfd);

    //rcv(sockfd);

    if (close(sockfd)<0) {
        perror("close()");
        exit(-1);
    }
    return 0;
}


