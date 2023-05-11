#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include<errno.h>
#include <unistd.h>
#include <unistd.h>
#include<sys/types.h>

char *getFileExtension(const char *filename){
    char *extension = strrchr(filename, '.');
    if (extension == NULL) {
        return "";
    }
    return extension + 1;
}
void printMenu1() {
    printf("----FILE-MENU----\n");
    printf("\tm - time of last modification\n");
    printf("\tl - create symbolic link\n");
    printf("\tn - name\n");
    printf("\td - size\n");
    printf("\ta - access rights\n");
    printf("\th - hard link count\n");
    printf("Choose your weapon\n");
    printf("Activate it by typing '-' and the weapon of choice \n");
}

void printMenu2() {
    printf("----DIRECTORY-MENU----\n");
    printf("\tc - number of C file entries\n");
    printf("\td - size\n");
    printf("\tn - name\n");
    printf("\ta - access rights\n");
    printf("Choose your weapon\n");
    printf("Activate it by typing '-' and the weapon of choice \n");
}

void printMenu3() {
    printf("----LINK-MENU----\n");
    printf("\td - size\n");
    printf("\tn - name\n");
    printf("\tl - delete\n");
    printf("\ta - access rights\n");
    printf("\tt - size of target file\n");
    printf("Choose your weapon\n");
    printf("Activate it by typing '-' and the weapon of choice \n");
}

void printAccessRights(struct stat status, char *filePath) {
    printf("\nAccess Rights for %s:\n", filePath);

    char *groups[] = {"User", "Group", "Others"};
    int modes[] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
    char *permissions[] = {"Read", "Write", "Execute"};

    for (int i = 0; i < 3; i++) {
        printf("%s:\n", groups[i]);
        for (int j = 0; j < 3; j++) {
            int k = i * 3 + j;
            printf("\t%s - %s\n", permissions[j], (status.st_mode & modes[k]) ? "yes" : "no");
        }
        printf("\n");
    }
}

void processFileOptions(struct stat status, char *filePath) { // merge cu -nhsa type of input but still not work is -mna sak does not display error messege
    const int MAX_OPTIONS = 5;
    const char* VALID_OPTIONS = "ndhmal";
    char options[MAX_OPTIONS + 1];
    int isValid = 0;
    char *fileName;
    fileName=strrchr(filePath,'/'); //fa cu strcat sa scapi de prima linie
    do {
        if (!isValid) {
            printMenu1();
        }
        
        if (scanf("%5s", options) != 1) {
            perror("Scanf failed.\n");
        }

        isValid = 1;
        int nrOfOptions = strlen(options);

        if (nrOfOptions < 2 || options[0] != '-') {
            printf("U cannot even choose corectly :)) \n");
            isValid = 0;
            continue;
        }
        
        for (int i = 1; i < nrOfOptions; i++) {
            if (strchr(VALID_OPTIONS, options[i]) == NULL) {
                printf("Can u even see the obtions?.\n");
                isValid = 0;
                break;
            }
        }
    } while (!isValid);

    for (int i = 1; i < strlen(options); i++) {
    
        switch (options[i]) {
            case 'n':
                printf("Name of file: %s\n",fileName);
                break;
            case 'd':
                printf("File size: %ld bytes\n", status.st_size);
                break;
            case 'h':
                printf("Hard link count: %ld\n", status.st_nlink);
                break;
            case 'm':
                printf("Time of last modification: %s\n", ctime(&status.st_mtime));
                break;
            case 'a':
                printf("Access rights:\n");
                printAccessRights(status, filePath);
                break;
            case 'l':
                printf("Please enter the link name: ");
                char linkname[256];
                if (scanf("%255s", linkname) != 1) {
                    perror("Scanf failed.\n");
                } else {
                    if (symlink(filePath, linkname) == -1) {
                        perror("Failed to create the symbolic link.\n");
                    } else {
                        printf("Symbolic link was created: from %s to %s\n", linkname, filePath);
                    }
                }
                break;
            default:
                printf("Invalid option: %c\n", options[i]);
                break;
        }
    }
}

void processDirectoryOptions(struct stat status, char *filePath) {
    const int MAX_OPTIONS = 3;
    const char* VALID_OPTIONS = "ndac";
    char options[MAX_OPTIONS + 1];
    int isValid = 0;
    char *dirName;
    dirName=strrchr(filePath,'/');
    do {
        if (!isValid) {
            printMenu2();
        }
        
        if (scanf("%3s", options) != 1) {
            perror("Scanf failed.\n");
        }

        isValid = 1;
        int nrOfOptions = strlen(options);

        if (nrOfOptions < 2 || options[0] != '-') {
             printf("U cannot even choose corectly :)) \n");
            isValid = 0;
            continue;
        }
        
        for (int i = 1; i < nrOfOptions; i++) {
            if (strchr(VALID_OPTIONS, options[i]) == NULL) {
                printf("Can u even see the obtions?.\n");
                isValid = 0;
                break;
            }
        }
    } while (!isValid);

    for (int i = 1; i < strlen(options); i++) {
        switch (options[i]) {
            case 'n':
                printf("Name of directory: %s\n", dirName);
                break;
            case 'd':
                printf("Directory size: %ld bytes\n", status.st_size);
                break;
            case 'a':
                printf("Access rights:\n");
                printAccessRights(status, filePath);
                break;
            case 'c':
                {
                    int countC = 0;
                    DIR* directory = opendir(filePath);
                    if (directory == NULL) {
                        perror("Could not open the directory.\n");
                        break;
                    }

                    struct dirent *entry;
                    while ((entry = readdir(directory)) != NULL) { 
                        struct stat entryStat;
                        char entryPath[PATH_MAX];
                        snprintf(entryPath, sizeof(entryPath), "%s/%s", filePath, entry->d_name);

                        if (lstat(entryPath, &entryStat) == -1) {
                            perror("lstat failed.\n");
                            break;
                        } 

                        if (S_ISREG(entryStat.st_mode) && (strcmp(getFileExtension(entry->d_name), "c") == 0)) {
                            countC++;
                        }
                    }
                    closedir(directory);
                    printf("The number of C files in the directory is %d.\n", countC);
                }
                break;
            default:
                printf("Invalid option: %c\n", options[i]);
                break;
        }
    }
}

void processLinkOptions(struct stat status, char *filePath) {
    const int MAX_OPTIONS = 5;
    const char* VALID_OPTIONS = "ndtal";
    char options[MAX_OPTIONS + 1];
    int isValid = 0;
    char *linkName;
    linkName=strrchr(filePath,'/');
    do {
        if (!isValid) {
            printMenu3();
        }
        
        if (scanf("%5s", options) != 1) {
            perror("Scanf failed.\n");
        }

        isValid = 1;
        int nrOfOptions = strlen(options);

        if (nrOfOptions < 2 || options[0] != '-') {
            printf("U cannot even choose corectly :)) \n");
            isValid = 0;
            continue;
        }
        
        for (int i = 1; i < nrOfOptions; i++) {
            if (strchr(VALID_OPTIONS, options[i]) == NULL) {
                printf("Can u even see the obtions?.\n");
                isValid = 0;
                break;
            }
        }
    } while (!isValid);

    for (int i = 1; i < strlen(options); i++) {
        switch (options[i]) {
            case 'n':
                printf("Name of link: %s\n", linkName);
                break;
            case 'd':
                printf("Link size: %ld bytes\n", status.st_size);
                break;
            case 't':
                struct stat targetSt;
                if (stat(filePath, &targetSt) == -1) {
                    perror("Stat failed.\n");
                    break;
                }
                printf("Target file size: %ld bytes\n", targetSt.st_size);
                break;
            case 'a':
                printf("Access rights:\n");
                printAccessRights(status, filePath);
               break;
            case 'l':
                printf("Deleting link...\n");
                if (unlink(filePath) == -1) {
                    perror("Failed to remove the link.\n");
                } else {
                    printf("Link removed successfully.\n");
                }
                break;
            default:
                printf("Invalid option: %c\n", options[i]);
                break;
        }
    }
    exit(33);
}

int main(int argc, char *argv[]) {
    if (argc < 1) {
        printf("remember that this fking project has to do something with somethinggggg \n ");
        exit(78);
    }

     for (int i = 1; i < argc; i++) 
     {
        char* filePath = argv[i];
        struct stat status;
        
        if (lstat(filePath, &status) == -1) {
        perror("Failed to get file status.\n");
        exit(1);
    }

    printf("Successfully opened %s\n", filePath);
        switch (status.st_mode & S_IFMT) 
        {
            case S_IFREG:
              {
                    // Child process for file options
                    processFileOptions(status, filePath);
               break;
             }
            
            
            case S_IFDIR:
              {
                // pid_t pid_dir=
                    // Child process for file options
                   processDirectoryOptions(status, filePath);
               
               }
            case S_IFLNK:
               {
               pid_t pid_link=fork();
               if(pid_link<0){
                perror("no process was created");
                exit(12);
                }
                else if(pid_link==0){
                    // Child process for file options
                    processLinkOptions(status, filePath); 
                    printf("aici dau exist");
                   // exit(23);
                   // break;
                    
               }
               else {
                     pid_t pid_changeAcces = fork();
                     if(pid_changeAcces<0){
                         perror("no process was created");
                          exit(14);
                      }
                      if(pid_changeAcces == 0){
                         // get path of target file
                       //  printf("am intrat");
                         char targetPath[1024];
                          ssize_t len = readlink(filePath, targetPath, sizeof(targetPath) - 1);
                          if(len == -1){
                              perror("readlink failed");
                              exit(18);
                               }
                               targetPath[len] = '\0';

                              // change permissions of target file
                           if(chmod(targetPath, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH) == -1){
                               perror("chmod failed");
                               exit(18);
                               }
                            // exit(20);
                      }
                
                 
                 int status;
                 waitpid(pid_link,&status,0);
                        if(WIFEXITED(status)){
                            printf("\n");
                            printf("child process exited with status de campion %d \n",WEXITSTATUS(status));
                          // exit(8); 
                            
                        }
                        else{
                             printf("\n");
                             printf("child process o luat o pe aratura \n");
                            // exit(6);
                        }
                    
                 } break;
                 }
              
            default: 
            printf("we don t know ce cacat de file e asta");
            break;
    } 
             } 
        
   
    return 0;
}



