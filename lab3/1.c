#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include "arraylist.h"


typedef struct dirent dirent;

void FindFiles(char* directoryFullPath,  arraylist **listHead);

void CalculateFileBits(FILE* fStream, char* fileName);

void ProcessFile(int index, arraylist* list, int processIndex);

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

    arraylist *List = arraylist_create();

    FindFiles(fullFilePath, &List);

    int listSize = List->size;

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
            ProcessFile(i, List, curr_processes_amount);
            break;
        }
    
    }


}

void ProcessFile(int index, arraylist* list, int processIndex){
    char* firstFName;
    FILE* fStream;
    char* tempFileInfoError;

    firstFName = arraylist_get(list, index);
    fStream = fopen(firstFName, "r");

    if(fStream == NULL){
        fprintf(stderr, "%s: Error opening file %s %s\n", programName, firstFName, strerror(errno));
        return;
    }

    printf("%d:", processIndex);

    CalculateFileBits(fStream, firstFName);

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

    printf("%d %s %d 1:%d 0:%d \n", getpid(), fileName, bytesRead, OnesAmount, ZeroesAmount);
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

void FindFiles(char* directoryFullPath,  arraylist **listHead){
    dirent* dirInfo;
    char* tempFileInfoError;

    char* result;
    struct stat fileInfo;

    DIR* tempDir;
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

            arraylist_add(*listHead, result);
            allocAndSetNewValue(&result, &directoryFullPath);
            continue;

        }
        else if((dirInfo->d_type == DT_DIR) && (!IsDots(dirInfo->d_name))){
            FindFiles(result, listHead);
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

int IsDots(char* name){
    if(!strcmp(name, ".")){
        return 1;
    }

    if(!strcmp(name, "..")){
        return 1;
    }

    return 0;
}