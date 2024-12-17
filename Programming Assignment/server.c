
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
#include <signal.h>
#include <ctype.h>


#define BUFFER_SIZE 8192
#define BACK_LOG 4

// DEC.


void serveClient(int);
char *getMenuOptions(void);

int countNumberOfWords(const char *);
int countVowels(const char *);
int countSpecialChars(const char *);
char *findLongestWord(const char *);



int main(int argc , char *argv[]) {
 
    if(argc != 3) {
        perror("[-] ERROR: Invalid Usage. [./server [Ip Address] [Port Number] ]");
        return -1;
    }

    int listenfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(listenfd < 0) {
        perror("[-] ERROR: Failed Create Listening Socket.");
        return -1;
    }

    struct sockaddr_in serverInfo;

    memset(&serverInfo,0,sizeof(serverInfo));
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&serverInfo.sin_addr);


    int bindResult = bind(listenfd,(struct sockaddr*)&serverInfo,sizeof(serverInfo));
    if(bindResult < 0) {
        perror("[-] ERROR: Failed To Bind.");
        return -1;
    }


    int listenResult = listen(listenfd,BACK_LOG);
    if(listenResult < 0) {
        perror("[-] ERROR: Failed To Listen.");
        return -1;
    }


    struct sockaddr_in clientInfo;

    int connfd = 0;
    int pid = 0;

    for(;;) {

        socklen_t clientLength = sizeof(clientInfo);

        connfd = accept(listenfd,(struct sockaddr*)&clientInfo,&clientLength);
        if(connfd < 0) {
            perror("[-] ERROR: Failed To Accept Connection.");
            continue;
        }

        if( (pid = fork()) == 0 ) {
            close(listenfd);
            serveClient(connfd);
            exit(0);
        }

        close(connfd);

    }



    return 0;
}



// DEF.


void serveClient(int connfd) {

    char *menu = getMenuOptions();

    int  numberOfBytes = 0;
    char rcvLine[BUFFER_SIZE];

    write(connfd,menu,strlen(menu));

    while( (numberOfBytes = read(connfd,rcvLine,BUFFER_SIZE)) > 0 ) {

        rcvLine[numberOfBytes] = '\0';

        printf("[*] Received Client Option: %s\n", rcvLine);

          char *line = strtok(rcvLine, "|");
          char *clientOptionStr = strtok(NULL, "|");
          int   clientOption = atoi(clientOptionStr);

        printf("Received line: %s", line);
        printf("Client option: %d\n", clientOption);

        char result[BUFFER_SIZE];

        if(clientOption < 1 || clientOption > 6) {
            strcpy(result, "Unsupported Operation");
            write(connfd,result,strlen(result));
        }
         
    }



}

char *getMenuOptions(void) {

char *menu = "-------------------------------------\n"
             "\t Server's Operations Menu \n"
             "-------------------------------------\n"
             "[1] Count Vowels\n"
             "[2] Count Words\n"
             "[3] Count Special Chars\n"
             "[4] Longest Word\n"
             "[5] Repeated Words\n"
             "-------------------------------------\n";
    return menu;
}


int countVowels(const char *line) {
    
    int answer = 0;
    const char *ptr = line;  

    while (*ptr != '\0') {
        char c = tolower(*ptr);
        if (c == 'a' || c == 'o' || c == 'e' || c == 'i' || c == 'u') {
            answer++;
        }
        ptr++;
    }

    return answer;
}


int countNumberOfWords(const char *line) {
    char *lineCopy = strdup(line);  
    if (!lineCopy) {
        perror("[-] ERROR: Failed to create a copy");
        return -1;
    }

    int answer = 0;
    char *token = strtok(lineCopy, " \t\n\r");
    while (token != NULL) {
        answer++;
        token = strtok(NULL, " \t\n\r");
    }

    free(lineCopy); 

    return answer;
}


int countSpecialChars(const char *line) {
    int answer = 0;
    const char *ptr = line;  

    while (*ptr != '\0') {
        char c = *ptr;
        if (!isdigit(c) && !isalpha(c) && !isspace(c)) {
            answer++;
        }
        ptr++;
    }

    return answer;
}


char *findLongestWord(const char *line) {

    char *lineCopy = strdup(line);  
    if (!lineCopy) {
        perror("[-] ERROR: Failed to create a copy");
        return NULL;
    }

    char *longestWord = NULL;
    int maxLength = 0;

    char *token = strtok(lineCopy, " \t\n\r");
    while (token != NULL) {
        int length = strlen(token);
        if (length > maxLength) {
            maxLength = length;            
            free(longestWord);
            longestWord = strdup(token);  
        }
        token = strtok(NULL, " \t\n\r");
    }

    free(lineCopy);  

    return longestWord; 
}
