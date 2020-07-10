#ifndef _UDP_EPOLL_H
#define _UDP_EPOLL_H

int udp_connect(struct sockaddr_in *client);
int udp_accept(int fd, struct User *user);
void add_event_ptr(int epollfd, int fd, int events, struct User *user);
void del_event(int epollfd, int fd);
int find_sub(struct User *team);
void add_to_sub_reactor(struct User *user);
int check_online(struct LogRequest *request);
#endif