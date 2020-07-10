#ifndef _COMMON_H
#define _COMMON_H
int socket_create(int port);
void make_block(int fd);
void make_non_block(int fd);
#ifdef _D
#define DBG(fmt,args...) printf(fmt,##args)
#else
#define DBG(fmt,args...)
#endif

#endif