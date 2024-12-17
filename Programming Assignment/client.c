#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <errno.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <signal.h>


#define BUFFER_SIZE 8192


// DEC.

void startInteraction(const int,const char*);

void handleSIGQUIT(const int);


int main(int argc , char *argv[]) {


      if(argc != 4) {
        perror("[-] ERROR: Invalid Usage. (Correct Usage:  ./client [server ip address] [server port number] [file.txt] )");
        return -1;
    }

    signal(SIGQUIT,handleSIGQUIT);

    int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sockfd < 0) {
        perror("[-] ERROR: Failed to create socket.");
        return -1;
    }



    struct sockaddr_in serverInfo;

    memset(&serverInfo,0,sizeof(serverInfo));
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&serverInfo.sin_addr);


    int connectionResult = connect(sockfd,(struct sockaddr*)&serverInfo,sizeof(serverInfo));
    if(connectionResult < 0) {
        perror("[-] ERROR: Failed to connect to server.");
        return -1;
    }


    startInteraction(sockfd,argv[3]);

    return 0;
} 



// DEF.


void startInteraction(const int sockfd , const char* filename) {

    int numberOfBytes = 0;
    char menu[BUFFER_SIZE]; // Store the menu one time rather than exchanging it every time.

    numberOfBytes = read(sockfd,menu,BUFFER_SIZE);
    //printf(menu);

    FILE *myFile = fopen(filename,"r");
    if(myFile == NULL) {
        perror("[-] ERROR: Failed To Open File.");
        return;
    }

    char line[BUFFER_SIZE];
    char answer[BUFFER_SIZE];
    int userOption = 0;

    while(fgets(line,BUFFER_SIZE,myFile) != NULL) {

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') { // Remove new line read by fgets() to match the assignmnet output format 
            line[len - 1] = '\0'; 
        }

        printf("\n\n\n\n");
        printf(menu);

        printf("\n[*] Please Enter Operation: ");
        scanf("%d", &userOption);
        
        // Formatted Message = Line | UserOption --> My Name Is Basil|2
        char formattedMessage[BUFFER_SIZE];  
        snprintf(formattedMessage, sizeof(formattedMessage), "%s|%d", line, userOption);

        numberOfBytes = write(sockfd,formattedMessage,strlen(formattedMessage));

        if(numberOfBytes < 0) {
            perror("[-] ERROR: Failed To Write On Server.");
            continue;
        }

        numberOfBytes = read(sockfd,answer,BUFFER_SIZE);

        answer[numberOfBytes] = '\0';
        printf("[*] Answer: %s",answer);
        printf("\n");


    }


    fclose(myFile);
}


void handleSIGQUIT(int sockfd) {
    printf("\n[*] MSG: Good Bye (:- \n");
    close(sockfd);
    exit(0);
}