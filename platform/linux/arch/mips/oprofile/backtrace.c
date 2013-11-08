/*
 * Mips specific backtracing code for oprofile
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 *
 * Copyright (c) 2008 Wind River Systems, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Author: David Lerner <david.lerner@windriver.com>
 *
 * Based on i386 oprofile backtrace code by John Levon, David Smith modified
 * for mips.
 *
 */

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/oprofile.h>
#include <asm/uaccess.h>
#include <asm/stacktrace.h>
#include "mem_validate.h"
#include "mips_context.h"
#include "stack_crawl.h"
#include "context.h"
#include "backtrace.h"

#define MAX_DEPTH 64

/*
 * Indicate that backtrace logic failed
 */
#define BACKTRACE_ABORTED (0UL)

/*
 * op_is_syscall_ret()
 * returns non-zero if the return pc is within a system call handler
 * at a point where the user registers have been saved on the stack.
 */
#pragma weak handle_sys
#pragma weak ret_from_handle_sys
#pragma weak handle_sysn32
#pragma weak ret_from_handle_sysn32
#pragma weak handle_sys64
#pragma weak ret_from_handle_sys64

extern void handle_sys(void);
extern void ret_from_handle_sys(void);
extern void handle_sysn32(void);
extern void ret_from_handle_sysn32(void);
extern void handle_sys64(void);
extern void ret_from_handle_sys64(void);

static inline
int op_is_syscall_ret(unsigned long pc)
{
	return ((pc >= (unsigned long)handle_sys) &&
			(pc < (unsigned long)ret_from_handle_sys)) ||
	       ((pc >= (unsigned long)handle_sysn32) &&
			(pc < (unsigned long)ret_from_handle_sysn32)) ||
	       ((pc >= (unsigned long)handle_sys64) &&
			(pc < (unsigned long)ret_from_handle_sys64));
}

/*
 * op_kernel_backtrace()
 * returns a pointer to the user register set if op_user_backtrace should
 * proceed
 */
static
struct pt_regs *op_kernel_backtrace(struct pt_regs *const regs,
				    unsigned int *depth)
{
	/* taken from arch/mips/kernel/traps.c */
	unsigned long pc = regs->cp0_epc;

	if (__kernel_text_address(pc)) {
#ifdef CONFIG_KALLSYMS
		unsigned long sp = regs->regs[29];
		unsigned long ra = regs->regs[31];
	        int trace_thru_syscall = oprofile_get_trace_thru_syscall();
		while (pc && *depth) {
			--(*depth);
			if (trace_thru_syscall && op_is_syscall_ret(pc)) {
				struct pt_regs *uregs = (struct pt_regs *) sp;
				if (__kernel_text_address(uregs->cp0_epc)) {
					pc = uregs->cp0_epc;
					sp = uregs->regs[29];
					ra = uregs->regs[31];
				} else {
					oprofile_syscall_trace_boundary();
					oprofile_add_trace(uregs->cp0_epc);
					return uregs;
				}
			} else {
				pc = unwind_stack(current, &sp, pc, &ra);
			}
			oprofile_add_trace(pc);
		}
#endif
		return 0;
	}
	return regs;
}

/*
 * op_user_backtrace()
 * Initiate a stack crawl, analyzing text instructions, to locate
 * the next stack frame.  Log the discovered callstack pc addresses
 * to the oprofile cpu buffer.
 */
static
void op_user_backtrace(struct pt_regs *const regs, unsigned int *depth)
{
	struct memory_access_data membuf;
	struct frame_deltas frame;
	struct op_context child;
	struct op_context parent;
	int have_gprs = 1;	/* first pass through, we have gpr regs */
	unsigned long pc_sample;
	child.pc = (void *)regs->cp0_epc;
	child.sp = (void *)regs->regs[REG_SP];
	child.fp = (void *)regs->regs[REG_S8];
	child.gpregs[BT_REG_SP] = regs->regs[REG_SP];
	child.gpregs[BT_REG_FP] = regs->regs[REG_S8];
	child.gpregs[BT_REG_LR] = regs->regs[REG_RA];
	/*  child could be a leaf, impacting frame_crawl */
	child.leaf = true;

	while (*depth) {
		--(*depth);
		pc_sample = BACKTRACE_ABORTED;
		memset((void *)&frame, 0, (size_t) sizeof(frame));
		if (op_frame_crawl((void *)child.pc, &membuf, have_gprs,
				   &frame)) {
			/* Found next frame, sp & pc */
			if (apply_context_results(&child, &frame, &parent)) {
				if (parent.pc) {
					/* Frame looked good && pc > 0 */
					pc_sample = (unsigned long)parent.pc;
				}
			}
		}
		/* log the sample */
		oprofile_add_trace(pc_sample);
		child.pc = parent.pc;
		child.sp = parent.sp;
		child.fp = parent.fp;
		child.gpregs[BT_REG_SP] = (unsigned long)parent.sp;
		child.gpregs[BT_REG_FP] = (unsigned long)parent.fp;
		child.leaf = 0;
		have_gprs = 0;
		if (pc_sample == BACKTRACE_ABORTED)
			break;
	}
}

void mips_backtrace(struct pt_regs *const regs, unsigned int depth)
{
	struct pt_regs *uregs;
	if (depth > MAX_DEPTH)
		depth = MAX_DEPTH;
	uregs = op_kernel_backtrace(regs, &depth);
	if (uregs)
		op_user_backtrace(uregs, &depth);
}
