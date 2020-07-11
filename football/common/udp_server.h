#ifndef UDP_SERVER_H
#define UDP_SERVER_H

int socket_create_udp(int port);
void sendtoall(struct ChatMsg *msg);
void sendto1(struct ChatMsg *msg,struct User *user);
void do_func(struct ChatMsg *msg,struct User *user);
#endif
