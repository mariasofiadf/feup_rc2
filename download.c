/**      (C)2000-2021 FEUP
 *       tidy up some includes and parameters
 * */

//https://www.omnisecu.com/tcpip/ftp-active-vs-passive-modes.php

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>  
#include <string.h>  
#include<arpa/inet.h>

#define SERVER_PORT 21
#define SERVER_ADDR "193.137.29.15"
#define READ_MAX 50

char user[255];
char password[255];
char host[255];
char url_path[255];
char ip[255];
char filename[255];

int get_resp(int controlfd, int code, char* resp){
    int num, count = 0;
    while(read(controlfd, resp, 255) >= 0){
        num = atoi(resp);
        if(code == num || 1){
            printf("%s\n", resp);
            return 0;
        }
        count++;
    }
    return 0;
}

int rcv(int sockfd, int code){
    char recBuf[255];
    int num = -1, count =0;
    while(read(sockfd, recBuf, 255) >= 0){
        num = atoi(recBuf);
        if(num == code) {
            printf("%s", recBuf);
            return 0;
        }
        num = 0; memset(recBuf,0,strlen(recBuf));
        count++;
    }
    return 0;
}

int send_cmd(int controlfd, char* cmd){
    int bytes = write(controlfd, cmd, strlen(cmd));
    //bytes = write(controlfd, buf, strlen(buf));
    if (bytes <= 0){
        perror("write()");
        exit(-1);
    }
    return 0;
}

int anonymous_login(int controlfd){
    char cmd[300];
    sprintf(cmd,"user %s\n", user);
    send_cmd(controlfd,cmd);

    rcv(controlfd, 331);

    sprintf(cmd,"pass %s\n", password);
    send_cmd(controlfd,cmd);

    rcv(controlfd, 230);

    return 0;
}

int get_port(char*string){
    
    int num, n1,n2;
    
    for(int i = 0; i < 4; i++){
        sscanf(string, "%d Entering Passive Mode (%d,%d,%d,%d,%d,%d)",&num,&num,&num,&num,&num,&n1,&n2);
    }
    return 256*n1 + n2;

}

int retr(int controlfd, char* filepath){
 
    char cmd[300];
    sprintf(cmd,"retr %s\n", url_path);

    //sprintf(cmd, "retr %s", "pub/kodi/timestamp.txt");
    send_cmd(controlfd, cmd);
    

    rcv(controlfd, 150);
    rcv(controlfd, 226);
    return 0;
}


int pasv(int controlfd){

    send_cmd(controlfd, "pasv\n");

    char r[255]; get_resp(controlfd, 227,r);
    int port = get_port(r);

    return port;
}

int connect_socket(int port){

    int sockfd;
    struct sockaddr_in server_addr;
    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);        /*server TCP port must be network byte ordered */

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

    printf("Connected\n");
    return sockfd;
}

int get_host(){

    struct hostent * h;

    if ((h = gethostbyname(host)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }

    sprintf(ip, "%s", inet_ntoa(*((struct in_addr *) h->h_addr)));
    return 0;
}

void revstr(char *str1)  
{  
    // declare variable  
    int i, len, temp;  
    len = strlen(str1); // use strlen() to get the length of str string  
      
    // use for loop to iterate the string   
    for (i = 0; i < len/2; i++)  
    {  
        // temp variable use to temporary hold the string  
        temp = str1[i];  
        str1[i] = str1[len - i - 1];  
        str1[len - i - 1] = temp;  
    }  
}  

int get_args(char *url){
    char *token = strtok(url, "[");

    token = strtok(NULL, ":");
    strcpy(user,token);
    token = strtok(NULL, "]");
    strcpy(password,token);
    token = strtok(NULL, "/");
    strcpy(host,token);
    token = strtok(NULL, "\0");
    strcpy(url_path,token);

    char rev[100];
    strcpy(rev, url_path);
    revstr(rev);
    token = strtok(rev, "/");
    strcpy(filename,token);
    revstr(filename);
    printf("filename: %s\n ", filename);
    get_host();
    printf("ip: %s\n", ip);
    return 0;
}

int main(int argc, char **argv) {

    if(argc != 2){
        printf("Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        return 1;
    }

    get_args(argv[1]);

    int controlfd = connect_socket(SERVER_PORT);

    fsync(controlfd);

    anonymous_login(controlfd);

    int listen_port = pasv(controlfd);

    printf("port: %d\n", listen_port);

    int datafd = connect_socket(listen_port);

    retr(controlfd, NULL);

    int fd = open("copy.txt", O_WRONLY | O_CREAT, S_IRWXU);

    int r = 0;
    char recBuf[255];
    while((r = read(datafd, recBuf, 255))){
        write(fd, &recBuf,r);
        printf("%s", recBuf);
    }

    close(fd);

    if (close(controlfd)<0) {
        perror("close()");
        exit(-1);
    }
    if (close(datafd)<0) {
        perror("close()");
        exit(-1);
    }
    return 0;
}


