#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

//#define IS_REG(mode) ((mode) & S_IFREG)

DIR *dir;
FILE *fisOut;

int main(int argc, char *argv[]){
    if(argc != 3){
        perror("Invalid number of arguments!\n");
        exit(0);
    }
    if((dir = opendir(argv[1])) <0 ){
        perror("Cannot open folder!\n");
        exit(1);
    }
    if((fisOut = open(argv[2], O_WRONLY)) <0 ){
         perror("Cannot open file!\n");
        exit(1);
    }

    struct stat info;

    if(fstat(fisOut, &info)){
        perror("NO!\n");
        exit(2);
    }
    if(!(S_ISREG(info.st_mode))){
        perror("NO1!\n");
        exit(2);
    }

    struct dirent *entry;
    char path[200];
    struct stat infoENT;
    char buffer[200];

    while((entry = readdir(dir)) != 0){
        sprintf(path, "%s/%s", argv[1], entry->d_name);
        printf("%s", path);
        if(lstat(path,&infoENT)){
            perror("NO2!\n");
            exit(2);
        }
        int len2;
        if(S_ISREG(infoENT.st_mode)){
            len2 = sprintf(buffer, " n:%s no:%d ", path, infoENT.st_blksize);
        
            int out;
            if((out = write(fisOut,buffer, len2)) < 0){
                perror("NO3!\n");
                exit(2);
            }
        }
        
    }
}