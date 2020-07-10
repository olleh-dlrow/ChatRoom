#include <ncurses.h>
#include <string.h>

int main(){
    char mesg[] = "Just a string";
    int row,col;
    initscr();
    getmaxyx(stdscr,row,col);
    mvprintw(row/2,(col - strlen(mesg))/2,"%s",mesg);
    mvprintw(row - 2,0,"this screen has %d rows and %d cols\n",row,col);
    printw("end it");
    refresh();
    getch();
    endwin();
    return 0;
}