#include "head.h"
#include "common.h"

int main(int argc, char *argv[])
{
    int port, server_listen, maxfd;
    if (argc != 2)
    {
        fprintf(stderr, "Usage : %s port!\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);
    if ((server_listen = socket_create(port)) < 0)
    {
        perror("socket_create()");
        exit(1);
    }
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(server_listen, &rfds);
    maxfd = server_listen;
    //struct timeval tv;
    while (1)
    {
        int retval = select(maxfd + 1, &rfds, NULL, NULL, NULL);
        if (retval < 0)
        {
            perror("select()");
            exit(1);
        }
        else
        {
            if (FD_ISSET(server_listen, &rfds))
            {
                int new_fd = accept(server_listen, NULL, NULL);
                if (new_fd < 0)
                {
                    perror("accept");
                    exit(1);
                }
                char msg[512] = {0};
                sprintf(msg, "%s is always %d years old!", "chen", 18);
                send(new_fd, msg, strlen(msg), 0);
                close(new_fd);
            }
        }
    }

    return 0;
}