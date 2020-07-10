#include "head.h"

int socket_udp(){
    int fd;
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        return -1;
    }
    return fd;
}