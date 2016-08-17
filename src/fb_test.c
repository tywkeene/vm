#include <ncurses.h>
#include "video.h"

extern int max_x;
extern int max_y;

#define getmaxxy(scr,x,y) getmaxyx(scr,y,x)

int main(){
    init_video();
    getmaxxy(stdscr, max_x, max_y);
    alloc_video_memory(max_x,max_y);
    clear_framebuffer();
    printvmf("%s derp\n", "herp");
    getchar();
    printvmf("derp\n");
    getchar();
    printvmf("derp\n");
    getchar();
    printvmf("derp\n");
    getchar();
    endwin();
}
