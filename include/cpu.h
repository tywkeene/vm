#ifndef CPU
#define CPU

typedef enum{
    A, //0 - General purpose register
    B, //1 - General purpose register
    C, //2 - General purpose register
    D, //3 - General purpose register
    BAK, //4 - Immutable backup register, accessed with SAV/SWP instructions
    IP, //5 - Instruction pointer
    SP, //6 - Stack pointer
    BP, //7 - Stack base pointer
    REGISTER_SIZE // 8 - Number of registers
}register_set;

typedef enum{
    NOP, //0 - nop: No instruction
    ADD, //1 - add: adds the top two values on the stack, pushes the sum onto the stack
    PSH, //2 - psh <val>: pushes a single value onto the stack
    POP, //3 - pop: Pops a value off the stack into A
    HLT, //4 - hlt: Halts the program
    MOV, //5 - mov: mov <reg|val>, <reg>
    SAV, //6 - sav <reg>: saves value in register to the BAK register
    SWP, //7 - swp <reg>: moves the value in BAK to the register
    PSHA, //8 - psha: Push all registers onto the stack
    POPA, //9 - popa: Pop
    DMPR, //10 - dmpr A B: Dump ram from address in register A to address in register B
    DMPRE, //11 - dmpre: Dump registers
    LOAD, //12 - load: load <val> <address> Load value into memory address
    JMP, //13 - jmp: jmp <address> Jump to <address> unconditionally
    INC, //14 - inc <reg>: increment value in <reg>
    DEC, //15 - dec <reg>: decrement value in <reg>
    CMP, //16 - cmp <reg1> <reg2>: compare reg1 and reg2, push result onto stack
    JNE, //17 - jne <address>: jump to <address> if value in register D is not equal to value on top of stack
    JE, //18 - je <address>: jump to <address> if value in register D is equal to value on top of stack
    LOADV //19 - loadv <reg A = x> <reg B = y> <reg C = value> - load the value in register C to video memory at
         //      the location pointed to by register A (x) and register B (y)
}instruction_set;

typedef void (*operation_ptr)(void);

#define MEM_SIZE 64000
#define STACK_SIZE 1024
#define RAM_START (STACK_SIZE)
#define ROM_PROGRAM_START (STACK_SIZE)
#define EXECUTION_STACK_START 2048

int ram[MEM_SIZE];
int *stack = ram;
int *execution_stack = &ram[EXECUTION_STACK_START];
int registers[REGISTER_SIZE];
_Bool running = 1;

#define sp registers[SP]
#define bp registers[BP]
#define ip registers[IP]

void psh(void);
void add(void);
void pop(void);
void hlt(void);
void nop(void);
void mov(void);
void sav(void);
void swp(void);
void psha(void);
void popa(void);
void dmpr(void);
void dmpre(void);
void load(void);
void jmp(void);
void inc(void);
void dec(void);
void cmp(void);
void jne(void);
void je(void);
void loadv(void);

#endif
