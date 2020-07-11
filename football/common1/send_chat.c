/*************************************************************************
	> File Name: send_chat.c
	> Author: 
	> Mail: 
	> Created Time: Fri 10 Jul 2020 11:58:58 PM CST
 ************************************************************************/

#include "head.h"
extern int sockfd;
extern WINDOW *input_sub, *input_win, *message_sub;


void send_chat() {
    struct ChatMsg msg;
    echo();
    nocbreak();
    bzero(&msg, sizeof(msg));
    msg.type = CHAT_WALL;
    w_gotoxy_puts(input_win, 1, 1, "Input Message : ");
    //show_message(message_sub, &msg, 1);
    wrefresh(input_win);
    mvwscanw(input_win, 2, 1, "%[^\n]s", msg.msg);
    if (strlen(msg.msg)) {
        if (msg.msg[0] == '@') msg.type = CHAT_MSG;
        if (msg.msg[0] == '#') msg.type = CHAT_FUNC;
        send(sockfd, (void *)&msg, sizeof(msg), 0);
    }
    wclear(input_win);
    box(input_win, 0, 0);
    wrefresh(input_win);
    noecho();
    cbreak();
}
