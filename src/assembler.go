package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

var registers = map[string]byte{
	"a":   0,
	"b":   1,
	"c":   2,
	"d":   3,
	"bak": 4,
	"ip":  5,
	"sp":  6,
	"bp":  7,
}

var instructions = map[string]byte{
	"nop":   0,  //0 - nop: No instruction
	"add":   1,  //1 - add: adds the top two values on the stack, pushes the sum onto the stack
	"psh":   2,  //2 - psh <val>: pushes a single value onto the stack
	"pop":   3,  //3 - pop: Pops a value off the stack into A
	"hlt":   4,  //4 - hlt: Halts the program
	"mov":   5,  //5 - mov: mov <reg|val>, <reg>
	"sav":   6,  //6 - sav <reg>: saves value in register to the BAK register
	"swp":   7,  //7 - swp <reg>: moves the value in BAK to the register
	"psha":  8,  //8 - psha: Push all registers onto the stack
	"popa":  9,  //9 - popa: Pop
	"dmpr":  10, //10 - dmpr A B: Dump ram from address in register A to address in register B
	"dmpre": 11, //11 - dmpre: Dump registers
	"load":  12, //12 - load: load <val> <address> Load value into memory address
	"jmp":   13, //13 - jmp: jmp <address> Jump to <address> unconditionally
	"inc":   14, //14 - inc <reg>: increment value in <reg>
	"dec":   15, //15 - dec <reg>: decrement value in <reg>
	"cmp":   16, //16 - cmp <reg1> <reg2>: compare reg1 and reg2, push result onto stack
	"jne":   17, //17 - jne <address>: jump to <address> if value in register D is not equal to value on top of stack
	"je":    18, //18 - je <address>: jump to <address> if value in register D is equal to value on top of stack
	"loadv": 19, //19 - loadv <reg A = x> <reg B = y> <reg C = value> - load the value in register C to video memory at
	//      the location pointed to by register A (x) and register B (y)
}

func translateRegister(reg string) byte {
	if _, exists := registers[reg]; exists == true {
		//fmt.Printf("%s -> %d\n", reg, registers[reg])
		return instructions[reg]
	}
	return 0
}

func translateInstruction(instr string) byte {
	if _, exists := instructions[instr]; exists == true {
		//fmt.Printf("%s -> %d\n", instr, instructions[instr])
		return instructions[instr]
	}
	return 0
}

func readLines(path string) ([]byte, error) {
	file, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	var lines []string
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		lines = append(lines, scanner.Text())
	}
	for i, line := range lines {
		lines[i] = strings.Replace(line, ",", "", 2)
	}
	var program []byte
	for _, line := range lines {
		tokens := strings.Split(line, " ")
		//fmt.Println(tokens)
		for _, token := range tokens {
			if op := translateInstruction(token); op != 0 {
				program = append(program, op)
			} else if op := translateRegister(token); op != 0 {
				program = append(program, op)
			} else if strings.Contains(token, "$") == true {
				token = strings.Replace(token, "$", "", 2)
				integer, _ := strconv.Atoi(token)
				number := byte(integer)
				program = append(program, number)
			}
		}
	}
	return program, scanner.Err()
}

func main() {
	if os.Args[1] == "" {
		fmt.Printf("usage: %s <file.asm>\n", os.Args[0])
		return
	}

	program, err := readLines(os.Args[1])
	if err != nil {
		panic(err)
	}

	for _, op := range program {
		fmt.Printf("%d ", op)
	}
	fmt.Printf("\n")
}
