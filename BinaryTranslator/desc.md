### Task
The task is to generate an executable file using custom assembly.
To achieve this, we need to perform binary translation. In current project the executable format is __.elf__.

### Working progress
#### List of essential operations
To start with something simple, we want to create a program that successfully exits.  
```asm
.global _start
.intel_syntax noprefix
.text

_start:
    mov rdi, rax
    mov rax, 60
    syscall
``` 
So to translate this example into binary we need to generate codes for:
1. mov {reg, reg} | {reg, imm}
2. syscall

Even in a such small example we can already see that a single instruction can have different argument sets, so our next step is to understand how to distinguish them on the binary level.

#### Instruction encoding
As a reference we will use:
1. [Intel® 64 and IA-32 Architectures Software Developer’s Manual](https://software.intel.com/sites/default/files/managed/39/c5/325462-sdm-vol-1-2abcd-3abcd.pdf)
2. [x86 and amd64 instruction reference](https://www.felixcloutier.com/x86/).  

As we can see from (1) paragraph 2.1 instruction encoding consists of:
- REX prefix (maybe stands for register extension?)
- Opcode (Operation code)
- ModR/M (Argument types)
- SIB (Scale-Index-Base)
- Displacement (pointer offset_)
- Immediate (constants, long for imm)

##### REX prefix  
If we are using 64-bit mode instruction than this part is essential, so let's look at it closer.  
REX prefix is always 1 byte long and starts with 0100 (it's just a recognizable pattern). The next 4 bits are called W, R, X, B from fourth to the first.
 
Name | Definitoin
--- | --- |
W | Tells if operands are 64-bit or not |
R | Tells if operand in ModR/M (4-6 bits) is r8 ... r15 |
X | Needed for correct SIB handling |
B | Teels if operand in ModR/M (1-3 bits) is r8 ... r15|

##### Opcode
This is the easiest part as we just have to look up the right code in the documentation (2).

##### ModR/M
This part is 1-byte long and contains three parts. They are called __MOD__ (2 bits), __Reg__ (3 bits), __R/M__ (3 bits). Sometimes
__Reg__ is used to store additional opcode info, but we are not gonna use such instructions.
Now to determine values for these fields we should use Table 2-2 on page 530 from (1).

##### SIB
In this project we will not use SIB operations for simplicity. Therefore X in REX prefix always will be 0.

##### Displacement & Immediate
These fields just store the named value. Displacement can be 0, 1, 2 or 4 bytes and Immediate can be 0, 1, 2, 4 or 8 bytes.  

#### Small example
For a better understanding let's try to manually translate ```mov rax, 60``` into machine code.

As we use rax here, W in the REX Prefix will be 1. We don't use any r8 ... r15 registers so R and B are both 0. Finally,
concatenating WRXB with default 0100 we have 01001000b or 48h as a first byte.

Moving on to the opcode. Open the [MOV opcodes list](https://www.felixcloutier.com/x86/mov), and find the one that we need.
The only instruction from the list that fits our case is ```MOV r/m64, imm32```, so we remember its opcode is c7h.

Next one is ModR/M, and here we again just look at the table on page 530 from (1). Here we get c0h.

We don't have any displacement so it's 0 bytes for that. And the only thing left is Immediate which in our case is equal to 60
or 3ch.

Combining all of the above we get 48 c7 c0 3c. Now let's check our result.

Create .S file with one line ```mov rax, 60```, run
```shell script
as test.S -o test.o -msyntax=intel -mnaked-reg
objdump -d --disassembler-options=intel-mnemonic test.o
```
and check the result.

![Imgur Image](https://i.imgur.com/l56jxsa.png)

##### TODO
Preferable syntax: 
```
MOV(RAX, 60); 
MOV(RDI, 1);
MOV(RAX, RDI[60]);
MOV(RAX[1], RDI);
```
So all we need is 4 types of arguments and some variables of REG class (like REG RAX; ...).  
We will have class Instruction that will create REX prefix and ModR/M depending on arguments and methods like MOV, ADD and so on 
to generate specific opcodes.