#ifndef VIDEO_H
#define VIDEO_H

char ***video_ram;

int max_x;
int max_y;

void draw_screen(void);
void init_video(void);
void clear_framebuffer(char ***);
void alloc_video_memory(int max_x, int max_y);
void load_video_ram(int x, int y, char *value);


void putch(char);
void putstr(const char *);
void putoct(uint32_t, uint16_t, uint8_t);
void putdec(int, uint16_t, uint8_t);
void puthex(uint32_t, uint16_t, uint8_t);
void putuns(uint32_t, uint16_t, uint8_t);
void vaprintvmf(char *, va_list);
void printvmf(char *, ...);


#endif
