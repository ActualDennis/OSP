#include "list.h"
#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>


typedef struct dirent dirent;

void FindFiles(char* directoryFullPath, Node** listHead);

void CalculateFileBits(FILE* fStream, char* fileName);

void ProcessFile(int index, Node* list, int processIndex);

int IsDots(char* name);

static char* programName;


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

    int PROCESSES_MAX = atoi(argv[2]);

    Node* list = list_create();

    FindFiles(fullFilePath, &list);

    int listSize = list_size(list);

    int curr_processes_amount = 0;

    int MainProcessPid = getpid();

    for(int i = 0; i < listSize; ++i){

        pid_t pid = -2;

        int IsMainProcess = (MainProcessPid == getpid());

        if(IsMainProcess){

            if(curr_processes_amount < PROCESSES_MAX){

            pid = fork();
            ++curr_processes_amount;

        } else if (curr_processes_amount >= PROCESSES_MAX){

            int proc_stat = 0;

            for(int i = 0; i < curr_processes_amount; ++i){
                
                if(wait(&proc_stat) == -1)
                    break;
            }

            curr_processes_amount = 0;
            --i;
            }
        }


        if(pid == 0){
            ProcessFile(i, list, curr_processes_amount);
            break;
        }
    
    }


}

void ProcessFile(int index, Node* list, int processIndex){
    NodeInfo tempNode;
    FILE* fStream;
    char* tempFileInfoError;

    tempNode = list_get(list, index);
    fStream = fopen(tempNode.fileName, "r");

    if(fStream == NULL){
        tempFileInfoError = calloc(sizeof(char),256);
        strcat(tempFileInfoError, programName);
        strcat(tempFileInfoError, ": Error opening file ");
        strcat(tempFileInfoError, tempNode.fileName);
        perror(tempFileInfoError);
        free(tempFileInfoError);
        return;
    }

    printf("\n %d:", processIndex);

    CalculateFileBits(fStream, tempNode.fileName);

    fclose(fStream);
}

void CalculateFileBits(FILE* fStream, char* fileName){
    int bytesRead = 0;
    int OnesAmount = 0;
    int ZeroesAmount = 0;
    char ch1;
    while(!feof(fStream)){
        ch1 = getc(fStream);

        for(int i = 1; i < 255; i = i * 2){
            if((ch1 & i) == i)
                ++OnesAmount;
            else
                ++ZeroesAmount;               
        }

        ++bytesRead;
    }

    printf("%d %s 1:%d 0:%d \n", getpid(), fileName, OnesAmount, ZeroesAmount);
}

void allocAndSetNewValue(char** dest, char** newValue){
    *dest =  calloc(sizeof(char), strlen(*newValue) + 1);
    char* tempFileInfoError;

    if(dest == NULL){
        tempFileInfoError = calloc(sizeof(char),256);
        strcat(tempFileInfoError, programName);
        strcat(tempFileInfoError, ": Error allocating memory ");
        perror(tempFileInfoError);
        free(tempFileInfoError);
    }

    strcpy(*dest,*newValue);
}

void FindFiles(char* directoryFullPath, Node** listHead){
    dirent* dirInfo;
    char* tempFileInfoError;

    char* result;
    struct stat fileInfo;

    DIR* tempDir;
    DIR* path = opendir (directoryFullPath);

    if(path == NULL){
        tempFileInfoError = calloc(sizeof(char), 256);
        tempFileInfoError = strcat(tempFileInfoError, programName);
        tempFileInfoError = strcat(tempFileInfoError, ": Error opening dir");
        tempFileInfoError = strcat(tempFileInfoError, directoryFullPath);
        perror(tempFileInfoError);
        free(tempFileInfoError);
        return;
    }

    NodeInfo value;
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
            tempFileInfoError = calloc(sizeof(char),256);
            strcat(tempFileInfoError, ": Couldn't reallocate memory ");
            strcat(tempFileInfoError, result);
            perror(tempFileInfoError);
            free(tempFileInfoError);
            result = calloc(1, strlen(directoryFullPath) + 1);
            allocAndSetNewValue(&result, &directoryFullPath);
            continue;
        }

        strcat(result, "/");
        strcat(result, dirInfo->d_name);
        if((dirInfo->d_type == DT_REG)){

            if(stat(result, &fileInfo) == -1){
                tempFileInfoError = calloc(sizeof(char),256);
                strcat(tempFileInfoError, programName);
                strcat(tempFileInfoError, ": Error retrieving file info ");
                strcat(tempFileInfoError, result);
                perror(tempFileInfoError);
                free(tempFileInfoError);
                allocAndSetNewValue(&result, &directoryFullPath);
                continue;
            }
                      
            value.fileName = result;
            list_insertLast(*listHead, value);
            allocAndSetNewValue(&result, &directoryFullPath);
            continue;

        }
        else if((dirInfo->d_type == DT_DIR) && (!IsDots(dirInfo->d_name))){
            FindFiles(result, listHead);
        }
        
        free(result);
        allocAndSetNewValue(&result, &directoryFullPath);
    }

    if((errno != 0) && (dirInfo == NULL)){
        tempFileInfoError = calloc(sizeof(char),256);
        strcat(tempFileInfoError, programName);
        strcat(tempFileInfoError, ": Error opening dir ");
        strcat(tempFileInfoError, directoryFullPath);
        perror(tempFileInfoError);
        free(tempFileInfoError);
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