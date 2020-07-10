#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    int opt, flag = 0;
    char msg[512] = {0};
    while ((opt = getopt(argc, argv, "m:")) != -1) {
        switch(opt) {
            case 'm':
                flag = 1;
                strcpy(msg, optarg);
                break;
            default:
                fprintf(stderr, "Usage : %s -m msg\n", argv[0]);
                exit(1);
        }
    }

    if (flag == 1) {
        printf("msg = %s\n", msg);
        exit(0);
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork()");
    }
    if (pid == 0) {
        printf("In Child Process!\n");
        //vim
        execlp("vim", "vim", "tmp.txt", NULL);
        sleep(3);
    } else {
        printf("Waiting for Child exit.\n");
        wait(NULL);
        printf("After Child exit.\n");
        printf("Message in File:\n");
        execl("/bin/cat", "cat", "tmp.txt", NULL);
    }
    return 0;
}