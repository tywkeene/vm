#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "video.h"

extern char ***video_ram;
int cursor_x = 0;
int cursor_y = 0;

void draw_screen(void){
    for(int x = 0; x < max_x; x++)
        for(int y = 0; y < max_y; y++)
            mvprintw(y, x, video_ram[x][y]);
    refresh();
}

void alloc_video_memory(int max_x, int max_y){
    video_ram = malloc(sizeof(char *));
    for(int x = 0; x < max_x; x++){
        video_ram[x] = malloc(sizeof(char *) * max_x);
        for(int y = 0; y < max_y; y++){
            video_ram[x][y] = (char *) malloc(sizeof(char *));
        }
    }
}

char ***new_framebuffer(void){
    char ***buffer = malloc(sizeof(char *));
    for(int x = 0; x < max_x; x++){
        buffer[x] = malloc(sizeof(char *) * max_x);
        for(int y = 0; y < max_y; y++){
            buffer[x][y] = (char *) malloc(sizeof(char *));
        }
    }
    clear_framebuffer(buffer);
    return buffer;
}

void clear_framebuffer(char ***buffer){
    cursor_x = 0;
    cursor_y = 0;
    for(int x = 0; x < max_x; x++){
        for(int y = 0; y < max_y; y++){
            memset(buffer[x][y], 0, 1);
        }
    }
    draw_screen();
}

void new_line(void){
    cursor_x = 0;
    if(cursor_y == (cursor_y-1)){
        cursor_y = 0;
        return;
    }
}

void load_video_ram(int x, int y, char *value){
    memcpy(video_ram[x][y], value, 1);
    draw_screen();
}

void init_video(){
    initscr();
    noecho();
    curs_set(TRUE);
}

void putch(char val){
    cursor_x++;
    if(cursor_y == (max_y-1)){
        video_ram = new_framebuffer();
        cursor_y = 0;
        cursor_x = 0;
        clear_framebuffer(video_ram);
    }else if(cursor_x == (max_x-1) || val == '\n'){
        cursor_x = 0;
        cursor_y++;
    }
    load_video_ram(cursor_x, cursor_y, &val);
}

void putstr(const char *str){
    while(*str){
        putch(*str);
        str++;
    }
    return;
}

void putoct(uint32_t val, uint16_t width, uint8_t zeropad ){
    if(!width){
        char c[11];
        uint32_t i = 0;
        do{
            c[i++] ='0'+(val&0x07);
            val >>= 3;
        }
        while(val != 0);
        while(i > 0)
            putch(c[--i]);
    }else{
        char c[width];
        uint32_t i = 0;
        do{
            c[i++] ='0'+(val&0x07);
            val >>= 3;
        }
        while((val != 0) && (i < width));
        while(i < width)
            c[i++] = (zeropad)?'0':' ';
        while(i > 0)
            putch(c[--i]);
    }
    return;
}

void putdec(int val, uint16_t width, uint8_t zeropad)
{
    uint8_t isneg = (val < 0);
    val = abs(val);
    if ( !width ){
        char c[10];
        uint32_t i = 0;
        do{
            c[i++] = '0'+val%10;
            val /= 10;
        }
        while ( val != 0 );
        if ( isneg )
            putch('-');
        while ( i > 0 )
            putch(c[--i]);
    }
    else
    {
        char c[width];
        uint32_t i = 0;
        do{
            c[i++] = '0'+val%10;
            val /= 10;
        }
        while ( (val != 0) && (i < width) );
        if ( isneg )
            putch('-');
        while ( i < width )
            c[i++] = (zeropad)?'0':' ';
        while ( i > 0 )
            putch(c[--i]);
    }
}

void puthex(uint32_t val, uint16_t width, uint8_t zeropad )
{
    if(!width){
        char c[8];
        uint32_t i = 0;
        do{
            c[i++] = ((val&0x0F)>0x09)?('A'+(val&0x0F)-0x0A)
                :('0'+(val&0x0F));
            val >>= 4;
        }
        while ( val != 0 );
        while ( i > 0 )
            putch(c[--i]);
    }else{
        char c[width];
        uint32_t i = 0;
        do{
            c[i++] = ((val&0x0F)>0x09)?('A'+(val&0x0F)-0x0A)
                :('0'+(val&0x0F));
            val >>= 4;
        }
        while ( (val != 0) && (i < width) );
        while ( i < width )
            c[i++] = (zeropad)?'0':' ';
        while ( i > 0 )
            putch(c[--i]);
    }
}

void putuns(uint32_t val, uint16_t width, uint8_t zeropad )
{
    if(!width){
        char c[10];
        uint32_t i = 0;
        do{
            c[i++] = '0'+val%10;
            val /= 10;
        }
        while ( val != 0 );
        while ( i > 0 )
            putch(c[--i]);
    }else{
        char c[width];
        uint32_t i = 0;
        do{
            c[i++] = '0'+val%10;
            val /= 10;
        }
        while ( (val != 0) && (i < width) );
        while ( i < width )
            c[i++] = (zeropad)?'0':' ';
        while ( i > 0 )
            putch(c[--i]);
    }
}

void vaprintvm(char *s, va_list args ){
    uint8_t alt;
    uint8_t zp;
    uint16_t wide;
    int i;

    for(i = 0; s[i] != 0x00; i++)
        ;

    while(*s){
        alt = 0;
        zp = 0;
        wide = 0;

        if(*s != '%'){
            putch(*(s++));
            continue;
        }
        s++;
        if(*s == '%'){
            putch(*(s++));
            continue;
        }
        if(*s == 's'){
            putstr((char*)va_arg(args, char *));
            s++;
            continue;
        }
        if(*s == 'c'){
            putch(va_arg(args, int));
            s++;
            continue;
        }
        if(*s == '#'){
            alt = 1;
            s++;
        }
        if(*s == '0'){
            zp = 1;
            s++;
        }
        while ( (*s >= '0') && (*s <= '9') ) /* field width */
            wide = wide*10+(*(s++)-'0');
        if(*s == 'd'){
            putdec((uint32_t)va_arg(args,uint32_t),wide,zp);
            s++;
            continue;
        }
        if(*s == 'u'){
            putuns((uint32_t)va_arg(args,uint32_t),wide,zp);
            s++;
            continue;
        }
        if(*s == 'x'){
            if(alt)
                putstr("0x");
            puthex((uint32_t)va_arg(args,uint32_t),wide,zp);
            s++;
            continue;
        }
        if(*s == 'o' ){
            if(alt)
                putch('0');
            putoct((uint32_t)va_arg(args,uint32_t),wide,zp);
            s++;
            continue;
        }
        if(*s == '\n'){
            cursor_y++;
            cursor_x = 0;
        }
        putch(*(s++));
    }
}

void printvmf(char *str, ...)
{
    va_list args;
    va_start(args, str);
    vaprintvm(str, args);
    va_end(args);
    return;
}
