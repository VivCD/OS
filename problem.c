#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <dirent.h>
#include<errno.h>
#include<sys/types.h>


#define BUFFER_SIZE 1024



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
    fileName=strrchr(filePath,'/'); 
    fileName=fileName+1;
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
 //  exit(1);
}

void processDirectoryOptions(struct stat status, char *filePath) {
    const int MAX_OPTIONS = 3;
    const char* VALID_OPTIONS = "ndac";
    char options[MAX_OPTIONS + 1];
    int isValid = 0;
    char *dirName;
    dirName=strrchr(filePath,'/');
    dirName=dirName+1;
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
   // exit(2);
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
   // exit(3);
}
int count_lines(const char* filePath) 
 {
    FILE* file = fopen(filePath, "r");
    if (file == NULL) 
    {
        printf("Unable to open file '%s'\n", filePath);
        return -1;
    }

    int lines = 1;
    char c;
    while ((c = fgetc(file)) != EOF) 
    {
        if (c == '\n') 
        {
            lines++;
        }
    }

    fclose(file);
    return lines;
}


int main(int argc, char *argv[]) {
    int fp = open("grades.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR);
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
                  char *fileName2;
                  fileName2=strrchr(filePath,'/');
                  fileName2=fileName2+1;
                  int pid_count = 0;
                  pid_t pid_file=fork();
                  if(pid_file < 0){
                      perror("no process was created for files");
                      exit(2);
                   }
                  else if(pid_file == 0){
                    // Child process for file options
                    processFileOptions(status, filePath);
                    int lines = count_lines(filePath);
                    if (lines >= 0 && ((fileName2-1) =="c" && (fileName2-2) ==".") )
                    {
                        printf("File '%s' has %d lines\n", fileName2, lines);
                    }
                    exit(3);
                   }
                 
                  else{
                    pid_count++;

                    int pipefd[2];
                    if (pipe(pipefd) == -1)
                    {
                        perror("pipe create went wrong");
                        exit(4);
                    }

                    pid_t pid_script = fork();

                    if (pid_script < 0 )
                    {
                        perror("there is no process created");
                        exit(5);
                    }
                
                    else if ( pid_script == 0)
                    {
                        if(fileName2[strlen(fileName2)-1]=='c' && fileName2[strlen(fileName2)-2]=='.')
                        {

                            if( close(pipefd[0] == -1) )
                            {
                                perror("error - pipe/read");
                                exit(6);
                            } 
                           
                            if (dup2(pipefd[1], STDOUT_FILENO) == -1) 
                            {
                                perror("error - dup2/write");
                                exit(7);
                            }

                            char scriptname[10] = "script.sh";
                            execlp("bash", "bash", scriptname, fileName2, NULL);
                            exit(8);
                        }
                        else
                        {
                            int lines = count_lines(fileName2);
                            if (lines >= 0) 
                            {
                                printf("File '%s' has %d lines\n", fileName2, lines);
                            }
                            exit(9);
                        }
                    }
                    else
                    {
                        pid_count++;
                        //printf("count %d",pid_count_reg);
                        pid_t wpid;
                        int statusFile;
                        for( int i = 0; i < pid_count; i++)
                        {
                            wpid = wait(&statusFile);
                            if (WIFEXITED(statusFile))
                            {
                                printf("\n");
                                printf("Child process with PID %d exited with status %d\n", wpid, WEXITSTATUS(statusFile));
                            }
                            else
                                {
                                    printf("\n");
                                    printf("Child process  with PID %d did not exit normally\n", wpid);
                                }     
                        }

                        if( close(pipefd[1]) == -1)
                        {
                            perror("error - pipe/write");
                            exit(10);
                        } 

                        char buffer[BUFFER_SIZE];
                       
                        int i;
                        for(i = 0; i <= 1; i++) 
                        {
                            char c;
                            read(pipefd[0], &c, 1);
                            buffer[i] = c;

                        }
                        buffer[i] = '\0';
                        //printf("%s\n", buffer);

                        int nr = atoi(buffer);
                        int nr_errors = nr / 10;
                        int nr_warnings = nr % 10;
                        int grade;
                        if (nr_errors == 0 && nr_warnings == 0)
                                grade = 10;
                        else if (nr_errors == 0 && nr_warnings > 10)
                                grade = 2;
                        else if (nr_errors == 0 && nr_warnings < 10)
                                grade = 2 + 8 * (10 - nr_warnings) / 10;
                        else if (nr_errors > 0)
                                grade = 1;

                        if( close(pipefd[0]) )
                        {
                            perror("error - pipe/read");
                            exit(11);
                        }
                        //printf("Code score for %s: %d errors, %d warnings\n", filename, nr_errors, nr_warnings);

                        char gradeString[100];
                        sprintf(gradeString, "Grade for file %s is: %d\n", fileName2, grade);
                        strcat(gradeString, "\0");
                        write(fp, gradeString, strlen(gradeString));
                    }
                }

             break;
            case S_IFDIR:
                 pid_t pid_dir=fork();
                 if(pid_dir < 0){
                      perror("no process was created for dir");
                      exit(12);
                   }
                 else if(pid_dir == 0){
                    // Child process for file options
                     processDirectoryOptions(status, filePath);
                  }
                 else{
                    pid_t pid_make=fork();
                    if(pid_make < 0){
                        perror("no process was created for sec child dir");
                          exit(13);
                     }
                    if(pid_make == 0){
                        char *dirName2;
                        dirName2=strrchr(filePath,'/');
                        dirName2=dirName2+1;
                        char *cmd = "touch";
                        char filenameDir[strlen(dirName2) + 10];
                        sprintf(filenameDir, "%s_file.txt", dirName2);
                        execlp(cmd, cmd, filenameDir, (char *) NULL);
                        exit(14);
                    }
                    else {
                        int statusDIR;
                        waitpid(pid_dir,&statusDIR,0);
                      if(WIFEXITED(statusDIR)){
                         printf("\n");
                         printf("child process la dir exited with status de campion %d \n",WEXITSTATUS(statusDIR));
                         
                            
                        }
                      else{
                            printf("\n");
                           printf("child process la dir o luat o pe aratura \n");
                        }
                    
                          
                     } 
                  
                 
               }
                break;
            case S_IFLNK:
               
                 pid_t pid_link=fork();
                 if(pid_link<0){
                   perror("no process was created for links");
                   exit(15);
                  }
                 else if(pid_link == 0){
                    // Child process for file options
                    processLinkOptions(status, filePath); 
                    printf("aici dau exist");
                  }
                 else {
                     pid_t pid_changeAcces = fork();
                     if(pid_changeAcces<0){
                         perror("no process was created for sec child symlink");
                          exit(16);
                         }  
                     if(pid_changeAcces == 0){
                          // get path of target file
                          //  printf("am intrat");
                          char targetPath[1024];
                          ssize_t len = readlink(filePath, targetPath, sizeof(targetPath) - 1);
                          if(len == -1){
                              perror("readlink failed");
                              exit(12);
                               }
                               targetPath[len] = '\0';

                              // change permissions of target file
                           if(chmod(targetPath, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH) == -1){
                               perror("chmod failed");
                               exit(13);
                               }
                            // exit(20);
                        }
                    else{
                      int statusLink;
                      waitpid(pid_link,&statusLink,0);
                      if(WIFEXITED(statusLink)){
                         printf("\n");
                         printf("child process la symlink exited with status de campion %d \n",WEXITSTATUS(statusLink)); 
                        }
                      else{
                             printf("\n");
                             printf("child process la symlinks o luat o pe aratura \n");
                            
                        }
                    
                     } 
                
               }
                 break;
            default: 
            printf("we don t know ce cacat de file e asta");
            break;
             
        } 
        
   
} return 0;
}


