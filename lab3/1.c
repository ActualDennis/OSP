#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

#define FILE_BUFFER_SIZE 81920

typedef struct dirent dirent;

void FindFiles(char* directoryFullPath);

void CalculateFileBits(FILE* fStream, char* fileName, int bufferSize);

void ProcessFile(char* fileName, int processIndex);

int IsDots(char* name);

void NewProcess(char* filePath);

char* programName;

int curr_processes_amount = 0;

int PROCESSES_MAX = 0;


int main(int argc, char* argv[]){
    dirent* dirInfo;

    int nameLength = strlen(argv[0]);

    int slashPos = 0;

    for(int i = nameLength - 1; i > 0; --i){
        if(argv[0][i] == '/'){
            slashPos = i;
            break;
        }
    }

    if(slashPos != 0){
        memcpy(argv[0], &argv[0][slashPos + 1], nameLength - slashPos - 1);
        argv[0][nameLength - slashPos - 1] = '\0';
        programName = argv[0];
    }
    else{
        programName = argv[0];
    }


    int pathLength = strlen(argv[1]);

    char* fullFilePath = calloc(1,pathLength + 1);

    fullFilePath = strcpy(fullFilePath,argv[1]);

    PROCESSES_MAX = atoi(argv[2]);

    struct timeval  tv1, tv2;
    gettimeofday(&tv1, NULL);

    FindFiles(fullFilePath);

    //wait for rest of processes to complete
    for(int i = 0; i < curr_processes_amount; ++i){              
        wait(NULL);
    }

    gettimeofday(&tv2, NULL);

   // printf ("Total time = %f seconds\n", (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
}

void ProcessFile(char* fileName, int processIndex){
    FILE* fStream;

    fStream = fopen(fileName, "r");

    if(fStream == NULL){
        fprintf(stderr, "%s: Error opening file %s %s\n", programName, fileName, strerror(errno));
        return;
    }

   // printf("%d:", processIndex);

    CalculateFileBits(fStream, fileName, FILE_BUFFER_SIZE);

    fclose(fStream);

    free(fileName);
}

void CalculateFileBits(FILE* fStream, char* fileName, int bufferSize){
    int bytesReadTotal = 0;
    int OnesAmount = 0;
    int ZeroesAmount = 0;
    size_t bytesRead;
    char* buffer = calloc(1, bufferSize);

    while(1){
        bytesRead = fread(buffer, 1, bufferSize, fStream );

        if(bytesRead == 0)
            break;

        for(int byte = 0 ; byte < bytesRead; ++byte){

            for(int i = 1; i < 255; i = i * 2){

                if((buffer[byte] & i) == i)
                    ++OnesAmount;
                else
                    ++ZeroesAmount;               
            }
        }

        bytesReadTotal += bytesRead;

        // if(feof(fStream)){ //exclude null byte at the end.
        //     if(ZeroesAmount != 0){
        //         ZeroesAmount -= 8;
        //         --bytesReadTotal;
        //     }
        //     break;
        // }
    }

    printf("%d %s %d 1:%d 0:%d \n", getpid(), fileName, bytesReadTotal, OnesAmount, ZeroesAmount);
    free(buffer);
}

void allocAndSetNewValue(char** dest, char** newValue){
    *dest = calloc(sizeof(char), strlen(*newValue) + 1);
    char *tempFileInfoError;

    if (dest == NULL)
    {
        fprintf(stderr, "%s: Error allocating memory %s\n", programName, strerror(errno));
        return;
    }

    strcpy(*dest, *newValue);
}

void FindFiles(char* directoryFullPath){
    dirent* dirInfo;
    char* tempFileInfoError;

    char* result;
    struct stat fileInfo;

    DIR* path = opendir (directoryFullPath);

    if(path == NULL){
        fprintf(stderr, "%s: Error opening dir %s %s\n", programName, directoryFullPath, strerror(errno));
        return;
    }

    errno = 0;
    result = calloc(1, strlen(directoryFullPath) + 1);
    allocAndSetNewValue(&result, &directoryFullPath);

    while(dirInfo = readdir(path)){

        if(dirInfo->d_name == NULL)
            continue;

        int resultLen = strlen(result);

        int entryLen = strlen(dirInfo->d_name);
        
        result = realloc(result, resultLen + entryLen + 2);

        if(result == NULL){
            fprintf(stderr, "%s: Couldn't reallocate memory %s\n", programName, strerror(errno));
            result = calloc(1, strlen(directoryFullPath) + 1);
            allocAndSetNewValue(&result, &directoryFullPath);
            continue;
        }

        strcat(result, "/");
        strcat(result, dirInfo->d_name);

        if((dirInfo->d_type == DT_REG)){

            if(stat(result, &fileInfo) == -1){
                fprintf(stderr, "%s: Error retrieving file info %s\n", programName, strerror(errno));
                allocAndSetNewValue(&result, &directoryFullPath);
                continue;
            }

            if (curr_processes_amount == PROCESSES_MAX)
            {
                int killed = 0;
                pid_t terminationResult;
                int status = 0; 

                //only kill terminated processes

                while ((terminationResult = waitpid(-1, &status, WNOHANG)) > 0)
                {
                    ++killed;
                }

                errno = 0;

                if((!killed) && (curr_processes_amount != 0)){
                    int stat = 0;
                    wait(&stat);
                    --curr_processes_amount;
                }

                curr_processes_amount -= killed;
            }

            ++curr_processes_amount;
            NewProcess(result);

            allocAndSetNewValue(&result, &directoryFullPath);
            continue;

        }
        else if((dirInfo->d_type == DT_DIR) && (!IsDots(dirInfo->d_name))){
            FindFiles(result);
        }
        
        free(result);
        allocAndSetNewValue(&result, &directoryFullPath);
    }

    if (closedir(path) == -1)
    {
        fprintf(stderr, "%s: Error closing dir %s\n", programName, strerror(errno));
    }

    if((errno != 0) && (dirInfo == NULL)){
         fprintf(stderr, "%s: Error opening dir %s %s\n", programName, directoryFullPath, strerror(errno));
    }
}

void NewProcess(char* filePath){
    pid_t pid;

    pid = fork();

    if(pid == 0)
    {
        ProcessFile(filePath, curr_processes_amount);
        exit(0);
    }else if (pid == -1){
        fprintf(stderr, "%s: Fork failed %s\n", programName, strerror(errno));
    }
}

int IsDots(char* name){
    if(!strcmp(name, ".")){
        return 1;
    }

    if(!strcmp(name, "..")){
        return 1;
    }

    return 0;
}