#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(){
    int fd;
    char *buf[100];

    fd = open("test.txt", O_CREAT | O_WRONLY, 0644);
    if(fd == -1){
        perror("Error opening file");
        return 1;
    }

    write(fd, "Hello World!\n", 13);
    close(fd);

    fd = open("test.txt", O_RDONLY);
    if(fd == -1){
        perror("Error opening file");
        return 1;
    }

    read(fd, buf, sizeof(buf));
    // printf("%s", buf);
    close(fd);

    pid_t pid = fork();
    if(pid == -1){
        perror("Error forking");
        return 1;
    }

    if(pid == 0){
        printf("Child process\n");
        execlp("ls", "ls", "-l", NULL);
        perror("Error executing ls");
        exit(1);
    } else {
        wait(NULL);
        printf("Child process completed\n");
    }

    return 0;
}
