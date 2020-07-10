#ifndef _COMMON_H
#define _COMMON_H

int socket_create(int port);
void make_block(int fd);
void make_non_block(int fd);
char *get_conf_value(const char *path, const char *key);
#ifdef _D
#define DBG(fmt,args...) printf(fmt,##args)
#else
#define DBG(fmt,args...)
#endif

#endif