#ifndef DATATYPE_H
#define DATATYPE_H

#define MAX 300 //用户数的1/2
#define NTHREAD 3

#define MAX_EVENTS 250

//球的位置
struct Bpoint {
    double x;
    double y;
};

struct Speed {
    double x;
    double y;
};

struct Aspeed {
    double x;
    double y;
};

//球的状态
struct BallStatus {
    struct Speed v;
    struct Aspeed a;
    int who;       //队伍
    char name[20]; //踢球人
};

struct Point {
    int x;
    int y;
};

struct User {
    int team;
    int fd; //该玩家对应的连接
    char name[20];
    int online; // 1 在线 0 不在线
    int flag;   //未相应次数
    struct Point loc;
};

struct Score {
    int red;
    int blue;
};

//定义登录相关
struct LogRequest {
    char name[20];
    int team;
    char msg[512];
};

struct LogResponse {
    int type; // 0 OK 1 NO:不能再连接了
    char msg[512];
};

//游戏期间交互
#define MAX_MSG 4096
//日常的消息交互，如聊天信息为512长度

#define ACTION_KICK 0x01
#define ACTION_CARRY 0x02
#define ACTION_STOP 0x04

struct Ctl {
    int action;
    int dirx;
    int diry;
    int strength; //踢球力度
};

#define FT_HEART 0x01 //心跳
#define FT_WALL 0x02  //公告
#define FT_MSG 0x04   //聊天信息
#define FT_ACK 0x08   // ack
#define FT_CTL 0x10   //控制信息
#define FT_GAME 0x20  //游戏场景
#define FT_SCORE 0x40 //比分变化
#define FT_GAMEOVER 0x80
#define FT_FIN 0x100 //离场

struct FootBallMsg {
    int type; // type & FT_HEART !=0 表示包含心跳
    int size;
    int team;
    char name[20];
    char msg[MAX_MSG];
    struct Ctl ctl;
};

#define CHAT_FIN 0x01  //断开连接
#define CHAT_HEART 0x02 
#define CHAT_ACK 0x04
#define CHAT_WALL 0x08  //公聊
#define CHAT_MSG 0x10   //私聊
#define CHAT_FUNC 0x20  //功能

struct ChatMsg {
    int type; //
    char msg[1024];
};

struct Map {
    int width;
    int height;
    struct Point start;
    int gate_width;
    int gate_height;
};

#endif