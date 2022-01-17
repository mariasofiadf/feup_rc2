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
#include <netdb.h>

#define SERVER_PORT 21

char user[255] = "anonymous";
char password[255] = "pass";
char host[255];
char filepath[255];
char ip[255];
char filename[255];


//Reads from socket and saves the line with desired code into param 'resp'
//Used to get port for data socket
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

//Reads from socket and prints to screen
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

//Sends command to socket
int send_cmd(int sockfd, char* cmd){
    int bytes = write(sockfd, cmd, strlen(cmd));

    if (bytes <= 0){
        perror("write()");
        exit(-1);
    }
    return 0;
}

int login(int controlfd){
    char cmd[300];
    sprintf(cmd,"user %s\n", user); // Construct user command to be sent. Eg: 'user anonymous'
    send_cmd(controlfd,cmd); // Send command

    rcv(controlfd, 331); //Receive until code 331 -> 'Please specify the password'

    sprintf(cmd,"pass %s\n", password); // Construct pass command to be sent. Eg: 'pass word'
    send_cmd(controlfd,cmd); // Send command

    rcv(controlfd, 230); //Receive until code 230-> 'Login sucessful'

    return 0;
}

//Calculate port for data socket
int get_port(char*string){
    
    int num, n1,n2;

    //227 Entering Passive Mode (193,137,29,15,196,249).
    sscanf(string, "%d Entering Passive Mode (%d,%d,%d,%d,%d,%d)",&num,&num,&num,&num,&num,&n1,&n2);

    return 256*n1 + n2;
}

//Send retr commnad
int retr(int controlfd){
 
    char cmd[300];
    sprintf(cmd,"retr %s\n", filepath); //Construct retr command to be sent. Eg: 'retr pub/kodi/timestamp.txt'

    send_cmd(controlfd, cmd);

    rcv(controlfd, 150); //Receive until code 150 -> 'Opening BINARY mode data connection for...'
    rcv(controlfd, 226); //Receive until code 226 -> 'Transfer complete.'
    return 0;
}


//Send pasv command
int pasv(int controlfd){

    send_cmd(controlfd, "pasv\n");

    char r[255]; 
    get_resp(controlfd, 227,r); //Receive until 227 and save receive line in r.

    //Eg. r = 227 Entering Passive Mode (193,137,29,15,201,27).
    int port = get_port(r);

    return port;//Returns port for data socket
}

//Open socket connection on port
//Uses global variable 'ip'
int connect_socket(int port){

    int sockfd;
    struct sockaddr_in server_addr;
    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);    /*32 bit Internet address network byte ordered*/
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

//"Translates" url to ipv4
//Utilizes global variables 'ip' and 'host'
int get_host(){

    struct hostent * h;

    if ((h = gethostbyname(host)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }

    sprintf(ip, "%s", inet_ntoa(*((struct in_addr *) h->h_addr)));
    return 0;
}

//Reverses a string
//Used in get_args
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

//Extract arguments from url
int get_args(char *url){
    int anonymous = strchr(url, '@') == NULL; // If there is no '@' in the url, then anonymous mode is used
    char *token = strtok(url, "//");
    
    if(!anonymous){
        token = strtok(NULL, ":"); //Extracts user from url
        strcpy(user,token+1);
        token = strtok(NULL, "@"); //Extracts password from url
        strcpy(password,token);
        
    }
    
    token = strtok(NULL, "/"); //Extracts host from url
    strcpy(host,token);
    token = strtok(NULL, "\0"); //Extracts filepath from url
    strcpy(filepath,token);

    char rev[100];
    strcpy(rev, filepath);
    revstr(rev); // Reverses filepath in order to get filename
    token = strtok(rev, "/");
    strcpy(filename,token);
    revstr(filename);
    get_host();
    return 0;
}


int main(int argc, char **argv) {

    if(argc != 2){
        printf("Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        return 1;
    }

    get_args(argv[1]);

    printf("\nuser: %s\n", user);
    printf("password: %s\n", password);
    printf("host: %s\n", host);
    printf("ip: %s\n", ip);
    printf("filepath: %s\n", filepath);
    printf("filename: %s\n\n ", filename);

    int controlfd = connect_socket(SERVER_PORT); // open control socket

    fsync(controlfd); //Flushes content in controlfd

    login(controlfd); //Sends login commands (anonymous by default)

    int listen_port = pasv(controlfd); //Sends pasv command

    //printf("port: %d\n", listen_port); //For debugging only

    int datafd = connect_socket(listen_port); // open data socket

    retr(controlfd); // Send retr commnad for desired file

    int fd = open(filename, O_WRONLY | O_CREAT, 0644); // Open/Create file to be written

    int r = 0;
    char recBuf[255];
    while((r = read(datafd, recBuf, 255))){ // Write to file while necessary
        write(fd, &recBuf,r);
        printf("%s", recBuf);
    }

    close(fd);

    if (close(controlfd)<0) { // Close control socket
        perror("close()");
        exit(-1);
    }
    if (close(datafd)<0) { // Close data socket
        perror("close()");
        exit(-1);
    }
    return 0;
}


