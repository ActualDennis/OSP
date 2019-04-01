#include "list.h"
#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct dirent dirent;

static int minSize;

static int maxSize;

void FindDuplicates(char* directoryFullPath, Node** listHead);

int IsDots(char* name);

int CompareFiles(FILE* first, FILE* second);

static char* programName;

static int errors = 0;


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

    minSize = atoi(argv[2]);

    maxSize = atoi(argv[3]);

    int pathLength = strlen(argv[1]);

    char* fullFilePath = calloc(1,pathLength + 1);

    fullFilePath = strcpy(fullFilePath,argv[1]);

    Node* list = list_create();

    FindDuplicates(fullFilePath, &list);

    int listSize = list_size(list);

    FILE* firstStream;
    FILE* secondStream;
    NodeInfo tempfirstNode;
    NodeInfo tempsecondNode;
    int result = 0;

    for(int i = 0; i < listSize; ++i){

        tempfirstNode = list_get(list, i);
        firstStream = fopen(tempfirstNode.fileName, "r");

        if(firstStream == NULL){
            ++errors;
            fprintf(stderr, "%s: Error opening file %s %s\n", programName,tempfirstNode.fileName, strerror(errno));
            continue;
        }

        for(int j = i + 1; j < listSize; ++j){
            fseek(firstStream, 0, 0);
            tempsecondNode = list_get(list, j);
            secondStream = fopen(tempsecondNode.fileName, "r");

            if(secondStream == NULL){
                continue;
            }
            
            int res = CompareFiles(firstStream, secondStream);
            if(res){
                printf("%s %s\n", tempfirstNode.fileName, tempsecondNode.fileName);
                ++result;
            }

            fclose(secondStream);
        }

        fclose(firstStream);
    }

    printf("\n%d\n", result);
    printf("\n%d\n", errors);

}

void allocAndSetNewValue(char** dest, char** newValue){
    *dest =  calloc(sizeof(char), strlen(*newValue) + 1);
    char* tempFileInfoError;

    if(dest == NULL){
        fprintf(stderr, "%s: Error allocating memory %s\n", programName, strerror(errno));
        return;
    }

    strcpy(*dest,*newValue);
}

void FindDuplicates(char* directoryFullPath, Node** listHead){
    dirent* dirInfo;
    char* tempFileInfoError;

    char* result;
    struct stat fileInfo;

    DIR* tempDir;
    DIR* path = opendir (directoryFullPath);

    if(path == NULL){
        fprintf(stderr, "%s: Error opening dir %s %s\n", programName,directoryFullPath, strerror(errno));
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
            

            if((fileInfo.st_size > minSize) && (fileInfo.st_size < maxSize)){               
                value.fileName = result;
                list_insertLast(*listHead, value);
                allocAndSetNewValue(&result, &directoryFullPath);
                continue;
            }
        }
        else if((dirInfo->d_type == DT_DIR) && (!IsDots(dirInfo->d_name))){
            FindDuplicates(result, listHead);
        }
        
        free(result);
        allocAndSetNewValue(&result, &directoryFullPath);
    }

    if(closedir(path) == -1){
         fprintf(stderr, "%s: Error closing dir %s\n", programName, strerror(errno));
    }

    if((errno != 0) && (dirInfo == NULL)){
        fprintf(stderr, "%s: Error opening dir %s %s\n", programName,directoryFullPath, strerror(errno));
    }
}

int CompareFiles(FILE* first, FILE* second){
    char ch1 = getc(first); 
    char ch2 = getc(second);    

    while (ch1 != EOF && ch2 != EOF) 
    {             
        if (ch1 != ch2) 
        { 
            return 0;
        } 
    
        // fetching character until end of file 
        ch1 = getc(first); 
        ch2 = getc(second); 
    }

    if((ch1 == EOF) && (ch2 == EOF))
        return 1;

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
