/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only change sim.c!                       */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#ifndef _SIM_SHELL_H_
#define _SIM_SHELL_H_

#include <stdint.h>

#define FALSE 0
#define TRUE  1

#define MIPS_REGS 32
//涵盖要实现的所有指令的op字段以及一些标识符字段 MIPS R4000
//instruction:op(6位)+rs(5位)+rt(5位)+rd(5位)+shamp(5位)+funct(6位)
enum{
  OP_J = 2,
  OP_JAL = 3,
  OP_BEQ = 4,
  OP_BNE = 5,
  OP_BLEZ = 6,
  OP_BGTZ = 7,
  OP_ADDI = 8,
  OP_ADDIU = 9,
  OP_SLTI = 10,
  OP_SLTIU = 11,
  OP_ANDI = 12,
  OP_ORI = 13,
  OP_XORI = 14,
  OP_LUI = 15,
  OP_LB = 32,
  OP_LH = 33,
  OP_LW = 35,
  OP_LBU = 36,
  OP_LHU = 37,
  OP_SB = 40,
  OP_SH = 41,
  OP_SW = 43,
  OP_REGIMM = 1,
  //下面为同类型指令 op为1，记录rt位置的字段作为标识(rt的位置，而不是说真的有rt)
  rt_BLTZ = 0, //rt段为0  op段为REGIMM是1
  rt_BGEZ = 1, //rt段的5位是1，op段为REGIMM是1
  rt_BLTZAL = 16, //rt段的5位是16
  rt_BGEZAL = 17,//rt段的5位是17
  OP_SPECIAL = 0, 
  //下面为同类型(op相同指令),R_ALU指令，下面记录funct字段
  funct_SLL = 0,
  funct_SRL = 2,
  funct_SRA = 3,
  funct_SLLV = 4,
  funct_SRLV = 6,
  funct_SRAV = 7,
  funct_JR = 8, 
  funct_JALR = 9,
  funct_ADD = 32,
  funct_ADDU = 33,
  funct_SUB = 34,
  funct_SUBU = 35,
  funct_AND = 36,
  funct_OR = 37,
  funct_XOR = 38,
  funct_NOR = 39,
  funct_SLT = 42,
  funct_SLTU = 43,
  funct_MULT = 24,
  funct_MFHI = 16,
  funct_MFLO = 18,
  funct_MTHI = 17,
  funct_MTLO = 19,
  funct_MULTU = 25,
  funct_DIV = 26,
  funct_DIVU = 27,
  funct_SYSCALL = 12,
};

typedef struct CPU_State_Struct {

  uint32_t PC;		/* program counter */
  uint32_t REGS[MIPS_REGS]; /* register file. */
  uint32_t HI, LO;          /* special regs for mult/div. */
} CPU_State;

/* Data Structure for Latch */

extern CPU_State CURRENT_STATE, NEXT_STATE;

extern int RUN_BIT;	/* run bit */

uint32_t mem_read_32(uint32_t address);
void     mem_write_32(uint32_t address, uint32_t value);

/* YOU IMPLEMENT THIS FUNCTION */
void process_instruction();

#endif
