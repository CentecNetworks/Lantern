/*
 * Mips specific backtracing code for oprofile
 *
 * Copyright (c) 2008 Windriver Systems, Inc.
 *
 * Author: David Lerner <david.lerner@windriver.com>
 *
 * Based on i386 oprofile backtrace code by John Levon, David Smith
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef BACKTRACE_H
#define BACKTRACE_H

/*
 * The mips backtrace crawl - this may take a while since
 * frame pointers are of no value, and the code traces and follows
 * instructions to find function epilogs or prologs.
 */
extern void mips_backtrace(struct pt_regs *const regs, unsigned int depth);

#if defined(CONFIG_64BIT)
/*
 * Callback from do_page_fault and do_ade to detect if
 * page fault occured during an oprofile_backtrace.
 * returns 1 if caller should not handle the fault
 *   (caused by oprofile backtracing)
 * returns 0 if the caller should handle the fault
 */
extern unsigned int (*is_oprofile_fault)(struct pt_regs *regs);

/*
 * Callback from do_page_fault and do_ade to set return
 * of oprofile user space address valididator to false
 */
extern unsigned int op_page_fault_filter(struct pt_regs *regs);
#endif

#endif /* BACKTRACE_H */
