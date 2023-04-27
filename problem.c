#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>


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

void processFileOptions(struct stat status, char *filePath) {
    const int MAX_OPTIONS = 5;
    const char* VALID_OPTIONS = "ndhmal";
    char options[MAX_OPTIONS + 1];
    int isValid = 0;

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
                printf("Name of file: %s\n", filePath);
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
                printf("Name of directory: %s\n", filePath);
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
                printf("Name of link: %s\n", filePath);
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

int main() {
    char path[256];
    struct stat status;
    DIR *dirp;
    struct dirent *dp;

    printf("Please enter the file or directory path or link paths: "); 
    if (scanf("%255s", path) != 1) {
        perror("Scanf failed.\n");
        return 1;
    }

    if (stat(path, &status) != 0) {
        perror("Failed to get file status.\n");
        return 1;
    }

    if (S_ISREG(status.st_mode)) {
        processFileOptions(status, path);
    } else if (S_ISDIR(status.st_mode)) {
        processDirectoryOptions(status, path);

        dirp = opendir(path);
        if (dirp == NULL) {
            perror("Failed to open directory.\n");
            return 1;
        }

        int cCount = 0;
        int totalSize = 0;
        while ((dp = readdir(dirp)) != NULL) {
            if (dp->d_type == DT_REG && strstr(dp->d_name, ".c") != NULL) {
                cCount++;
                char filePath[256];
               // snprintf(filePath, 256, "%s/%s", path, dp->d_name);
               snprintf(filePath, PATH_MAX, "%s/%s", path, dp->d_name);

                struct stat fileStatus;
                if (stat(filePath, &fileStatus) != 0) {
                    perror("Failed to get file status.\n");
                    return 1;
                }

                totalSize += fileStatus.st_size;
            }
        }
        closedir(dirp);

        printf("Number of C files: %d\n", cCount);
        printf("Total size of C files: %d bytes\n", totalSize);
    } else if (S_ISLNK(status.st_mode)) {
        printf("The file is a symbolic link.\n");
    } else {
        printf("The file is neither a regular file, a directory, nor a symbolic link.\n");
    }

    return 0;
}



