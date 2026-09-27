//2 operandos
#define OPCODE_MOV  0x10
#define OPCODE_ADD  0x11
#define OPCODE_SUB  0x12
#define OPCODE_MUL  0x13
#define OPCODE_DIV  0x14
#define OPCODE_CMP  0x15
#define OPCODE_AND  0x16
#define OPCODE_OR   0x17
#define OPCODE_XOR  0x18
#define OPCODE_SWAP 0x19
#define OPCODE_SHL  0x1A
#define OPCODE_SHR  0x1B
#define OPCODE_SAR  0x1C
#define OPCODE_LDL  0x1D
#define OPCODE_LDH  0x1E
#define OPCODE_RND  0x1F

// 1 operando
#define OPCODE_SYS  0x00
#define OPCODE_JMP  0x01
#define OPCODE_JP   0x02
#define OPCODE_JN   0x03
#define OPCODE_JZ   0x04
#define OPCODE_JC   0x05
#define OPCODE_JV   0x06
#define OPCODE_JNP  0x07
#define OPCODE_JNN  0x08
#define OPCODE_JNZ  0x09
#define OPCODE_NOT  0x0A

// 0 operandos
#define OPCODE_STOP 0x0F