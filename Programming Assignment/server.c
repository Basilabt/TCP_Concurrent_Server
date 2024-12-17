
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <errno.h>
#include <stdio.h>
#include <inttypes.h>
#include <signal.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>


#define BUFFER_SIZE 8192
#define BACK_LOG 1024
#define MAX_ALLOWED_CONCURRENT_CLIENTS 2

#define MAX_RECORD_LENGTH 4096

#define DATE_SIZE 11
#define TIME_SIZE 9


int numberOfConcurrentClients = 0;

// DEC.

void printConnectedClientInfo(const struct sockaddr_in);
void serveClient(const int,const struct sockaddr_in);
char *getMenuOptions(void);

int countNumberOfWords(const char *);
int countVowels(const char *);
int countSpecialChars(const char *);
char *findLongestWord(const char *);

void handleZombiesChildren(int); // Tracks terminated children & the number of concurrent clients 
void incrementNumberOfConcurrentClients(void);
void decrementNumberOfConcurrentClients(void);

bool isBlocking(void);

void logOperationRecord(const char*,const char*,const char *,const char*,const char*);
void getCurrentDateTime(char*,char*);





int main(int argc , char *argv[]) {

    if(argc != 3) {
        perror("[-] ERROR: Invalid Usage. [./server [Ip Address] [Port Number] ]\n");
        return -1;
    }

    int listenfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(listenfd < 0) {
        perror("[-] ERROR: Failed Create Listening Socket.\n");
        return -1;
    }

    struct sockaddr_in serverInfo;

    memset(&serverInfo,0,sizeof(serverInfo));
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&serverInfo.sin_addr);


    int bindResult = bind(listenfd,(struct sockaddr*)&serverInfo,sizeof(serverInfo));
    if(bindResult < 0) {
        perror("[-] ERROR: Failed To Bind.\n");
        return -1;
    }


    int listenResult = listen(listenfd,BACK_LOG);
    if(listenResult < 0) {
        perror("[-] ERROR: Failed To Listen.\n");
        return -1;
    }


    struct sockaddr_in clientInfo;

    int connfd = 0;
    int pid = 0;

   

    for(;;) {

        socklen_t clientLength = sizeof(clientInfo);

        connfd = accept(listenfd,(struct sockaddr*)&clientInfo,&clientLength);
        if(connfd < 0) {
            perror("[-] ERROR: Failed To Accept Connection.\n");          
        }

       signal(SIGCHLD,handleZombiesChildren);
        
       incrementNumberOfConcurrentClients();

       if(isBlocking()) {
          printf("[*] BLOCKING: Server Is Blocking Waiting For A Client To Terminate\n");
       }

       while(numberOfConcurrentClients > MAX_ALLOWED_CONCURRENT_CLIENTS);

        if( (pid = fork()) == 0 ) {
            close(listenfd);
            printConnectedClientInfo(clientInfo);
            serveClient(connfd,clientInfo);        
            exit(0);
        }

        close(connfd);

    }

   

    return 0;
}



// DEF.

void printConnectedClientInfo(const struct sockaddr_in clientInfo) {

    char ipAddress[INET_ADDRSTRLEN]; 
    inet_ntop(AF_INET, &(clientInfo.sin_addr), ipAddress, INET_ADDRSTRLEN);

    int portNumber = ntohs(clientInfo.sin_port);

    printf("[*] New Client Is Connected: Ip Address = %s  & Port Number = %d\n",ipAddress,portNumber);

}


void serveClient(const int connfd , const struct sockaddr_in clientInfo) {

    char ipAddress[INET_ADDRSTRLEN]; 
    inet_ntop(AF_INET, &(clientInfo.sin_addr), ipAddress, INET_ADDRSTRLEN);

    char *menu = getMenuOptions();

    int  numberOfBytes = 0;
    char rcvLine[BUFFER_SIZE];

    write(connfd,menu,strlen(menu));



    while( (numberOfBytes = read(connfd,rcvLine,BUFFER_SIZE)) > 0 ) {

        char date[DATE_SIZE];
        char time[TIME_SIZE];
        getCurrentDateTime(date,time);

        rcvLine[numberOfBytes] = '\0';

        //printf("[*] Received Client Option: %s\n", rcvLine);

          char *line = strtok(rcvLine, "|");
          char *clientOptionStr = strtok(NULL, "|");
          int   clientOption = atoi(clientOptionStr);

        //printf("Received line: %s", line);
        //printf("Client option: %d\n", clientOption);

        char result[BUFFER_SIZE];

        switch (clientOption) {

            case 1: {
                int answer = countNumberOfWords(line);
                snprintf(result, sizeof(result), "OP1(%s)=%d", line, answer);
                logOperationRecord(date,time,ipAddress,"[1] Count Words","Succeed");
                break;
            }
            case 2: {
                int answer = countVowels(line);
                snprintf(result, sizeof(result), "OP2(%s)=%d", line, answer);
                logOperationRecord(date,time,ipAddress,"[2] Count Vowels","Succeed");
                break;
            }
            case 3: {
                int answer = countSpecialChars(line);
                snprintf(result, sizeof(result), "OP3(%s)=%d", line, answer);
                logOperationRecord(date,time,ipAddress,"[3] Count Special Chars","Succeed");
                break;
            }
            case 4: {
                char *answer = findLongestWord(line);
                snprintf(result, sizeof(result), "OP4(%s)=%s", line, answer);
                logOperationRecord(date,time,ipAddress,"[4] Find Longest Words","Succeed");
                break;
            }
            case 5: {
                strcpy(result, "Operation 5"); // MUST DO
                logOperationRecord(date,time,ipAddress,"[5] Find Repeated Words","Succeed");
                break;
            }
            default: {
                strcpy(result, "Unsupported Operation");
                logOperationRecord(date,time,ipAddress,"[-] Invalid Option","Failed");
                break;
            }
    }

        write(connfd,result,strlen(result));
         
    }



}

char *getMenuOptions(void) {

char *menu = "-------------------------------------\n"
             "\t Server's Operations Menu \n"
             "-------------------------------------\n"
             "[1] Count Words\n"
             "[2] Count Vowels\n"
             "[3] Count Special Chars\n"
             "[4] Longest Word\n"
             "[5] Repeated Words\n"
             "-------------------------------------\n";
    return menu;
}


void handleZombiesChildren(int signo) {

    pid_t pid;
    int status;

    while( (pid = waitpid(-1,&status,WNOHANG)) > 0 ) {
        printf("[*] Terminated: Child Process With ID = %d Is Terminated\n",pid);
    }

    decrementNumberOfConcurrentClients();

}

void incrementNumberOfConcurrentClients(void) {
    numberOfConcurrentClients += 1;
}

void decrementNumberOfConcurrentClients(void) {
    numberOfConcurrentClients -= 1;
}

bool isBlocking() {
    return numberOfConcurrentClients > MAX_ALLOWED_CONCURRENT_CLIENTS;
}

void logOperationRecord(const char *date, const char *time, const char *clientIPAddress, const char *operation, const char *status) {

    char record[MAX_RECORD_LENGTH];

    snprintf(record, MAX_RECORD_LENGTH, "[*] LOG RECORD: [%s %s] Client IP: %s | Operation: %s | Status: %s", date, time, clientIPAddress, operation, status);
    

    FILE *myFile = fopen("log-file.txt", "a");
    
    
    if (myFile == NULL) {
        perror("[-] ERROR: Failed to open log file");
        return;
    }

    fprintf(myFile, "%s\n", record);

    fclose(myFile);

    printf("[*] LOG: Operation Logged Succsesfully.\n");
}

void getCurrentDateTime(char *dateBuffer, char *timeBuffer) {

    time_t now = time(NULL);
    struct tm *localTime = localtime(&now);

    strftime(dateBuffer, 11, "%Y-%m-%d", localTime); // Date format: YYYY-MM-DD
    strftime(timeBuffer, 9, "%H:%M:%S", localTime);  // Time format: HH:MM:SS
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
