#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <curses.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "cpu.h"
#include "video.h"

const operation_ptr operations[] = {
    nop, add,
    psh, pop,
    hlt, mov,
    sav, swp,
    psha, popa,
    dmpr, dmpre,
    load, jmp,
    inc, dec,
    cmp, jne,
    je, loadv
};

char *instruction_strings[] = {
    "NOP", "ADD",
    "PSH", "POP",
    "HLT", "MOV",
    "SAV", "SWP",
    "PSHA", "POPA",
    "DMPR", "DMPRE",
    "LOAD", "JMP",
    "INC", "DEC",
    "CMP", "JNE",
    "JE", "LOADV"
};

char *register_strings[] ={"A","B","C","D","BAK", "IP", "SP", "BP"};

extern int max_x;
extern int max_y;

static void print_register_dump(void){
    int i;
    printvmf("REGISTERS ");
    for(i = 0; i < REGISTER_SIZE; i++){
        printvmf("[%s]=%d ", register_strings[i], registers[i]);
    }
    printvmf( "\n");
}

static void print_stack_dump(){
    int i;
    printvmf( "----------\n");
    for(i = 0; i < STACK_SIZE; i++){
        printvmf( "%d [%#08x]: %#08x [%d]\n", i, i, stack[i], stack[i]);
    }
    printvmf("----------\n");
}

static void print_ram_dump(int from, int to, bool full){
    int i;
    printvmf( "----------\n");
    for(i = from; i < to; i++){
        if(!full){
            if(ram[i] == 0)
                printvmf( "...\n");
            while(ram[i++] == 0){
                ;;
            }
        }
        printvmf( "ADDR[%#08x](%d): VALUE: [%#08x] (%d)", i,i, ram[i], ram[i]);
        switch(i){
            case 0:
                printvmf( " STACK START\n");
                break;
            case ROM_PROGRAM_START:
                printvmf( " ROM PROGRAM START\n");
                break;
            case EXECUTION_STACK_START:
                printvmf( " EXECUTION STACK START\n");
                break;
            default:
                printvmf( "\n");
        }
    }
    printvmf("----------\n");
}

void psh(void){
    int value = ram[ip+1];
    stack[sp+1] = value;
    sp++;
    ip++;
    return;
}

void add(void){
    int a, b, sum;
    a = stack[sp-1];
    b = stack[sp-2];
    sum = a + b;
    sp--;
    stack[sp] = sum;
    printvmf( "%s (%d + %d = %d)\n",
            instruction_strings[ADD], a, b, sum);
    return;
}

void pop(void){
    printvmf( "(POP)\n");
    sp--;
}

void hlt(void){
    printvmf( "(HLT)\n");
    running = false;
}

void nop(void){
    printvmf( "(NOP)");
    return;
}

void mov(void){
    int val = ram[ip+1];
    int reg = ram[ip+2];
    registers[reg] = val;
    printvmf( "(MOV %d -> REG [%s])\n", val, register_strings[reg]);
    ip+=2;
}

void sav(void){
    int reg;
    reg = ram[ip+1];
    registers[BAK] = registers[reg];
    printvmf( "(SAV REG [%s])\n", register_strings[reg]);
}

void swp(void){
    int reg;
    reg = ram[ip+1];
    registers[reg] = registers[BAK];
    printvmf( "(SWP REG [%s])\n", register_strings[reg]);
}

void psha(void){
    for(int reg = 0; reg < 4; reg++){
        sp++;
        stack[sp] = registers[reg];
        registers[reg] = 0;
    }
    printvmf( "(PSHA)\n");
    print_stack_dump();
}

void popa(void){
    for(int reg = 3; reg > -1; --reg){
        registers[reg] = stack[sp];
        stack[sp] = 0;
        sp--;
    }
    printvmf( "(POPA)\n");
}

void dmpr(void){
    printvmf( "(DMPR)\n");
    print_ram_dump(registers[A], registers[B], true);
}

void dmpre(void){
    printvmf( "(DMPRE)\n");
    print_register_dump();
}

void load(void){
    int address;
    int val;
    val = ram[ip+1];
    address = ram[ip+2];
    ram[address] = val;
    printvmf( "(LOAD %d -> ADDR [%#08x](%d))\n", val, address, address);
    print_ram_dump(address, address+1, true);
    ip+=2;
}

void jmp(void){
    int addr = ram[ip+1];
    ip = addr;
    printvmf( "(JMP -> ADDR[%#08x](%d))\n", addr, addr);
}

void inc(void){
    int reg = ram[ip+1];
    registers[reg]+=1;
    printvmf( "(INC REG [%s])\n", register_strings[reg]);
    ip++;
}

void dec(void){
    int reg = ram[ip+1];
    registers[reg]-=1;
    printvmf( "(DEC REG [%s])\n", register_strings[reg]);
    ip++;
}

void cmp(){
    bool result;
    int reg1 = ram[ip+1];
    int reg2= ram[ip+2];
    result = (registers[reg1] == registers[reg2]);
    sp++;
    stack[sp] = result;
    ip+=2;
    printvmf( "(CMP REG [%s] REG [%s] RESULT %d)\n",
            register_strings[reg1], register_strings[reg2], result);
}

void jne(void){
    int address = ram[ip+1];
    bool result = (registers[A] != registers[B]);
    if(result == true)
        ip = address-1;
    else
        ip++;
    printvmf( "(JNE -> ADDR[%#08x](%d) (REG [%s] != REG [%s]) == %d)\n",
            address, address, register_strings[A], register_strings[B], result);
}

void je(void){
    int address = ram[ip+1];
    bool result = (registers[A] == registers[B]);
    if(result == true)
        ip = address-1;
    else
        ip++;
    printvmf( "(JE -> ADDR[%#08x](%d) (REG [%s] == REG [%s]) == %d)\n",
            address, address, register_strings[A], register_strings[B], result);
}

void loadv(void){
    int x = registers[A];
    int y = registers[B];
    int value = registers[C];
    char *buffer = malloc(1);
    memcpy(buffer, &value, 1);
    load_video_ram(x, y, buffer);
    free(buffer);
    printvmf("(LOADV %#02x -> VRAM[%#08x](%d))\n",
            value, &video_ram[x][y], &video_ram[x][y]);
}

static void load_program(int program_start, int program_size, int instruction_array[]){
    int instruction;
    for(int i = 0, address = program_start; i < program_size; i++, address++){
        instruction = instruction_array[i];
        ram[address] = instruction;
    }
}

void execute(int *program){
    int instruction = program[ip];
    operation_ptr op = operations[instruction];
    printvmf( "ADDR [%#08x](%d)-> ", ip, ip);
    op();
}

#define getmaxxy(scr,x,y) getmaxyx(scr,y,x)
#define sizeof_array(x) (sizeof(x) / sizeof(x[0]))

char *copy_string(const char *str)
{
	char *new = calloc(1, strlen(str) + 1);
	strncpy(new, str, strlen(str) + 1);
	return new;
}

unsigned int count_token(char *string, const char *token_string)
{
	unsigned int count = 0;
	char *token;
	char *copy;
	copy = copy_string(string);
	for(token = strtok(copy, token_string); token != NULL;
			token = strtok(NULL, token_string))
		count++;
	free(copy);
	return count;
}

char *alloc_program_buffer(char *file_name){
    char *buffer;
    struct stat info;
    int fd = open(file_name, O_RDONLY);
    fstat(fd, &info);
    close(fd);
    printvmf("%d\n", info.st_size);
    getchar();
    buffer = malloc(sizeof(char) * info.st_size);

    FILE *file = fopen(file_name, "r");
    fgets(buffer, info.st_size, file);
    fclose(file);
    return buffer;
}

int *read_binary_file(char* file_name){
    int *program;
    char *buffer;
    int byte_count;
    buffer = alloc_program_buffer(file_name);
    byte_count = count_token(buffer, " ");
    printvmf("byte count: %d\n", byte_count);
    getchar();
    return program;
}

int main(void){
    init_video();
    getmaxxy(stdscr, max_x, max_y);
    alloc_video_memory(max_x,max_y);
    clear_framebuffer(video_ram);

    ip = ROM_PROGRAM_START;
    sp = -1;
    bp = -1;

    int *rom_program = read_binary_file("file.out");

    load_program(ROM_PROGRAM_START, sizeof_array(rom_program), rom_program);
    print_ram_dump(ROM_PROGRAM_START, ROM_PROGRAM_START+32, true);
    while(running){
        print_register_dump();
        execute(ram);
        ip++;
        draw_screen();
        getchar();
    }
    print_register_dump();
    print_ram_dump(0, 5, true);
    getchar();
    endwin();
    return 0;
}
