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
}

int main(int argc, char *argv[]) {
    if (argc < 1) {
        printf("remember that this fking project has to do something with somethinggggg \n ");
        exit(0);
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
                int pipefd[2];
                if (pipe(pipefd) == -1) {
                     perror("pipe");
                     exit(EXIT_FAILURE);
                 }
                 pid_t pidreg1 = fork();
                 if (pidreg1 == 0) {
                    // Child process for file options
                     processFileOptions(status, filePath);
                     break;
                   }
                 pid_t pidreg2 = fork();
                 if (pidreg2 == 0) {
                       if ((strlen(filePath) - 1) == 'c' && (strlen(filePath) - 2)== '.') {
                            close(pipefd[0]);               // Close read end of pipe
                            dup2(pipefd[1], STDOUT_FILENO); // Redirect standard output to write end of pipe
                            close(pipefd[1]);               // Close write end of pipe
                            char scriptname[10] = "script.sh";
                            execlp("bash", "bash", scriptname, filePath, NULL);
                            exit(EXIT_FAILURE);
                          } 
                         else {
                        char scriptname1[10] = "script1.sh";
                        execlp("bash", "bash", scriptname1, filePath, NULL);
                        exit(EXIT_FAILURE);

                           }
                     }
               break;
    
             }
            
            
            case S_IFDIR:
              {
            
                    // Child process for file options
                   processDirectoryOptions(status, filePath);
               
               }
            case S_IFLNK:
               {
             
                    // Child process for file options
                    processLinkOptions(status, filePath);
                    break;
               }
        
        } 
    }
   

    
       
    
    return 0;
}


