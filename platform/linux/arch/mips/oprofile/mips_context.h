/*
 *  mips-context.h - MIPS-specific context information.h
 *
 * Copyright (c) 2008 Windriver Systems, Inc.
 *
 * Author: David Lerner <david.lerner@windriver.com>
 *         Wind River scopetools developers
 *
 * This code is closely adapted from the improved backtrace 'context' analysis
 * code that was developed to be shared by profiler and debugger
 * products, host and target based for various Wind River Systems, Inc products.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * (The file was also known as mips_regs.h in scopetools source tree)
 */

#ifndef _MIPS_CONTEXT_H_
#define _MIPS_CONTEXT_H_

/*
 * Architecture specific defines
 */

/*
 * Instruction search range from the original input PC is limited to + or -
 * RTITOOLS_CONTEXT_MAX_FN_LENGTH instructions.
 */
#define RTITOOLS_CONTEXT_MAX_FN_LENGTH (2048)

typedef unsigned long tAddr;	/* Address type */

#define MIPS_ABI_O32    (1)
#define MIPS_ABI_N32    (2)

#  define MIPS_ABI      MIPS_ABI_N32	/* dml hack */

#if (MIPS_ABI == MIPS_ABI_O32)
#  define STACK_ALIGN     (4)
#  define FRAME_ALIGN     (4)
#  define MIN_STACK_WIDTH (4)
#elif (MIPS_ABI == MIPS_ABI_N32)
/* 64-bit N32 stack alignment is 8 bytes, but VxWorks only
 * supports 32-bit addresses, so allow 4-byte alignment.
 */
#  define STACK_ALIGN     (4)
#  define FRAME_ALIGN     (8)
#  define MIN_STACK_WIDTH (8)
#endif

/* big endian 64-bit targets require a 4-byte register offset. */
#if defined(__KERNEL__)
#	if defined(CONFIG_64BIT) && defined(CONFIG_CPU_BIG_ENDIAN)
		/* dml - setting per spirit of original code,
		   define REGISTER_OFFSET (4)
		   only made the word swapping problem seen in test worse.
		   Requires further investigaion.
		 */
#		define REGISTER_OFFSET (0)
#	else
#		define REGISTER_OFFSET (0)
#	endif
#else
# error should not get here
#	if defined(RTI_ENDIAN_BIG) && defined(MIPS_64BIT)
#  		define REGISTER_OFFSET (4)
#	else
#  		define REGISTER_OFFSET (0)
#	endif
#endif

#define MIN_INST_WIDTH  (4)

#define INITIAL_STK_FRAME_SIZE  (MAX_TASK_ARGS * sizeof(_RType))

/* Number of back branches before we stop taking them */
#define MAX_ARB_BRANCHES         10

typedef unsigned int tInst;	/* Instruction type */
typedef int rOffset;		/* instruction register offset field */
#define INST_SIZE (4)		/* Size of instr -- avoid sizeof() overhead */

/*
 *  Parameters - define context parameters here
 */

/* How many instructions to look backwards */
#define MAX_BACKWARD_SEARCH      10

/* Instructions before we assume we're not in the prologue.  This typically
   includes just the instructions between the stack push
   (addiu    sp,sp,-96) and saving the link register (sw     ra,56(sp)).*/
#define MAX_PROLOGUE_LENGTH      24

/* How long is the epilogue? [until restore fp] */
#define MAX_EPILOGUE_LENGTH      35

#if (MAX_PROLOGUE_LENGTH > MAX_BACKWARD_SEARCH)
#if (MAX_PROLOGUE_LENGTH > MAX_EPILOGUE_LENGTH)
#define MAX_BACKWARD_LENGTH      (MAX_PROLOGUE_LENGTH * INST_SIZE)
#else
#define MAX_BACKWARD_LENGTH      (MAX_EPILOGUE_LENGTH * INST_SIZE)
#endif
#else
#if (MAX_BACKWARD_SEARCH > MAX_EPILOGUE_LENGTH)
#define MAX_BACKWARD_LENGTH      (MAX_BACKWARD_SEARCH * INST_SIZE)
#else
#define MAX_BACKWARD_LENGTH      (MAX_EPILOGUE_LENGTH * INST_SIZE)
#endif
#endif

#define MAX_FORWARD_LENGTH           (4 * INST_SIZE)
#define MAX_SEARCH_LENGTH            (MAX_BACKWARD_LENGTH + \
					MAX_FORWARD_LENGTH)

/*
 *  Instruction values - define any instruction op-codes here
 */
#define INST_UNKNOWN       (0xffffffff)

#define INST_ADD_TO_SP     (0x23bd0000)	/* addi/addiu sp,sp,immed */
#define MASK_ADD_OFFSET    (0x0000ffff)	/* Signed 16-bit offset added to SP */
#define MASK_NEG_OFFSET    (0x00008000)
#define NEG_SIGN_EXTEND     0xffff0000
#define MASK_ADD_OPCODE    (0xfbff0000)	/* ignore addi/addiu differences */

#define MASK_OPCODE         0xfc000000	/* MIPS major op codes are 1st 6 bits */
#define INST_ADDIU          0x24000000	/* addiu rd,rs,immed */
#define INST_LUI            0x3c000000	/* lui   rd,immed    */

#define MASK_BASIC_OPCODE   0xfc0007ff	/* add, sub, mult, div, jr, mov, etc. */
#define INST_ADDU           0x00000021	/* addu  rd,rs,rt    */
#define INST_NOP            0x00000000

/* Convert mips' signed 16-bit immediate instruction-embedded values
 * into signed int (32-bit) values. Note that ints are still 32 bits
 * for MIPS64.
 */
#define IMM_TO_INT(inst) \
(signed int) (((inst) & MASK_NEG_OFFSET) ?\
 (((inst) & MASK_ADD_OFFSET) | NEG_SIGN_EXTEND) :\
 ((inst) & MASK_ADD_OFFSET))

#define INST_STORE_RA_64   (0xffbf0000)	/* sd  ra,offset(sp) */
#define INST_RESTORE_RA_64 (0xdfbf0000)	/* ld  ra,offset(sp) */
#define INST_STORE_RA_32   (0xafbf0000)	/* sw  ra,offset(sp) */
#define INST_RESTORE_RA_32 (0x8fbf0000)	/* lw  ra,offset(sp) */

#define MASK_SWLW_OFFSET   (0x0000ffff)	/* Store/load word offset */
#define MASK_SWLW_OPCODE   (0xffff0000)
#define MASK_SWLW_FP_SP    (0xffff8000)
#define INST_SW_FP_SP      (0xafbe0000)	/* sw  fp,+d(sp) */
#define INST_LW_FP_SP      (0x8fbe0000)	/* lw  fp,+d(sp) */
#define INST_FP_TO_SP      (0x03c0e821)	/* move sp,fp  [move sp,s8] */
#define INST_SP_TO_FP      (0x03a0f021)	/* move fp,sp  [move s8,sp] */
#define MASK_LX_OPCODE     (0xfc0007ff)
#define INST_LBUX          (0x7c00018a)	/* LBUX Rd,Rindx(Rbase) DSP ASE only */
#define INST_LHX           (0x7c00010a)	/* LHX  Rd,Rindx(Rbase) DSP ASE only */
#define INST_LWX           (0x7c00000a)	/* LWX  Rd,Rindx(Rbase) DSP ASE only */

#define MASK_NEG_OFFSET    (0x00008000)	/* for all 16-bit signed offsets */

/* Unconditional branch instructions */
#define INST_JR_REG        (0x00000008)	/* jr t1, for example */
#define INST_JR_T9_REG     (0x03200008)	/* jr   t9 */
#define MASK_JR_REG        (0xfc1ff83f)	/* ick */
#define INST_J             (0x08000000)	/* First six bits */
#define INST_JAL           (0x0c000000)	/* First six bits */
#define MASK_JUMP_OFFSET   (0x03ffffff)	/* Mask off the first six bits */
#define MASK_JUMP_OPCODE   (0xfc000000)

/* Conditional branch instructions */
#define INST_BEQ           (0x10000000)	/* All branches execute delay slot */
#define INST_B             (0x10000000)	/* B is BEQ 0, 0, offset */
#define INST_BNE           (0x14000000)
#define INST_BLEZ          (0x18000000)
#define INST_BGTZ          (0x1c000000)
#define INST_BPOSGE32      (0x041c0000)	/* BPOSGE32  (DSP ASE only) */
#define MASK_BRANCH_OFFSET (0x0000ffff)	/* Branch offset */
#define MASK_BRANCH_OPCODE (0xfc000000)
#define MASK_B_OPCODE      (0xffff0000)

#define INST_RTS           (0x03e00008)	/* jr ra */

#define INST_BRANCH_SELFPT (0x1000ffff)

/* eret instruction */
#define INST_ERET          (0x42000018)

#define CAUSE_EXCCODE_MASK (0x0000007c)
#define CAUSE_INTRPT_CODE  (0)
#define CAUSE_SYSCL_CODE   (8)

/*
 *  Context flags - define arch specific context package flags here
 */
/* CHECK_TOP_FIRST if you want to call RtiTools_ContextTopDiscover at the
 * beginning of RtiTools_ContextCallingContextGet; CHECK_TOP_LAST if you
 * want to call it at the end.  You can use both, don't unless you
 * have to.
 */
#define RTITOOLS_CONTEXT_CHECK_TOP_FIRST        (1)
#define RTITOOLS_CONTEXT_CHECK_TOP_LAST         (0)

#define RTITOOLS_CONTEXT_USES_SP  (1)	/* Does context package use sp? */
#define RTITOOLS_CONTEXT_USES_FP  (1)	/* Does context package use fp? */

/* Register numbers for op_context */
enum register_name {
	BT_REG_LR,
	BT_NUM_GPREGS,
	BT_REG_SP,
	BT_REG_FP,
	BT_REG_PC
};

/* Register numbers for struct pt_regs */
enum reg_num {
	REG_ZERO, REG_AT, REG_V0, REG_V1,
	REG_A0, REG_A1, REG_A2, REG_A3,
	REG_T0, REG_T1, REG_T2, REG_T3,
	REG_T4, REG_T5, REG_T6, REG_T7,
	REG_S0, REG_S1, REG_S2, REG_S3,
	REG_S4, REG_S5, REG_S6, REG_S7,
	REG_T8, REG_T9, REG_K0, REG_K1,
	REG_GP, REG_SP, REG_S8, REG_RA,
	REG_ALL			/* This last one is the number of GPRs */
};

#endif /* ifndef _MIPS_CONTEXT_H_ */
